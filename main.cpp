#include <SFML/Graphics.hpp>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

enum GameMode { MENU, SINGLE_PLAYER, TWO_PLAYERS, VS_AI, PLAYING, GAME_OVER };
GameMode currentMode = MENU;
bool vsMode = false;

struct Wave {
    int numBigAsteroids;
    int numMediumAsteroids;
    int numSmallAsteroids;
    float speedMultiplier;
    float scaleMultiplier;
};

Wave waves[] = {
    {3, 0, 0, 1.0f, 1.0f}, {4, 2, 0, 1.1f, 1.1f}, {5, 3, 2, 1.2f, 1.2f},
    {6, 4, 4, 1.3f, 1.3f}, {7, 5, 5, 1.4f, 1.4f}, {8, 6, 6, 1.5f, 1.5f},
};

int currentWave = 0;
int maxWaves = 6;

sf::Sprite playerSprite;
sf::Vector2f playerCurrentSpeed(0.f, 0.f);
float playerSpeed = 0.2f;
float playerFriction = 0.98f;
float playerRotationSpeed = 5.f;
float playerScale = 0.3f;
int lives = -1;
sf::Sprite livesSprites[3];
int playerRespawning = -1;
int playerInvulnerable = 0;
sf::RectangleShape playerHitbox;
sf::Sprite livesSprites2[3];

sf::Sprite player2Sprite;
sf::Vector2f player2CurrentSpeed(0.f, 0.f);
float player2Speed = 0.2f;
float player2Friction = 0.98f;
float player2RotationSpeed = 5.f;
float player2Scale = 0.3f;
int lives2 = -1;
int player2Respawning = -1;
int player2Invulnerable = 0;
sf::RectangleShape player2Hitbox;
bool isAI = false;
float aiThinkTimer = 0.f;
sf::Vector2f aiTargetPos;

sf::Texture textures[4];
sf::Vector2f screenSize(1400.f, 900.f);

std::vector<sf::Sprite> bullets;
std::vector<sf::Vector2f> bulletSpeeds;
std::vector<int> bulletTimers;
std::vector<sf::CircleShape> bulletHitboxes;
std::vector<int> bulletOwners;
float bulletFriction = 0.98f;
float bulletRadius = 4.f;
int bulletCD = 0;
int bullet2CD = 0;

std::vector<sf::Sprite> asteroids;
std::vector<sf::Vector2f> asteroidSpeeds;
std::vector<sf::CircleShape> asteroidHitboxes;
float asteroidScale = 2.f;
float asteroidRadius1 = 100.f;
float asteroidRadius2 = 50.f;
float asteroidRadius3 = 25.f;

sf::Font font;
sf::Text scoreText;
sf::Text scoreText2;
sf::RectangleShape restartButton;
sf::Text restartButtonText;
sf::RectangleShape startButton;
sf::Text startButtonText;
sf::RectangleShape player2Button;
sf::Text player2ButtonText;
sf::RectangleShape vsModeButton;
sf::Text vsModeButtonText;

sf::Text titleText;
sf::Text gameoverText;
sf::Text finalScoreText;

int score = 0;
int score2 = 0;

bool checkPlayerCollisions();
bool checkPlayer2Collisions();
void checkBulletPlayerCollisions();
void checkPlayerPlayerCollisions();

void initializePlayer() {
    textures[0].loadFromFile("images/sprite.png");
    textures[1].loadFromFile("images/sprite_move.png");
    textures[2].loadFromFile("images/bullet.png");
    textures[3].loadFromFile("images/asteroid.png");

    playerSprite.setTexture(textures[0]);
    playerSprite.setPosition(screenSize.x / 2.f, screenSize.y / 2.f);
    sf::Vector2u playerSize = textures[0].getSize();
    playerSprite.setOrigin(playerSize.x / 2.f, 0.f);
    playerSprite.setScale(playerScale, playerScale);
    playerSprite.setRotation(0.f);

    if (!checkPlayerCollisions()) {
        playerHitbox.setSize(
            sf::Vector2f(playerSize.x * playerScale / 2, playerSize.y * playerScale));
        playerHitbox.setOrigin(10.f, 0.f);
        playerHitbox.setFillColor(sf::Color::Transparent);
        playerHitbox.setOutlineColor(sf::Color::Green);
        playerHitbox.setOutlineThickness(1.f);
        playerRespawning = -1;
        playerInvulnerable = 120;
    }

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10.f, 10.f);

    for (int i = 0; i < lives; i++) {
        livesSprites[i].setTexture(textures[0]);
        sf::Vector2u playerSize = textures[0].getSize();
        livesSprites[i].setOrigin(playerSize.x / 2.f, 0.f);
        livesSprites[i].setScale(playerScale, playerScale);
        livesSprites[i].setPosition(30.f + i * 40.f, 40.f);
    }
}

