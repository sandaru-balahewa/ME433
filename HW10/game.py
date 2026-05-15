import random
import pgzrun

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
# PLAYER
# =========================
player = Rect((370, 540), (60, 40))
player_speed = 6

# =========================
# BULLETS & ENEMIES
# =========================
bullets = []
enemies = []

enemy_timer = 0

# =========================
# RESTART BUTTON
# =========================
restart_button = Rect((300, 350), (200, 60))


# =========================
# DRAW FUNCTION
# =========================
def draw():

    screen.clear()
    screen.fill((0, 0, 20))

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
        screen.draw.filled_rect(restart_button, "green")

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
    screen.draw.filled_rect(player, "cyan")

    # -------------------------
    # DRAW BULLETS
    # -------------------------
    for bullet in bullets:
        screen.draw.filled_rect(bullet, "yellow")

    # -------------------------
    # DRAW ENEMIES
    # -------------------------
    for enemy in enemies:
        screen.draw.filled_rect(enemy, "red")

    # -------------------------
    # DRAW SCORE
    # -------------------------
    screen.draw.text(
        f"Score: {score}",
        (10, 10),
        fontsize=35,
        color="white"
    )


# =========================
# UPDATE FUNCTION
# =========================
def update():

    global enemy_timer
    global game_over
    global score

    if game_over:
        return

    # -------------------------
    # PLAYER MOVEMENT
    # -------------------------
    if keyboard.left:
        player.x -= player_speed

    if keyboard.right:
        player.x += player_speed

    # Keep player on screen
    if player.left < 0:
        player.left = 0

    if player.right > WIDTH:
        player.right = WIDTH

    # -------------------------
    # MOVE BULLETS
    # -------------------------
    for bullet in bullets[:]:

        bullet.y -= 8

        # Remove bullets off screen
        if bullet.bottom < 0:
            bullets.remove(bullet)

    # -------------------------
    # SPAWN ENEMIES
    # -------------------------
    enemy_timer += 1

    if enemy_timer > 40:

        enemy_timer = 0

        enemy_x = random.randint(0, WIDTH - 40)

        enemy = Rect((enemy_x, 0), (40, 40))

        enemies.append(enemy)

    # -------------------------
    # MOVE ENEMIES
    # -------------------------
    for enemy in enemies[:]:

        enemy.y += random.randint(2, 5)

        # Enemy hits player
        if enemy.colliderect(player):
            game_over = True

        # Remove enemies off screen
        if enemy.top > HEIGHT:
            enemies.remove(enemy)

    # -------------------------
    # BULLET-ENEMY COLLISIONS
    # -------------------------
    for bullet in bullets[:]:

        for enemy in enemies[:]:

            if bullet.colliderect(enemy):

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

        bullet = Rect(
            (player.centerx - 3, player.top - 10),
            (6, 15)
        )

        bullets.append(bullet)


# =========================
# MOUSE INPUT
# =========================
def on_mouse_down(pos):

    if game_over and restart_button.collidepoint(pos):
        restart_game()


# =========================
# RESTART FUNCTION
# =========================
def restart_game():

    global game_over
    global score
    global enemies
    global bullets
    global enemy_timer

    game_over = False
    score = 0

    enemies.clear()
    bullets.clear()

    enemy_timer = 0

    # Reset player position
    player.x = 370
    player.y = 540


# =========================
# START GAME
# =========================
pgzrun.go()