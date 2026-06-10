"""
Haptic Terrain Game - HW17
Reads raw angle integers from Pico over serial, moves character along terrain.

Usage:
    pip install pygame pyserial
    python haptic_game.py

    Edit SERIAL_PORT to match your Pico's port.
    Edit TERRAIN_POINTS to add hills/bumps for HW18.
"""

import pygame
import serial
import threading
import sys
import math

# ---------------------------------------------------------------------------
# Config
# ---------------------------------------------------------------------------

SERIAL_PORT  = "COM4"        # Windows: "COM3" etc. Mac/Linux: "/dev/ttyACM0"
BAUD_RATE    = 115200

ANGLE_MIN    = 2000             # Raw angle value at one end of paddle travel
ANGLE_MAX    = 3500          # Raw angle value at other end (~70 deg * 4096/360 ≈ 796)

WIDTH, HEIGHT = 1100, 500
FPS           = 60

# ---------------------------------------------------------------------------
# Terrain definition
# Each point is (x_fraction, y_fraction) where:
#   x: 0.0 = left end, 1.0 = right end of the track
#   y: 0.0 = top of screen, 1.0 = bottom  (so 0.5 = middle, 0.7 = lower)
# Add more points to create hills, bumps, dips, etc. for HW18.
# ---------------------------------------------------------------------------

TERRAIN_POINTS = [
    (0.0,  0.55),
    (0.15, 0.55),
    (0.3,  0.55),
    (0.45, 0.55),
    (0.6,  0.55),
    (0.75, 0.55),
    (0.9,  0.55),
    (1.0,  0.55),
]

# ---------------------------------------------------------------------------
# Colors  (dark cockpit palette — feels mechanical/hardware-ish)
# ---------------------------------------------------------------------------

BG          = ( 18,  20,  28)
GROUND_FILL = ( 30,  34,  48)
TERRAIN_COL = ( 80, 180, 140)   # teal line
CHAR_COL    = (240, 200,  60)   # amber character
CHAR_WHEEL  = ( 60,  60,  80)
ACCENT      = ( 80, 180, 140)
TEXT_COL    = (160, 170, 190)
ANGLE_COL   = (100, 140, 220)

# ---------------------------------------------------------------------------
# Serial reader (runs in background thread)
# ---------------------------------------------------------------------------

latest_angle = [None]   # shared between threads
serial_ok    = [False]

def serial_thread(port, baud):
    try:
        ser = serial.Serial(port, baud, timeout=1)
        serial_ok[0] = True
        while True:
            line = ser.readline().decode("utf-8", errors="ignore").strip()
            if line.isdigit():
                latest_angle[0] = int(line)
    except Exception as e:
        serial_ok[0] = False
        print(f"Serial error: {e}")

# ---------------------------------------------------------------------------
# Terrain helpers
# ---------------------------------------------------------------------------

def build_terrain_px(points, w, h):
    """Convert normalised terrain points to pixel coords."""
    return [(int(x * w), int(y * h)) for x, y in points]

def terrain_y_at(px_x, terrain_px):
    """Linearly interpolate terrain height (y) at a given pixel x."""
    for i in range(len(terrain_px) - 1):
        x0, y0 = terrain_px[i]
        x1, y1 = terrain_px[i + 1]
        if x0 <= px_x <= x1:
            t = (px_x - x0) / (x1 - x0) if x1 != x0 else 0
            return y0 + t * (y1 - y0)
    # clamp to ends
    if px_x <= terrain_px[0][0]:
        return terrain_px[0][1]
    return terrain_px[-1][1]

def terrain_slope_at(px_x, terrain_px):
    """Return rise/run slope at px_x (used later for haptic force)."""
    for i in range(len(terrain_px) - 1):
        x0, y0 = terrain_px[i]
        x1, y1 = terrain_px[i + 1]
        if x0 <= px_x <= x1:
            return (y1 - y0) / (x1 - x0) if x1 != x0 else 0
    return 0

def draw_terrain(surf, terrain_px, h):
    """Fill below terrain and draw the surface line."""
    if len(terrain_px) < 2:
        return
    poly = list(terrain_px) + [(terrain_px[-1][0], h), (terrain_px[0][0], h)]
    pygame.draw.polygon(surf, GROUND_FILL, poly)
    pygame.draw.lines(surf, TERRAIN_COL, False, terrain_px, 3)

def draw_character(surf, cx, cy, wheel_r=10):
    """Draw a simple wheeled character (circle body + two wheels)."""
    body_r = 14
    # wheels
    pygame.draw.circle(surf, CHAR_WHEEL, (int(cx - body_r + 4), int(cy + body_r - 2)), wheel_r)
    pygame.draw.circle(surf, CHAR_WHEEL, (int(cx + body_r - 4), int(cy + body_r - 2)), wheel_r)
    # body
    pygame.draw.circle(surf, CHAR_COL, (int(cx), int(cy)), body_r)
    # eye
    pygame.draw.circle(surf, BG, (int(cx + 5), int(cy - 3)), 3)