void initializePlayer2() {
    player2Sprite.setTexture(textures[0]);
    player2Sprite.setPosition(screenSize.x / 2.f + 100.f, screenSize.y / 2.f + 100.f);
    sf::Vector2u playerSize = textures[0].getSize();
    player2Sprite.setOrigin(playerSize.x / 2.f, 0.f);
    player2Sprite.setScale(player2Scale, player2Scale);
    player2Sprite.setRotation(180.f);
    player2CurrentSpeed = sf::Vector2f(0.f, 0.f);

    if (!checkPlayer2Collisions()) {
        player2Hitbox.setSize(
            sf::Vector2f(playerSize.x * player2Scale / 2, playerSize.y * player2Scale));
        player2Hitbox.setOrigin(10.f, 0.f);
        player2Hitbox.setFillColor(sf::Color::Transparent);
        player2Hitbox.setOutlineColor(sf::Color::Cyan);
        player2Hitbox.setOutlineThickness(1.f);
        player2Respawning = -1;
        player2Invulnerable = 120;
    }

    scoreText2.setFont(font);
    scoreText2.setCharacterSize(24);
    scoreText2.setFillColor(sf::Color::Cyan);
    scoreText2.setPosition(screenSize.x - 100.f, 10.f);

    for (int i = 0; i < lives2; i++) {
        livesSprites2[i].setTexture(textures[0]);
        livesSprites2[i].setOrigin(playerSize.x / 2.f, 0.f);
        livesSprites2[i].setScale(player2Scale, player2Scale);
        livesSprites2[i].setPosition(screenSize.x - 30.f - i * 40.f, 40.f);
    }
}

void initializeTexts() {
    titleText.setFont(font);
    titleText.setString("ASTEROIDS");
    titleText.setCharacterSize(80);
    titleText.setFillColor(sf::Color::White);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
                        titleBounds.top + titleBounds.height / 2.f);
    titleText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f - 150.f);

    gameoverText.setFont(font);
    gameoverText.setString("Game Over");
    gameoverText.setCharacterSize(60);
    gameoverText.setFillColor(sf::Color::White);
    sf::FloatRect gameoverBounds = gameoverText.getLocalBounds();
    gameoverText.setOrigin(gameoverBounds.left + gameoverBounds.width / 2.f,
                           gameoverBounds.top + gameoverBounds.height / 2.f);
    gameoverText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f - 100.f);

    finalScoreText.setFont(font);
    finalScoreText.setCharacterSize(36);
    finalScoreText.setFillColor(sf::Color::White);
}

