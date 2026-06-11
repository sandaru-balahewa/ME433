import pygame
import serial
import threading
import sys

# ---------------------------------------------------------------------------
# Config
# ---------------------------------------------------------------------------

SERIAL_PORT  = "COM4"
BAUD_RATE    = 115200

ANGLE_MIN    = 2000
ANGLE_MAX    = 3500

# Haptic thresholds — must match your #defines in pico_main.c
ANGLE_WALL_LOW   = 2050
ANGLE_FREE_LOW   = 2350
ANGLE_FREE_HIGH  = 3150
ANGLE_WALL_HIGH  = 3450

WIDTH, HEIGHT = 1200, 700
FPS           = 60

# ---------------------------------------------------------------------------
# Terrain definition
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
# Colors
# ---------------------------------------------------------------------------

BG           = ( 18,  20,  28)
GROUND_FILL  = ( 23,  77,   7)
TERRAIN_COL  = ( 50, 168,  60)
CHAR_COL     = (240, 200,  60)
CHAR_WHEEL   = ( 60,  60,  80)
ACCENT       = ( 80, 180, 140)
TEXT_COL     = (160, 170, 190)
ANGLE_COL    = (100, 140, 220)

WALL_COL         = (200,  40,  40)   # hard wall — red
RAMP_COL         = (210, 120,  20)   # ramp zone — orange
FREE_ZONE_COL    = ( 60, 180,  80)   # free zone — green
ZONE_ALPHA       = 60                # overlay transparency

# ---------------------------------------------------------------------------
# Serial reader
# ---------------------------------------------------------------------------

latest_angle = [None]
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
# Helpers: angle -> screen x
# ---------------------------------------------------------------------------

def angle_to_x(angle, terrain_px):
    x_left  = terrain_px[0][0]
    x_right = terrain_px[-1][0]
    t = (max(ANGLE_MIN, min(ANGLE_MAX, angle)) - ANGLE_MIN) / (ANGLE_MAX - ANGLE_MIN)
    return int(x_left + t * (x_right - x_left))

# ---------------------------------------------------------------------------
# Terrain helpers
# ---------------------------------------------------------------------------

def build_terrain_px(points, w, h):
    return [(int(x * w), int(y * h)) for x, y in points]

def terrain_y_at(px_x, terrain_px):
    for i in range(len(terrain_px) - 1):
        x0, y0 = terrain_px[i]
        x1, y1 = terrain_px[i + 1]
        if x0 <= px_x <= x1:
            t = (px_x - x0) / (x1 - x0) if x1 != x0 else 0
            return y0 + t * (y1 - y0)
    if px_x <= terrain_px[0][0]:
        return terrain_px[0][1]
    return terrain_px[-1][1]

def terrain_slope_at(px_x, terrain_px):
    for i in range(len(terrain_px) - 1):
        x0, y0 = terrain_px[i]
        x1, y1 = terrain_px[i + 1]
        if x0 <= px_x <= x1:
            return (y1 - y0) / (x1 - x0) if x1 != x0 else 0
    return 0

def draw_terrain(surf, terrain_px, h):
    if len(terrain_px) < 2:
        return
    poly = list(terrain_px) + [(terrain_px[-1][0], h), (terrain_px[0][0], h)]
    pygame.draw.polygon(surf, GROUND_FILL, poly)
    pygame.draw.lines(surf, TERRAIN_COL, False, terrain_px, 5)

def draw_character(surf, cx, cy, wheel_r=15):
    body_r = 28
    pygame.draw.circle(surf, CHAR_WHEEL, (int(cx - body_r + 4), int(cy + body_r - 2)), wheel_r)
    pygame.draw.circle(surf, CHAR_WHEEL, (int(cx + body_r - 4), int(cy + body_r - 2)), wheel_r)
    pygame.draw.circle(surf, CHAR_COL,   (int(cx), int(cy)), body_r)
    pygame.draw.circle(surf, BG,         (int(cx + 5), int(cy - 3)), 3)

# ---------------------------------------------------------------------------
# Zone overlay: draws wall / ramp / free zones on the terrain surface
# ---------------------------------------------------------------------------

def draw_zones(surf, terrain_px, h):
    """
    Left side:   ANGLE_MIN -> ANGLE_WALL_LOW  = hard wall (red)
                 ANGLE_WALL_LOW -> ANGLE_FREE_LOW = ramp (orange)
    Middle:      ANGLE_FREE_LOW -> ANGLE_FREE_HIGH = free (green tint)
    Right side:  ANGLE_FREE_HIGH -> ANGLE_WALL_HIGH = ramp (orange)
                 ANGLE_WALL_HIGH -> ANGLE_MAX = hard wall (red)
    """
    zones = [
        (ANGLE_MIN,       ANGLE_WALL_LOW,  WALL_COL),
        (ANGLE_WALL_LOW,  ANGLE_FREE_LOW,  RAMP_COL),
        (ANGLE_FREE_LOW,  ANGLE_FREE_HIGH, FREE_ZONE_COL),
        (ANGLE_FREE_HIGH, ANGLE_WALL_HIGH, RAMP_COL),
        (ANGLE_WALL_HIGH, ANGLE_MAX,       WALL_COL),
    ]

    overlay = pygame.Surface((WIDTH, h), pygame.SRCALPHA)

    for a_start, a_end, color in zones:
        x0 = angle_to_x(a_start, terrain_px)
        x1 = angle_to_x(a_end,   terrain_px)
        if x1 <= x0:
            continue

        # Build a polygon that follows the terrain top and fills down
        top_points = []
        for px_x in range(x0, x1 + 1, 2):
            py = terrain_y_at(px_x, terrain_px)
            top_points.append((px_x, py))

        poly = top_points + [(x1, h), (x0, h)]
        r, g, b = color
        pygame.draw.polygon(overlay, (r, g, b, ZONE_ALPHA), poly)

    surf.blit(overlay, (0, 0))