# ---------------------------------------------------------------------------
# HW18 hook: compute desired force from terrain slope
# Returns a value in [-1, 1]; positive = resist rightward movement
# ---------------------------------------------------------------------------

def haptic_force(slope):
    """
    For HW18: map terrain slope to a normalised force value.
    slope > 0 means going downhill in pixel space (y increases downward),
    so uphill in visual space. Resist going uphill.
    You can replace or extend this with a lookup table or more complex curve.
    """
    force = -slope * 5.0          # scale factor — tune to taste
    return max(-1.0, min(1.0, force))

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    # Start serial thread (non-blocking — game runs even without serial)
    t = threading.Thread(target=serial_thread, args=(SERIAL_PORT, BAUD_RATE), daemon=True)
    t.start()

    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("Haptic Terrain  |  HW17")
    clock  = pygame.time.Clock()

    font_big  = pygame.font.SysFont("Consolas", 22)
    font_small= pygame.font.SysFont("Consolas", 15)

    terrain_px = build_terrain_px(TERRAIN_POINTS, WIDTH, HEIGHT)

    # Track range of seen angles for auto-calibration if needed
    angle_seen_min = [ANGLE_MIN]
    angle_seen_max = [ANGLE_MAX]

    simulated_x = 0.0   # fallback when no serial: mouse-driven
    use_mouse_fallback = False

    while True:
        dt = clock.tick(FPS) / 1000.0

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit(); sys.exit()
            if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                pygame.quit(); sys.exit()

        # ---- Map encoder angle → x position --------------------------------
        raw = latest_angle[0]
        if raw is not None:
            use_mouse_fallback = False
            clamped = max(ANGLE_MIN, min(ANGLE_MAX, raw))
            t_val   = (clamped - ANGLE_MIN) / (ANGLE_MAX - ANGLE_MIN)
        else:
            use_mouse_fallback = True
            mx, _  = pygame.mouse.get_pos()
            t_val  = mx / WIDTH

        # Map t_val (0–1) to pixel x within terrain range
        x_left  = terrain_px[0][0]
        x_right = terrain_px[-1][0]
        char_x  = x_left + t_val * (x_right - x_left)
        char_y  = terrain_y_at(char_x, terrain_px) - 16   # sit on top of terrain

        slope = terrain_slope_at(char_x, terrain_px)
        force = haptic_force(slope)

        # ---- Draw -----------------------------------------------------------
        screen.fill(BG)
        draw_terrain(screen, terrain_px, HEIGHT)
        draw_character(screen, char_x, char_y)

        # HUD
        serial_status = "SERIAL OK" if serial_ok[0] else ("NO SERIAL (mouse mode)" if use_mouse_fallback else "CONNECTING...")
        serial_color  = ACCENT if serial_ok[0] else (220, 100, 80)

        angle_text = f"Angle raw: {raw if raw is not None else '---':>5}"
        force_text = f"Force:     {force:+.2f}"
        pos_text   = f"Position:  {t_val * 100:.1f}%"

        screen.blit(font_big.render(serial_status,     True, serial_color), (20, 16))
        screen.blit(font_small.render(angle_text,      True, ANGLE_COL),    (20, 50))
        screen.blit(font_small.render(pos_text,        True, TEXT_COL),     (20, 70))
        screen.blit(font_small.render(force_text,      True, TEXT_COL),     (20, 90))

        hint = "ESC to quit  |  Edit TERRAIN_POINTS to add hills for HW18"
        screen.blit(font_small.render(hint, True, (70, 80, 100)), (20, HEIGHT - 24))

        # Force bar (preview of haptic output)
        bar_x, bar_y, bar_w, bar_h = WIDTH - 180, 16, 140, 14
        pygame.draw.rect(screen, (40, 45, 60), (bar_x, bar_y, bar_w, bar_h))
        mid = bar_x + bar_w // 2
        fill_w = int(abs(force) * bar_w / 2)
        fill_x = mid if force >= 0 else mid - fill_w
        bar_color = (220, 100, 80) if force > 0 else (100, 160, 220)
        if fill_w > 0:
            pygame.draw.rect(screen, bar_color, (fill_x, bar_y, fill_w, bar_h))
        pygame.draw.line(screen, TEXT_COL, (mid, bar_y - 2), (mid, bar_y + bar_h + 2), 1)
        screen.blit(font_small.render("force", True, TEXT_COL), (bar_x, bar_y + 18))

        pygame.display.flip()

if __name__ == "__main__":
    main()