void initializeButtons() {
    restartButton.setSize(sf::Vector2f(200.f, 60.f));
    restartButton.setPosition(screenSize.x / 2.f - 100.f, screenSize.y / 2.f + 50.f);
    restartButton.setFillColor(sf::Color(70, 70, 70));
    restartButton.setOutlineColor(sf::Color::White);
    restartButton.setOutlineThickness(2.f);

    restartButtonText.setFont(font);
    restartButtonText.setString("Reiniciar");
    restartButtonText.setCharacterSize(28);
    restartButtonText.setFillColor(sf::Color::White);
    sf::FloatRect restartBounds = restartButtonText.getLocalBounds();
    restartButtonText.setOrigin(restartBounds.left + restartBounds.width / 2.f,
                                restartBounds.top + restartBounds.height / 2.f);
    restartButtonText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f + 80.f);

    startButton.setSize(sf::Vector2f(250.f, 70.f));
    startButton.setPosition(screenSize.x / 2.f - 125.f, screenSize.y / 2.f);
    startButton.setFillColor(sf::Color(70, 70, 70));
    startButton.setOutlineColor(sf::Color::White);
    startButton.setOutlineThickness(3.f);

    startButtonText.setFont(font);
    startButtonText.setString("Iniciar Partida");
    startButtonText.setCharacterSize(32);
    startButtonText.setFillColor(sf::Color::White);
    sf::FloatRect startBounds = startButtonText.getLocalBounds();
    startButtonText.setOrigin(startBounds.left + startBounds.width / 2.f,
                              startBounds.top + startBounds.height / 2.f);
    startButtonText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f + 35.f);

    player2Button.setSize(sf::Vector2f(250.f, 70.f));
    player2Button.setPosition(screenSize.x / 2.f - 125.f, screenSize.y / 2.f + 100.f);
    player2Button.setFillColor(sf::Color(70, 70, 70));
    player2Button.setOutlineColor(sf::Color::White);
    player2Button.setOutlineThickness(3.f);

    player2ButtonText.setFont(font);
    player2ButtonText.setString("Jugador 2");
    player2ButtonText.setCharacterSize(32);
    player2ButtonText.setFillColor(sf::Color::White);
    sf::FloatRect player2Bounds = player2ButtonText.getLocalBounds();
    player2ButtonText.setOrigin(player2Bounds.left + player2Bounds.width / 2.f,
                                player2Bounds.top + player2Bounds.height / 2.f);
    player2ButtonText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f + 135.f);

    vsModeButton.setSize(sf::Vector2f(250.f, 70.f));
    vsModeButton.setPosition(screenSize.x / 2.f - 125.f, screenSize.y / 2.f + 200.f);
    vsModeButton.setFillColor(sf::Color(70, 70, 70));
    vsModeButton.setOutlineColor(sf::Color::White);
    vsModeButton.setOutlineThickness(3.f);

    vsModeButtonText.setFont(font);
    vsModeButtonText.setString("Modo VS");
    vsModeButtonText.setCharacterSize(32);
    vsModeButtonText.setFillColor(sf::Color::White);
    sf::FloatRect vsModeBounds = vsModeButtonText.getLocalBounds();
    vsModeButtonText.setOrigin(vsModeBounds.left + vsModeBounds.width / 2.f,
                               vsModeBounds.top + vsModeBounds.height / 2.f);
    vsModeButtonText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f + 235.f);
}

void teleportInSides(sf::Sprite& sprite) {
    if (sprite.getPosition().x < -100)
        sprite.setPosition(screenSize.x + 100, sprite.getPosition().y);
    if (sprite.getPosition().x > screenSize.x + 100)
        sprite.setPosition(-100, sprite.getPosition().y);
    if (sprite.getPosition().y < -100)
        sprite.setPosition(sprite.getPosition().x, screenSize.y + 100);
    if (sprite.getPosition().y > screenSize.y + 100)
        sprite.setPosition(sprite.getPosition().x, -100);
}

bool checkCollision(sf::CircleShape& circle1, sf::CircleShape& circle2) {
    sf::Vector2f pos1 = circle1.getPosition();
    sf::Vector2f pos2 = circle2.getPosition();

    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    float distance = std::sqrt(dx * dx + dy * dy);

    float radiusSum = circle1.getRadius() + circle2.getRadius();
    return distance < radiusSum;
}

bool checkCollision(sf::RectangleShape& rect, sf::CircleShape& circle) {
    if (rect.getSize().x == 0.f || rect.getSize().y == 0.f) return false;
    sf::Vector2f rectPos = rect.getPosition();
    sf::Vector2f rectSize = rect.getSize();
    sf::Vector2f rectOrigin = rect.getOrigin();
    float rectLeft = rectPos.x - rectOrigin.x;
    float rectRight = rectLeft + rectSize.x;
    float rectTop = rectPos.y - rectOrigin.y;
    float rectBottom = rectTop + rectSize.y;

    sf::Vector2f circlePos = circle.getPosition();
    float radius = circle.getRadius();

    float closestX = std::max(rectLeft, std::min(circlePos.x, rectRight));
    float closestY = std::max(rectTop, std::min(circlePos.y, rectBottom));

    float dx = circlePos.x - closestX;
    float dy = circlePos.y - closestY;
    float distanceSquared = dx * dx + dy * dy;
    return distanceSquared < (radius * radius);
}