def draw_wall_lines(surf, terrain_px, font):
    """Vertical dashed lines + labels at each threshold."""
    thresholds = [
        (ANGLE_WALL_LOW,  "WALL",  WALL_COL),
        (ANGLE_FREE_LOW,  "FREE",  FREE_ZONE_COL),
        (ANGLE_FREE_HIGH, "FREE",  FREE_ZONE_COL),
        (ANGLE_WALL_HIGH, "WALL",  WALL_COL),
    ]
    for angle, label, color in thresholds:
        x = angle_to_x(angle, terrain_px)
        # Dashed line from top to terrain
        y_terrain = int(terrain_y_at(x, terrain_px))
        dash_len, gap = 8, 5
        y = 0
        while y < y_terrain:
            pygame.draw.line(surf, color, (x, y), (x, min(y + dash_len, y_terrain)), 1)
            y += dash_len + gap
        txt = font.render(label, True, color)
        surf.blit(txt, (x - txt.get_width() // 2, 4))

# ---------------------------------------------------------------------------
# Haptic force from slope (for HUD display)
# ---------------------------------------------------------------------------

def haptic_force(slope):
    force = -slope * 5.0
    return max(-1.0, min(1.0, force))

# ---------------------------------------------------------------------------
# Zone name for current position
# ---------------------------------------------------------------------------

def zone_name(angle):
    if angle is None:
        return "---", TEXT_COL
    if angle <= ANGLE_WALL_LOW:
        return "HARD WALL", WALL_COL
    if angle <= ANGLE_FREE_LOW:
        return "RAMP", RAMP_COL
    if angle <= ANGLE_FREE_HIGH:
        return "FREE ZONE", FREE_ZONE_COL
    if angle <= ANGLE_WALL_HIGH:
        return "RAMP", RAMP_COL
    return "HARD WALL", WALL_COL

# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    t = threading.Thread(target=serial_thread, args=(SERIAL_PORT, BAUD_RATE), daemon=True)
    t.start()

    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("Haptic Terrain  |  HW17/18")
    clock = pygame.time.Clock()

    font_big   = pygame.font.SysFont("Consolas", 22)
    font_small = pygame.font.SysFont("Consolas", 15)

    terrain_px = build_terrain_px(TERRAIN_POINTS, WIDTH, HEIGHT)

    use_mouse_fallback = False

    while True:
        clock.tick(FPS)

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit(); sys.exit()
            if event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                pygame.quit(); sys.exit()

        raw = latest_angle[0]
        if raw is not None:
            use_mouse_fallback = False
            clamped = max(ANGLE_MIN, min(ANGLE_MAX, raw))
            t_val   = (clamped - ANGLE_MIN) / (ANGLE_MAX - ANGLE_MIN)
        else:
            use_mouse_fallback = True
            mx, _  = pygame.mouse.get_pos()
            t_val  = mx / WIDTH

        x_left  = terrain_px[0][0]
        x_right = terrain_px[-1][0]
        char_x  = x_left + t_val * (x_right - x_left)
        char_y  = terrain_y_at(char_x, terrain_px) - 16

        slope = terrain_slope_at(char_x, terrain_px)
        force = haptic_force(slope)

        # Draw
        screen.fill(BG)
        draw_terrain(screen, terrain_px, HEIGHT)
        draw_zones(screen, terrain_px, HEIGHT)
        draw_wall_lines(screen, terrain_px, font_small)
        draw_character(screen, char_x, char_y)

        # HUD
        serial_status = "SERIAL OK" if serial_ok[0] else ("MOUSE MODE" if use_mouse_fallback else "CONNECTING...")
        serial_color  = ACCENT if serial_ok[0] else (220, 100, 80)

        z_name, z_col = zone_name(raw if raw is not None else None)

        screen.blit(font_big.render(serial_status, True, serial_color),        (20, 16))
        screen.blit(font_small.render(f"Angle:    {raw if raw is not None else '---':>5}", True, ANGLE_COL),   (20, 50))
        screen.blit(font_small.render(f"Position: {t_val * 100:.1f}%",         True, TEXT_COL),  (20, 70))
        screen.blit(font_small.render(f"Zone:     {z_name}",                   True, z_col),     (20, 90))

        # Zone legend
        legend_y = 16
        for label, color in [("WALL", WALL_COL), ("RAMP", RAMP_COL), ("FREE", FREE_ZONE_COL)]:
            pygame.draw.rect(screen, color, (WIDTH - 180, legend_y, 12, 12))
            screen.blit(font_small.render(label, True, color), (WIDTH - 162, legend_y))
            legend_y += 20

        pygame.display.flip()

if __name__ == "__main__":
    main()