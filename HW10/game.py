import random
import math
import pgzrun
import pygame

# =========================
# SCREEN SETTINGS
# =========================
WIDTH = 800
HEIGHT = 600

# =========================
# GAME VARIABLES
# =========================
game_over = False
score = 0

# =========================
# PLAYER SHIP
# =========================
player_x = WIDTH // 2
player_y = HEIGHT - 70
player_speed = 6

# =========================
# BULLETS & ENEMIES
# =========================
bullets = []
enemies = []

enemy_timer = 0

# =========================
# STAR BACKGROUND
# =========================
stars = []

for i in range(100):

    stars.append([
        random.randint(0, WIDTH),
        random.randint(0, HEIGHT),
        random.randint(1, 3)
    ])

# =========================
# RESTART BUTTON
# =========================
restart_button = Rect((300, 350), (200, 60))


# =========================
# DRAW PLAYER SHIP
# =========================
def draw_ship(x, y):

    # Main body
    pygame.draw.polygon(
        screen.surface,
        (0, 220, 255),
        [
            (x, y - 30),
            (x - 25, y + 20),
            (x + 25, y + 20)
        ]
    )

    # Cockpit
    screen.draw.filled_circle(
        (x, y - 5),
        8,
        (255, 255, 255)
    )

    # Left wing
    pygame.draw.polygon(
        screen.surface,
        (0, 150, 255),
        [
            (x - 25, y + 15),
            (x - 40, y + 30),
            (x - 15, y + 20)
        ]
    )

    # Right wing
    pygame.draw.polygon(
        screen.surface,
        (0, 150, 255),
        [
            (x + 25, y + 15),
            (x + 40, y + 30),
            (x + 15, y + 20)
        ]
    )

    # Engine flame
    flame_size = random.randint(8, 15)

    pygame.draw.polygon(
        screen.surface,
        (255, 140, 0),
        [
            (x - 10, y + 20),
            (x, y + 20 + flame_size),
            (x + 10, y + 20)
        ]
    )


# =========================
# DRAW ASTEROID
# =========================
def draw_asteroid(enemy):

    x = enemy["x"]
    y = enemy["y"]
    size = enemy["size"]

    points = []

    for i in range(8):

        angle = (math.pi * 2 / 8) * i

        radius = size + random.randint(-5, 5)

        px = x + math.cos(angle) * radius
        py = y + math.sin(angle) * radius

        points.append((px, py))

    pygame.draw.polygon(
        screen.surface,
        (120, 120, 120),
        points
    )

    # Craters
    screen.draw.filled_circle(
        (x - 5, y - 3),
        3,
        (90, 90, 90)
    )

    screen.draw.filled_circle(
        (x + 7, y + 5),
        2,
        (90, 90, 90)
    )


# =========================
# DRAW FUNCTION
# =========================
def draw():

    screen.clear()
    screen.fill((5, 5, 20))

    # -------------------------
    # DRAW STARS
    # -------------------------
    for star in stars:

        screen.draw.filled_circle(
            (star[0], star[1]),
            star[2],
            "white"
        )

    # -------------------------
    # GAME OVER SCREEN
    # -------------------------
    if game_over:

        screen.draw.text(
            "GAME OVER",
            center=(WIDTH // 2, HEIGHT // 2 - 80),
            fontsize=70,
            color="red"
        )

        screen.draw.text(
            f"Final Score: {score}",
            center=(WIDTH // 2, HEIGHT // 2),
            fontsize=40,
            color="white"
        )

        # Restart button
        screen.draw.filled_rect(
            restart_button,
            (0, 200, 100)
        )

        screen.draw.text(
            "RESTART",
            center=restart_button.center,
            fontsize=40,
            color="black"
        )

        return

    # -------------------------
    # DRAW PLAYER
    # -------------------------
    draw_ship(player_x, player_y)

    # -------------------------
    # DRAW BULLETS
    # -------------------------
    for bullet in bullets:

        bullet_rect = Rect(
            (bullet["x"], bullet["y"]),
            (4, 15)
        )

        screen.draw.filled_rect(
            bullet_rect,
            "yellow"
        )

    # -------------------------
    # DRAW ENEMIES
    # -------------------------
    for enemy in enemies:
        draw_asteroid(enemy)

    # -------------------------
    # DRAW SCORE
    # -------------------------
    screen.draw.text(
        f"Score: {score}",
        (15, 15),
        fontsize=35,
        color="white"
    )


# =========================
# UPDATE FUNCTION
# =========================
def update():

    global player_x
    global enemy_timer
    global game_over
    global score

    if game_over:
        return

    # -------------------------
    # MOVE STARS
    # -------------------------
    for star in stars:

        star[1] += star[2]

        if star[1] > HEIGHT:

            star[0] = random.randint(0, WIDTH)
            star[1] = 0

    # -------------------------
    # PLAYER MOVEMENT
    # -------------------------
    if keyboard.left:
        player_x -= player_speed

    if keyboard.right:
        player_x += player_speed

    # Keep player on screen
    if player_x < 40:
        player_x = 40

    if player_x > WIDTH - 40:
        player_x = WIDTH - 40

    # -------------------------
    # MOVE BULLETS
    # -------------------------
    for bullet in bullets[:]:

        bullet["y"] -= 10

        if bullet["y"] < 0:
            bullets.remove(bullet)

    # -------------------------
    # SPAWN ENEMIES
    # -------------------------
    enemy_timer += 1

    if enemy_timer > 40:

        enemy_timer = 0

        enemies.append({
            "x": random.randint(50, WIDTH - 50),
            "y": -40,
            "size": random.randint(20, 35),
            "speed": random.randint(2, 5)
        })

    # -------------------------
    # MOVE ENEMIES
    # -------------------------
    for enemy in enemies[:]:

        enemy["y"] += enemy["speed"]

        # Collision with player
        distance = math.sqrt(
            (enemy["x"] - player_x) ** 2 +
            (enemy["y"] - player_y) ** 2
        )

        if distance < enemy["size"] + 20:
            game_over = True

        # Remove off screen
        if enemy["y"] > HEIGHT + 50:
            enemies.remove(enemy)

    # -------------------------
    # BULLET COLLISIONS
    # -------------------------
    for bullet in bullets[:]:

        for enemy in enemies[:]:

            distance = math.sqrt(
                (enemy["x"] - bullet["x"]) ** 2 +
                (enemy["y"] - bullet["y"]) ** 2
            )

            if distance < enemy["size"]:

                if bullet in bullets:
                    bullets.remove(bullet)

                if enemy in enemies:
                    enemies.remove(enemy)

                score += 1

                break


# =========================
# KEYBOARD INPUT
# =========================
def on_key_down(key):

    if key == keys.SPACE and not game_over:

        bullets.append({
            "x": player_x - 2,
            "y": player_y - 30
        })


# =========================
# MOUSE INPUT
# =========================
def on_mouse_down(pos):

    if game_over and restart_button.collidepoint(pos):
        restart_game()


# =========================
# RESTART GAME
# =========================
def restart_game():

    global game_over
    global score
    global enemy_timer
    global player_x

    game_over = False
    score = 0

    bullets.clear()
    enemies.clear()

    enemy_timer = 0

    player_x = WIDTH // 2


# =========================
# START GAME
# =========================
pgzrun.go()