bool checkCollision(sf::RectangleShape& rect1, sf::RectangleShape& rect2) {
    if (rect1.getSize().x == 0.f || rect1.getSize().y == 0.f) return false;
    if (rect2.getSize().x == 0.f || rect2.getSize().y == 0.f) return false;

    sf::FloatRect bounds1 = rect1.getGlobalBounds();
    sf::FloatRect bounds2 = rect2.getGlobalBounds();

    return bounds1.intersects(bounds2);
}

void updatePlayer() {
    playerSprite.setTexture(textures[0]);
    playerCurrentSpeed *= playerFriction;
    playerSprite.move(playerCurrentSpeed);
    teleportInSides(playerSprite);
    playerHitbox.setPosition(playerSprite.getPosition());
    playerHitbox.setRotation(playerSprite.getRotation());
}

void rotatePlayer(float direction) { playerSprite.rotate(direction * playerRotationSpeed); }

void movePlayer(float direction) {
    float angle = (playerSprite.getRotation() - 90) * M_PI / 180.f;
    playerCurrentSpeed.x += direction * playerSpeed * cos(angle);
    playerCurrentSpeed.y += direction * playerSpeed * sin(angle);
    playerSprite.setTexture(textures[1]);
}

void updatePlayer2() {
    player2Sprite.setTexture(textures[0]);
    player2CurrentSpeed *= player2Friction;
    player2Sprite.move(player2CurrentSpeed);
    teleportInSides(player2Sprite);
    player2Hitbox.setPosition(player2Sprite.getPosition());
    player2Hitbox.setRotation(player2Sprite.getRotation());
}

void rotatePlayer2(float direction) { player2Sprite.rotate(direction * player2RotationSpeed); }

void movePlayer2(float direction) {
    float angle = (player2Sprite.getRotation() - 90) * M_PI / 180.f;
    player2CurrentSpeed.x += direction * player2Speed * cos(angle);
    player2CurrentSpeed.y += direction * player2Speed * sin(angle);
    player2Sprite.setTexture(textures[1]);
}

void shoot() {
    sf::Sprite newBullet;
    newBullet.setTexture(textures[2]);
    newBullet.setOrigin(64.f, 64.f);
    newBullet.setPosition(playerSprite.getPosition());
    newBullet.setScale(0.1f, 0.1f);

    float angle = (playerSprite.getRotation() - 90) * M_PI / 180.f;
    sf::Vector2f bulletSpeed(cos(angle) * 10.f, sin(angle) * 10.f);

    sf::CircleShape bulletHitbox;
    bulletHitbox.setRadius(bulletRadius);
    bulletHitbox.setOrigin(bulletRadius, bulletRadius);
    bulletHitbox.setPosition(playerSprite.getPosition());
    bulletHitbox.setFillColor(sf::Color::Yellow);
    bulletHitbox.setOutlineColor(sf::Color::Green);
    bulletHitbox.setOutlineThickness(1.f);

    bullets.push_back(newBullet);
    bulletSpeeds.push_back(bulletSpeed);
    bulletHitboxes.push_back(bulletHitbox);
    bulletTimers.push_back(60);
    bulletOwners.push_back(1);
}

void shoot2() {
    sf::Sprite newBullet;
    newBullet.setTexture(textures[2]);
    newBullet.setOrigin(64.f, 64.f);
    newBullet.setPosition(player2Sprite.getPosition());
    newBullet.setScale(0.1f, 0.1f);

    float angle = (player2Sprite.getRotation() - 90) * M_PI / 180.f;
    sf::Vector2f bulletSpeed(cos(angle) * 10.f, sin(angle) * 10.f);

    sf::CircleShape bulletHitbox;
    bulletHitbox.setRadius(bulletRadius);
    bulletHitbox.setOrigin(bulletRadius, bulletRadius);
    bulletHitbox.setPosition(player2Sprite.getPosition());
    bulletHitbox.setFillColor(sf::Color::Cyan);
    bulletHitbox.setOutlineColor(sf::Color::Blue);
    bulletHitbox.setOutlineThickness(1.f);

    bullets.push_back(newBullet);
    bulletSpeeds.push_back(bulletSpeed);
    bulletHitboxes.push_back(bulletHitbox);
    bulletTimers.push_back(60);
    bulletOwners.push_back(2);
}

void updateBullets() {
    for (size_t i = 0; i < bullets.size(); i++) {
        bullets[i].move(bulletSpeeds[i]);
        bulletTimers[i]--;
        teleportInSides(bullets[i]);
        bulletHitboxes[i].setPosition(bullets[i].getPosition());
    }
}

void removeBullet(size_t index) {
    bullets.erase(bullets.begin() + index);
    bulletSpeeds.erase(bulletSpeeds.begin() + index);
    bulletHitboxes.erase(bulletHitboxes.begin() + index);
    bulletTimers.erase(bulletTimers.begin() + index);
}

void removeBullets() {
    for (size_t i = 0; i < bullets.size();) {
        if (bulletTimers[i] <= 0) {
            removeBullet(i);
        } else {
            i++;
        }
    }
}

void spawnAsteroid(float radius) {
    Wave wave = waves[currentWave];
    float speedMultiplier = wave.speedMultiplier;
    float scaleMultiplier = wave.scaleMultiplier;

    sf::Sprite newAsteroid;
    newAsteroid.setTexture(textures[3]);
    newAsteroid.setOrigin(64.f, 64.f);
    do {
        newAsteroid.setPosition(rand() % (int)screenSize.x, rand() % (int)screenSize.y);
    } while (newAsteroid.getPosition().x > 100 &&
             newAsteroid.getPosition().x < screenSize.x - 100 &&
             newAsteroid.getPosition().y > 100 && newAsteroid.getPosition().y < screenSize.y - 100);
    newAsteroid.setScale(asteroidScale * (radius / 100) * scaleMultiplier,
                         asteroidScale * (radius / 100) * scaleMultiplier);

    float angle = (rand() % 360) * M_PI / 180.f;
    sf::Vector2f bulletSpeed(cos(angle) * 200.f / radius * speedMultiplier,
                             sin(angle) * 200.f / radius * speedMultiplier);

    sf::CircleShape asteroidHitbox;
    asteroidHitbox.setRadius(radius * scaleMultiplier);
    asteroidHitbox.setOrigin(radius * scaleMultiplier, radius * scaleMultiplier);
    asteroidHitbox.setPosition(newAsteroid.getPosition());
    asteroidHitbox.setFillColor(sf::Color::Yellow);
    asteroidHitbox.setOutlineColor(sf::Color::Green);
    asteroidHitbox.setOutlineThickness(1.f);

    asteroids.push_back(newAsteroid);
    asteroidSpeeds.push_back(bulletSpeed);
    asteroidHitboxes.push_back(asteroidHitbox);
}

void spawnAsteroid(float radius, sf::Sprite biggerAsteroid) {
    sf::Sprite newAsteroid;
    newAsteroid.setTexture(textures[3]);
    newAsteroid.setOrigin(64.f, 64.f);
    newAsteroid.setPosition(biggerAsteroid.getPosition());
    newAsteroid.setScale(biggerAsteroid.getScale().x / 2.f, biggerAsteroid.getScale().y / 2.f);

    float angle = (rand() % 360) * M_PI / 180.f;
    sf::Vector2f bulletSpeed(cos(angle) * 200.f / (radius * 1.5),
                             sin(angle) * 200.f / (radius * 1.5));

    sf::CircleShape asteroidHitbox;
    asteroidHitbox.setRadius(radius);
    asteroidHitbox.setOrigin(radius, radius);
    asteroidHitbox.setPosition(newAsteroid.getPosition());
    asteroidHitbox.setFillColor(sf::Color::Yellow);
    asteroidHitbox.setOutlineColor(sf::Color::Green);
    asteroidHitbox.setOutlineThickness(1.f);

    asteroids.push_back(newAsteroid);
    asteroidSpeeds.push_back(bulletSpeed);
    asteroidHitboxes.push_back(asteroidHitbox);
}

void updateAsteroids() {
    for (size_t i = 0; i < asteroids.size(); i++) {
        asteroids[i].move(asteroidSpeeds[i]);
        teleportInSides(asteroids[i]);
        asteroidHitboxes[i].setPosition(asteroids[i].getPosition());
    }
}

void removeAsteroid(size_t index, int owner) {
    Wave wave = waves[currentWave - 1];
    float scaleMultiplier = wave.scaleMultiplier;
    if (asteroids[index].getScale().x == asteroidScale * scaleMultiplier) {
        spawnAsteroid(asteroidRadius2, asteroids[index]);
        spawnAsteroid(asteroidRadius2, asteroids[index]);
        if (owner == 1) {
            score += 20;
        } else {
            score2 += 20;
        }

    } else if (asteroids[index].getScale().x == asteroidScale * 0.5 * scaleMultiplier) {
        spawnAsteroid(asteroidRadius3, asteroids[index]);
        spawnAsteroid(asteroidRadius3, asteroids[index]);
        if (owner == 1) {
            score += 50;
        } else {
            score2 += 50;
        }
    } else {
        if (owner == 1) {
            score += 100;
        } else {
            score2 += 100;
        }
    }

    asteroids.erase(asteroids.begin() + index);
    asteroidSpeeds.erase(asteroidSpeeds.begin() + index);
    asteroidHitboxes.erase(asteroidHitboxes.begin() + index);
}
void checkAsteroidCollisions() {
    for (int i = asteroids.size() - 1; i >= 0; i--) {
        for (int j = bullets.size() - 1; j >= 0; j--) {
            if (checkCollision(asteroidHitboxes[i], bulletHitboxes[j])) {
                removeBullet(j);
                removeAsteroid(i, bulletOwners[j]);
                break;
            }
        }
    }
}

bool checkPlayerCollisions() {
    if (playerInvulnerable > 0) return false;

    for (size_t i = 0; i < asteroids.size(); i++) {
        if (checkCollision(playerHitbox, asteroidHitboxes[i])) {
            removeAsteroid(i, 1);
            playerHitbox.setSize(sf::Vector2f(0.f, 0.f));
            playerSprite.setScale(sf::Vector2f(0.f, 0.f));
            std::cout << "Colisión detectada con el asteroide" << std::endl;
            playerRespawning = 120;
            playerInvulnerable = 0;
            return true;
        }
    }
    return false;
}

bool checkPlayer2Collisions() {
    if (player2Invulnerable > 0) return false;

    for (size_t i = 0; i < asteroids.size(); i++) {
        if (checkCollision(player2Hitbox, asteroidHitboxes[i])) {
            removeAsteroid(i, 2);
            player2Hitbox.setSize(sf::Vector2f(0.f, 0.f));
            player2Sprite.setScale(sf::Vector2f(0.f, 0.f));
            std::cout << "Colisión detectada con el asteroide" << std::endl;
            player2Respawning = 120;
            player2Invulnerable = 0;
            return true;
        }
    }
    return false;
}

void checkBulletPlayerCollisions() {
    for (int i = bullets.size() - 1; i >= 0; i--) {
        if (bulletOwners[i] == 2 && lives > 0 && playerRespawning == -1 &&
            playerInvulnerable == 0) {
            if (checkCollision(playerHitbox, bulletHitboxes[i])) {
                removeBullet(i);
                playerHitbox.setSize(sf::Vector2f(0.f, 0.f));
                playerSprite.setScale(sf::Vector2f(0.f, 0.f));
                std::cout << "P1 golpeado por P2!" << std::endl;
                playerRespawning = 120;
                playerInvulnerable = 0;
                lives--;
                score2 += 500;
            }
        } else if (bulletOwners[i] == 1 && lives2 > 0 && player2Respawning == -1 &&
                   player2Invulnerable == 0) {
            if (checkCollision(player2Hitbox, bulletHitboxes[i])) {
                removeBullet(i);
                player2Hitbox.setSize(sf::Vector2f(0.f, 0.f));
                player2Sprite.setScale(sf::Vector2f(0.f, 0.f));
                std::cout << "P2 golpeado por P1!" << std::endl;
                player2Respawning = 120;
                player2Invulnerable = 0;
                lives2--;
                score += 500;
            }
        }
    }
}

void checkPlayerPlayerCollisions() {
    if (lives > 0 && lives2 > 0 && playerRespawning == -1 && player2Respawning == -1 &&
        playerInvulnerable == 0 && player2Invulnerable == 0) {
        if (checkCollision(playerHitbox, player2Hitbox)) {
            std::cout << "Colisión entre jugadores!" << std::endl;

            playerHitbox.setSize(sf::Vector2f(0.f, 0.f));
            playerSprite.setScale(sf::Vector2f(0.f, 0.f));
            playerRespawning = 120;
            playerInvulnerable = 0;
            lives--;

            player2Hitbox.setSize(sf::Vector2f(0.f, 0.f));
            player2Sprite.setScale(sf::Vector2f(0.f, 0.f));
            player2Respawning = 120;
            player2Invulnerable = 0;
            lives2--;
        }
    }
}

void render(sf::RenderWindow& window) {
    if (lives == -1) {
        window.clear(sf::Color::Black);

        window.draw(titleText);

        window.draw(startButton);
        window.draw(startButtonText);
        window.draw(player2Button);
        window.draw(player2ButtonText);
        window.draw(vsModeButton);
        window.draw(vsModeButtonText);
    } else if (lives > 0 || lives2 > 0) {
        window.clear(sf::Color::Black);

        if (lives > 0) {
            if (playerInvulnerable == 0 || (playerInvulnerable / 10) % 2 == 0) {
                window.draw(playerSprite);
                // window.draw(playerHitbox);
            }
        }

        if (lives2 > 0 && currentMode == TWO_PLAYERS) {
            if (player2Invulnerable == 0 || (player2Invulnerable / 10) % 2 == 0) {
                window.draw(player2Sprite);
                // window.draw(player2Hitbox);
            }
        }

        for (size_t i = 0; i < bullets.size(); i++) {
            window.draw(bullets[i]);
        }

        for (size_t i = 0; i < asteroids.size(); i++) {
            window.draw(asteroids[i]);
            window.draw(asteroidHitboxes[i]);
        }

        window.draw(scoreText);

        if (currentMode == TWO_PLAYERS) {
            window.draw(scoreText2);
        }

        for (int i = 0; i < lives; i++) {
            window.draw(livesSprites[i]);
        }

        if (currentMode == TWO_PLAYERS) {
            for (int i = 0; i < lives2; i++) {
                window.draw(livesSprites2[i]);
            }
        }

    } else if (currentMode == GAME_OVER) {
        window.clear(sf::Color::Black);
        if (lives2 > 0) {
            if (score > score2) {
                finalScoreText.setString("Ganador: P1\n Puntaje: " + std::to_string(score));
            } else {
                finalScoreText.setString("Ganador: P2\n Puntaje: " + std::to_string(score2));
            }
        } else {
            finalScoreText.setString("Puntaje Final: " + std::to_string(score));
        }
        sf::FloatRect scoreBounds = finalScoreText.getLocalBounds();
        finalScoreText.setOrigin(scoreBounds.left + scoreBounds.width / 2.f,
                                 scoreBounds.top + scoreBounds.height / 2.f);
        finalScoreText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f - 20.f);

        window.draw(gameoverText);
        window.draw(finalScoreText);

        window.draw(restartButton);
        window.draw(restartButtonText);
    }
    window.display();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(screenSize.x, screenSize.y), "Asteroids");
    window.setFramerateLimit(60);

    if (!font.loadFromFile("fonts/font.ttf")) {
        font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }
    initializeButtons();
    initializeTexts();

    while (window.isOpen()) {
        if (lives == -1) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();

                if (event.type == sf::Event::MouseButtonReleased &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (startButton.getGlobalBounds().contains(
                            static_cast<sf::Vector2f>(mousePos))) {
                        lives = 3;
                        currentMode = SINGLE_PLAYER;
                        initializePlayer();
                    } else if (player2Button.getGlobalBounds().contains(
                                   static_cast<sf::Vector2f>(mousePos))) {
                        lives = 3;
                        lives2 = 3;
                        currentMode = TWO_PLAYERS;
                        vsMode = false;
                        initializePlayer();
                        initializePlayer2();
                    } else if (vsModeButton.getGlobalBounds().contains(
                                   static_cast<sf::Vector2f>(mousePos))) {
                        lives = 3;
                        lives2 = 3;
                        currentMode = TWO_PLAYERS;
                        vsMode = true;
                        initializePlayer();
                        initializePlayer2();
                    }
                }
            }

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (startButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                startButton.setFillColor(sf::Color(150, 150, 150));
            } else {
                startButton.setFillColor(sf::Color(100, 100, 100));
            }

            if (player2Button.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                player2Button.setFillColor(sf::Color(150, 150, 150));
            } else {
                player2Button.setFillColor(sf::Color(100, 100, 100));
            }

            if (vsModeButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                vsModeButton.setFillColor(sf::Color(150, 150, 150));
            } else {
                vsModeButton.setFillColor(sf::Color(100, 100, 100));
            }

        } else if (lives > 0 || lives2 > 0) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();
            }

            if (lives > 0) {
                updatePlayer();

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                    rotatePlayer(-1.f);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                    rotatePlayer(+1.f);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                    movePlayer(1.f);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                    movePlayer(-1.f);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && bulletCD == 0 &&
                    playerRespawning == -1) {
                    bulletCD = 15;
                    std::cout << "Disparo P1" << std::endl;
                    shoot();
                }

                if (bulletCD > 0) bulletCD--;

                if (playerRespawning > 0) playerRespawning--;
                if (playerRespawning == 0) initializePlayer();

                if (playerInvulnerable > 0) playerInvulnerable--;

                scoreText.setString("P1: " + std::to_string(score));
                if (checkPlayerCollisions()) lives--;
            }

            if (lives2 > 0 && currentMode == TWO_PLAYERS) {
                updatePlayer2();

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                    rotatePlayer2(-1.f);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                    rotatePlayer2(+1.f);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                    movePlayer2(1.f);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                    movePlayer2(-1.f);
                }

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) && bullet2CD == 0 &&
                    player2Respawning == -1) {
                    bullet2CD = 15;
                    std::cout << "Disparo P2" << std::endl;
                    shoot2();
                }

                if (bullet2CD > 0) bullet2CD--;

                if (player2Respawning > 0) player2Respawning--;
                if (player2Respawning == 0) initializePlayer2();

                if (player2Invulnerable > 0) player2Invulnerable--;

                scoreText2.setString("P2: " + std::to_string(score2));
                if (checkPlayer2Collisions()) lives2--;
            }

            if (asteroids.empty() && currentWave < maxWaves) {
                Wave wave = waves[currentWave];
                for (int i = 0; i < wave.numBigAsteroids; i++) spawnAsteroid(asteroidRadius1);
                for (int i = 0; i < wave.numMediumAsteroids; i++) spawnAsteroid(asteroidRadius2);
                for (int i = 0; i < wave.numSmallAsteroids; i++) spawnAsteroid(asteroidRadius3);
                currentWave++;
            } else if (asteroids.empty() && currentWave >= maxWaves) {
                currentMode = GAME_OVER;
            }

            checkAsteroidCollisions();

            if (vsMode) {
                checkBulletPlayerCollisions();
                checkPlayerPlayerCollisions();
            }

            updateBullets();
            updateAsteroids();
            removeBullets();

        } else {
            currentMode = GAME_OVER;
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();

                if (event.type == sf::Event::MouseButtonReleased &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (restartButton.getGlobalBounds().contains(
                            static_cast<sf::Vector2f>(mousePos))) {
                        lives = -1;
                        lives2 = -1;
                        score = 0;
                        score2 = 0;
                        asteroids.clear();
                        asteroidSpeeds.clear();
                        asteroidHitboxes.clear();
                        bullets.clear();
                        bulletSpeeds.clear();
                        bulletHitboxes.clear();
                        bulletTimers.clear();
                        bulletOwners.clear();
                        initializePlayer();
                        if (currentMode == TWO_PLAYERS) {
                            initializePlayer2();
                        }
                    }
                }
            }

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (restartButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                restartButton.setFillColor(sf::Color(100, 100, 100));
            } else {
                restartButton.setFillColor(sf::Color(70, 70, 70));
            }
        }
        render(window);
    }

    return 0;
}
