#include <SFML/Graphics.hpp>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

enum GameMode { MENU, SINGLE_PLAYER, TWO_PLAYERS, VS_AI, DIFFICULTY_SELECT, PLAYING, GAME_OVER };
GameMode currentMode = MENU;
bool vsMode = false;

enum AIDifficulty { AI_EASY, AI_MEDIUM, AI_HARD };
AIDifficulty aiDifficulty = AI_EASY;

struct Wave {
    int numBigAsteroids;
    int numMediumAsteroids;
    int numSmallAsteroids;
    float speedMultiplier;
    float scaleMultiplier;
    int ufoCD;
    float ufoSpeed;
};

Wave waves[] = {
    {3, 0, 0, 1.0f, 1.0f, 600, 5.f},  {4, 2, 0, 1.1f, 1.1f, 550, 5.2f},
    {5, 3, 2, 1.2f, 1.2f, 500, 5.4f}, {6, 4, 4, 1.3f, 1.3f, 450, 5.6f},
    {7, 5, 5, 1.4f, 1.4f, 400, 5.8f}, {8, 6, 6, 1.5f, 1.5f, 350, 6.f},
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
float aiThinkInterval = 60.f;
float aiVisionRange = 600.f;
int aiTargetIndex = -1;
sf::CircleShape aiVisionCircle;

sf::Texture textures[5];
sf::Vector2f screenSize(1400.f, 900.f);

int ufoCD = 600;
std::vector<sf::Sprite> ufos;
std::vector<bool> ufoDirections;
std::vector<int> ufoShootCDs;
float ufoScale = 0.3f;
std::vector<sf::RectangleShape> ufoHitboxes;

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
sf::RectangleShape vsAIButton;
sf::Text vsAIButtonText;

sf::RectangleShape easyButton;
sf::Text easyButtonText;
sf::RectangleShape mediumButton;
sf::Text mediumButtonText;
sf::RectangleShape hardButton;
sf::Text hardButtonText;
sf::Text difficultyTitleText;

sf::Text titleText;
sf::Text gameoverText;
sf::Text finalScoreText;

int score = 0;
int score2 = 0;

// Se declaran las funciones para poder llamarlas desde otras funciones antes de su definición
bool checkPlayerCollisions();
bool checkPlayer2Collisions();
void checkBulletPlayerCollisions();
void checkPlayerPlayerCollisions();
void shoot2();
void rotatePlayer2(float direction);
void movePlayer2(float direction);

void initializePlayer() {
    textures[0].loadFromFile("images/sprite.png");
    textures[1].loadFromFile("images/sprite_move.png");
    textures[2].loadFromFile("images/bullet.png");
    textures[3].loadFromFile("images/asteroid.png");
    textures[4].loadFromFile("images/ufo.png");

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

    vsAIButton.setSize(sf::Vector2f(250.f, 70.f));
    vsAIButton.setPosition(screenSize.x / 2.f - 125.f, screenSize.y / 2.f + 300.f);
    vsAIButton.setFillColor(sf::Color(70, 70, 70));
    vsAIButton.setOutlineColor(sf::Color::White);
    vsAIButton.setOutlineThickness(3.f);

    vsAIButtonText.setFont(font);
    vsAIButtonText.setString("VS IA");
    vsAIButtonText.setCharacterSize(32);
    vsAIButtonText.setFillColor(sf::Color::White);
    sf::FloatRect vsAIBounds = vsAIButtonText.getLocalBounds();
    vsAIButtonText.setOrigin(vsAIBounds.left + vsAIBounds.width / 2.f,
                             vsAIBounds.top + vsAIBounds.height / 2.f);
    vsAIButtonText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f + 335.f);

    aiVisionCircle.setRadius(aiVisionRange);
    aiVisionCircle.setOrigin(aiVisionRange, aiVisionRange);
    aiVisionCircle.setFillColor(sf::Color::Transparent);
    aiVisionCircle.setOutlineColor(sf::Color(255, 255, 0, 100));
    aiVisionCircle.setOutlineThickness(2.f);

    easyButton.setSize(sf::Vector2f(250.f, 70.f));
    easyButton.setPosition(screenSize.x / 2.f - 125.f, screenSize.y / 2.f + 50.f);
    easyButton.setFillColor(sf::Color(0, 150, 0));
    easyButton.setOutlineColor(sf::Color::White);
    easyButton.setOutlineThickness(3.f);

    easyButtonText.setFont(font);
    easyButtonText.setString("FACIL");
    easyButtonText.setCharacterSize(32);
    easyButtonText.setFillColor(sf::Color::White);
    sf::FloatRect easyBounds = easyButtonText.getLocalBounds();
    easyButtonText.setOrigin(easyBounds.left + easyBounds.width / 2.f,
                             easyBounds.top + easyBounds.height / 2.f);
    easyButtonText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f + 85.f);

    mediumButton.setSize(sf::Vector2f(250.f, 70.f));
    mediumButton.setPosition(screenSize.x / 2.f - 125.f, screenSize.y / 2.f + 150.f);
    mediumButton.setFillColor(sf::Color(200, 150, 0));
    mediumButton.setOutlineColor(sf::Color::White);
    mediumButton.setOutlineThickness(3.f);

    mediumButtonText.setFont(font);
    mediumButtonText.setString("MEDIO");
    mediumButtonText.setCharacterSize(32);
    mediumButtonText.setFillColor(sf::Color::White);
    sf::FloatRect mediumBounds = mediumButtonText.getLocalBounds();
    mediumButtonText.setOrigin(mediumBounds.left + mediumBounds.width / 2.f,
                               mediumBounds.top + mediumBounds.height / 2.f);
    mediumButtonText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f + 185.f);

    hardButton.setSize(sf::Vector2f(250.f, 70.f));
    hardButton.setPosition(screenSize.x / 2.f - 125.f, screenSize.y / 2.f + 250.f);
    hardButton.setFillColor(sf::Color(200, 0, 0));
    hardButton.setOutlineColor(sf::Color::White);
    hardButton.setOutlineThickness(3.f);

    hardButtonText.setFont(font);
    hardButtonText.setString("DIFICIL");
    hardButtonText.setCharacterSize(32);
    hardButtonText.setFillColor(sf::Color::White);
    sf::FloatRect hardBounds = hardButtonText.getLocalBounds();
    hardButtonText.setOrigin(hardBounds.left + hardBounds.width / 2.f,
                             hardBounds.top + hardBounds.height / 2.f);
    hardButtonText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f + 285.f);

    difficultyTitleText.setFont(font);
    difficultyTitleText.setString("SELECCIONA DIFICULTAD");
    difficultyTitleText.setCharacterSize(48);
    difficultyTitleText.setFillColor(sf::Color::White);
    sf::FloatRect diffTitleBounds = difficultyTitleText.getLocalBounds();
    difficultyTitleText.setOrigin(diffTitleBounds.left + diffTitleBounds.width / 2.f,
                                  diffTitleBounds.top + diffTitleBounds.height / 2.f);
    difficultyTitleText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f - 100.f);
}

void initializeUfo() {
    sf::Sprite ufoSprite;
    sf::RectangleShape ufoHitbox;

    ufoSprite.setTexture(textures[4]);
    float diff = (float)(rand() % 25 - 50);
    bool up = rand() % 2;
    bool direction = rand() % 2;
    if (up)
        ufoSprite.setPosition(-100, screenSize.y / 4 + diff);
    else
        ufoSprite.setPosition(-100, screenSize.y * 3 / 4 - diff);
    sf::Vector2u ufoSize = textures[4].getSize();
    ufoSprite.setOrigin(ufoSize.x / 2.f, ufoSize.y / 2.f);
    ufoSprite.setScale(ufoScale, ufoScale);
    ufoSprite.setRotation(0.f);

    ufoHitbox.setSize(sf::Vector2f(ufoSize.x * ufoScale, ufoSize.y * ufoScale));
    ufoHitbox.setOrigin(ufoSize.x * ufoScale / 2.f, ufoSize.y * ufoScale / 2.f);
    ufoHitbox.setFillColor(sf::Color::Transparent);
    ufoHitbox.setOutlineColor(sf::Color::Red);
    ufoHitbox.setOutlineThickness(1.f);

    ufos.push_back(ufoSprite);
    ufoHitboxes.push_back(ufoHitbox);
    ufoDirections.push_back(direction);
    ufoShootCDs.push_back(60);
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

/*
 * 1. Obtener la transformación del rectángulo (incluye posición, rotación y escala)
 * 2. Invertir la transformación para convertir coordenadas globales → locales
 * 3. Transformar la posición del círculo al espacio local del rectángulo
 * 4. Ahora el rectángulo está en (0,0) sin rotación, aplicamos AABB normal
 * 5. Encontramos el punto más cercano del rectángulo al círculo
 * 6. Si la distancia es menor que el radio, hay colisión
 */
bool checkCollision(sf::RectangleShape& rect, sf::CircleShape& circle) {
    if (rect.getSize().x == 0.f || rect.getSize().y == 0.f) return false;

    sf::Transform transform = rect.getTransform();
    sf::Transform inverseTransform = transform.getInverse();

    sf::Vector2f localCirclePos = inverseTransform.transformPoint(circle.getPosition());

    sf::Vector2f rectSize = rect.getSize();
    sf::Vector2f rectOrigin = rect.getOrigin();
    float rectLeft = -rectOrigin.x;
    float rectRight = rectLeft + rectSize.x;
    float rectTop = -rectOrigin.y;
    float rectBottom = rectTop + rectSize.y;

    float radius = circle.getRadius();

    float closestX = std::max(rectLeft, std::min(localCirclePos.x, rectRight));
    float closestY = std::max(rectTop, std::min(localCirclePos.y, rectBottom));

    float dx = localCirclePos.x - closestX;
    float dy = localCirclePos.y - closestY;
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

void aiThink() {
    // Buscar nuevo objetivo cada vez que se llama
    float closestDistanceSquared = aiVisionRange * aiVisionRange;
    aiTargetIndex = -1;

    sf::Vector2f aiPos = player2Sprite.getPosition();

    for (size_t i = 0; i < asteroids.size(); i++) {
        sf::Vector2f asteroidPos = asteroids[i].getPosition();
        float dx = asteroidPos.x - aiPos.x;
        float dy = asteroidPos.y - aiPos.y;
        float distanceSquared = dx * dx + dy * dy;

        if (distanceSquared < closestDistanceSquared) {
            closestDistanceSquared = distanceSquared;
            aiTargetIndex = i;
        }
    }

    // Si hay un objetivo válido, moverse hacia él
    if (aiTargetIndex != -1 && aiTargetIndex < asteroids.size()) {
        sf::Vector2f targetPos = asteroids[aiTargetIndex].getPosition();

        // Calcular ángulo hacia el objetivo
        float dx = targetPos.x - aiPos.x;
        float dy = targetPos.y - aiPos.y;
        float angleToTarget = std::atan2(dy, dx) * 180.f / M_PI + 90.f;

        // Ángulo actual de la IA
        float currentAngle = player2Sprite.getRotation();

        // Calcular diferencia de ángulo
        float angleDiff = angleToTarget - currentAngle;

        // Normalizar a [-180, 180]
        while (angleDiff > 180.f) angleDiff -= 360.f;
        while (angleDiff < -180.f) angleDiff += 360.f;

        // Girar
        if (std::abs(angleDiff) > 5.f) {
            if (angleDiff > 0) {
                rotatePlayer2(1.f);
            } else {
                rotatePlayer2(-1.f);
            }
        }

        if (aiThinkTimer <= aiThinkInterval / 2) {
            // Moverse hacia adelante si está apuntando bien
            if (std::abs(angleDiff) < 30.f) {
                movePlayer2(1.f);
            }
        }

        // std::cout << "IA P2: " << aiThinkTimer << std::endl;
        if (aiThinkTimer == 0) {
            // Disparar si está apuntando al objetivo
            if (std::abs(angleDiff) < 30.f && bullet2CD == 0 && player2Respawning == -1) {
                shoot2();
                bullet2CD = 15 + aiThinkInterval;
            }
        }
    }
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

void ufoShoot() {
    for (size_t i = 0; i < ufos.size(); i++) {
        if (ufoShootCDs[i] == 0) {
            for (int j = 0; j < 4; j++) {
                sf::Sprite newBullet;
                newBullet.setTexture(textures[2]);
                newBullet.setOrigin(64.f, 64.f);
                newBullet.setPosition(ufos[i].getPosition());
                newBullet.setScale(0.1f, 0.1f);

                int direction = rand() % 4;

                float angle = (direction * 90 - 45) * M_PI / 180.f;
                sf::Vector2f bulletSpeed(cos(angle) * 10.f, sin(angle) * 10.f);

                sf::CircleShape bulletHitbox;
                bulletHitbox.setRadius(bulletRadius);
                bulletHitbox.setOrigin(bulletRadius, bulletRadius);
                bulletHitbox.setPosition(newBullet.getPosition());
                bulletHitbox.setFillColor(sf::Color::Red);
                bulletHitbox.setOutlineColor(sf::Color::Blue);
                bulletHitbox.setOutlineThickness(1.f);

                bullets.push_back(newBullet);
                bulletSpeeds.push_back(bulletSpeed);
                bulletHitboxes.push_back(bulletHitbox);
                bulletTimers.push_back(60);
                bulletOwners.push_back(3);
                ufoShootCDs[i] = 30;
            }
        } else {
            ufoShootCDs[i]--;
        }
    }
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
    bulletOwners.erase(bulletOwners.begin() + index);
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

void updateUfos() {
    for (size_t i = 0; i < ufos.size(); i++) {
        ufos[i].move(waves[currentWave].ufoSpeed * (ufoDirections[i] ? 1 : -1), 0.f);
        teleportInSides(ufos[i]);
        ufoHitboxes[i].setPosition(ufos[i].getPosition());
    }
}

void removeUfo(size_t index) {
    ufos.erase(ufos.begin() + index);
    ufoHitboxes.erase(ufoHitboxes.begin() + index);
    ufoDirections.erase(ufoDirections.begin() + index);
    ufoShootCDs.erase(ufoShootCDs.begin() + index);
}

void checkAsteroidCollisions() {
    for (int i = asteroids.size() - 1; i >= 0; i--) {
        for (int j = bullets.size() - 1; j >= 0; j--) {
            if (checkCollision(asteroidHitboxes[i], bulletHitboxes[j]) && bulletOwners[j] != 3) {
                removeAsteroid(i, bulletOwners[j]);
                removeBullet(j);
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
            // std::cout << "Colisión detectada con el asteroide" << std::endl;
            playerRespawning = 120;
            playerInvulnerable = 0;
            return true;
        }
    }
    for (size_t i = 0; i < ufos.size(); i++) {
        if (checkCollision(playerHitbox, ufoHitboxes[i])) {
            removeUfo(i);
            playerHitbox.setSize(sf::Vector2f(0.f, 0.f));
            playerSprite.setScale(sf::Vector2f(0.f, 0.f));
            // std::cout << "Colisión detectada con el asteroide" << std::endl;
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
            // std::cout << "Colisión detectada con el asteroide" << std::endl;
            player2Respawning = 120;
            player2Invulnerable = 0;
            return true;
        }
    }
    for (size_t i = 0; i < ufos.size(); i++) {
        if (checkCollision(playerHitbox, ufoHitboxes[i])) {
            removeUfo(i);
            playerHitbox.setSize(sf::Vector2f(0.f, 0.f));
            playerSprite.setScale(sf::Vector2f(0.f, 0.f));
            // std::cout << "Colisión detectada con el asteroide" << std::endl;
            playerRespawning = 120;
            playerInvulnerable = 0;
            return true;
        }
    }
    return false;
}

void checkBulletPlayerCollisions() {
    for (int i = bullets.size() - 1; i >= 0; i--) {
        if ((bulletOwners[i] == 2 || bulletOwners[i] == 3) && lives > 0 && playerRespawning == -1 &&
            playerInvulnerable == 0) {
            if (checkCollision(playerHitbox, bulletHitboxes[i])) {
                playerHitbox.setSize(sf::Vector2f(0.f, 0.f));
                playerSprite.setScale(sf::Vector2f(0.f, 0.f));
                if (bulletOwners[i] == 2) {
                    // std::cout << "P1 golpeado por P2!" << std::endl;
                    score2 += 500;
                } else {
                    // std::cout << "P1 golpeado por UFO!" << std::endl;
                }
                playerRespawning = 120;
                playerInvulnerable = 0;
                lives--;
                removeBullet(i);
                break;
            }
        }

        else if ((bulletOwners[i] == 1 || bulletOwners[i] == 3) && lives2 > 0 &&
                 player2Respawning == -1 && player2Invulnerable == 0) {
            if (checkCollision(player2Hitbox, bulletHitboxes[i])) {
                player2Hitbox.setSize(sf::Vector2f(0.f, 0.f));
                player2Sprite.setScale(sf::Vector2f(0.f, 0.f));
                if (bulletOwners[i] == 1) {
                    // std::cout << "P2 golpeado por P1!" << std::endl;
                    score += 500;
                } else {
                    // std::cout << "P2 golpeado por UFO!" << std::endl;
                }
                player2Respawning = 120;
                player2Invulnerable = 0;
                removeBullet(i);
                lives2--;
                break;
            }
        }
    }
}

void checkPlayerPlayerCollisions() {
    if (lives > 0 && lives2 > 0 && playerRespawning == -1 && player2Respawning == -1 &&
        playerInvulnerable == 0 && player2Invulnerable == 0) {
        if (checkCollision(playerHitbox, player2Hitbox)) {
            // std::cout << "Colisión entre jugadores!" << std::endl;

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

void checkBulletUfoCollisions() {
    for (size_t i = 0; i < bullets.size(); i++) {
        for (size_t j = 0; j < ufos.size(); j++) {
            if (bulletOwners[i] != 3 && checkCollision(ufoHitboxes[j], bulletHitboxes[i])) {
                removeUfo(j);
                if (bulletOwners[i] == 1) {
                    score += 250;
                } else if (bulletOwners[i] == 2) {
                    score2 += 250;
                }
                removeBullet(i);
                break;
            }
        }
    }
}

void render(sf::RenderWindow& window) {
    if (currentMode == DIFFICULTY_SELECT) {
        window.clear(sf::Color::Black);

        window.draw(difficultyTitleText);

        window.draw(easyButton);
        window.draw(easyButtonText);
        window.draw(mediumButton);
        window.draw(mediumButtonText);
        window.draw(hardButton);
        window.draw(hardButtonText);
    } else if (lives == -1) {
        window.clear(sf::Color::Black);

        window.draw(titleText);

        window.draw(startButton);
        window.draw(startButtonText);
        window.draw(player2Button);
        window.draw(player2ButtonText);
        window.draw(vsModeButton);
        window.draw(vsModeButtonText);
        window.draw(vsAIButton);
        window.draw(vsAIButtonText);
    } else if (lives > 0 || lives2 > 0) {
        // GAME RENDER
        window.clear(sf::Color::Black);

        if (lives > 0) {
            if (playerInvulnerable == 0 || (playerInvulnerable / 10) % 2 == 0) {
                window.draw(playerSprite);
                window.draw(playerHitbox);
            }
        }

        if (lives2 > 0 && currentMode == TWO_PLAYERS) {
            if (player2Invulnerable == 0 || (player2Invulnerable / 10) % 2 == 0) {
                window.draw(player2Sprite);
                // window.draw(player2Hitbox);
            }

            if (isAI) {
                aiVisionCircle.setPosition(player2Sprite.getPosition());
                window.draw(aiVisionCircle);
            }
        }

        for (size_t i = 0; i < ufos.size(); i++) {
            window.draw(ufos[i]);
            // window.draw(ufoHitboxes[i]);
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
        if (lives2 >= 0) {
            if (score > score2) {
                finalScoreText.setString("Ganador: P1\n Puntaje: " + std::to_string(score));
            } else {
                std::string winnerName = isAI ? "IA" : "P2";
                finalScoreText.setString("Ganador: " + winnerName +
                                         "\n Puntaje: " + std::to_string(score2));
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
    currentMode = MENU;

    while (window.isOpen()) {
        if (lives == -1) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();
                if (currentMode == MENU) {
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
                            isAI = false;
                            initializePlayer();
                            initializePlayer2();
                        } else if (vsModeButton.getGlobalBounds().contains(
                                       static_cast<sf::Vector2f>(mousePos))) {
                            lives = 3;
                            lives2 = 3;
                            currentMode = TWO_PLAYERS;
                            vsMode = true;
                            isAI = false;
                            initializePlayer();
                            initializePlayer2();
                        } else if (vsAIButton.getGlobalBounds().contains(
                                       static_cast<sf::Vector2f>(mousePos))) {
                            currentMode = DIFFICULTY_SELECT;
                        }
                    }

                } else if (currentMode == DIFFICULTY_SELECT) {
                    if (event.type == sf::Event::MouseButtonReleased &&
                        event.mouseButton.button == sf::Mouse::Left) {
                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                        if (easyButton.getGlobalBounds().contains(
                                static_cast<sf::Vector2f>(mousePos))) {
                            aiDifficulty = AI_EASY;
                            aiThinkInterval = 10.f;
                            lives = 3;
                            lives2 = 3;
                            currentMode = TWO_PLAYERS;
                            vsMode = true;
                            isAI = true;
                            aiThinkTimer = aiThinkInterval;
                            initializePlayer();
                            initializePlayer2();
                        } else if (mediumButton.getGlobalBounds().contains(
                                       static_cast<sf::Vector2f>(mousePos))) {
                            aiDifficulty = AI_MEDIUM;
                            aiThinkInterval = 5.f;
                            lives = 3;
                            lives2 = 3;
                            currentMode = TWO_PLAYERS;
                            vsMode = true;
                            isAI = true;
                            aiThinkTimer = aiThinkInterval;
                            initializePlayer();
                            initializePlayer2();
                        } else if (hardButton.getGlobalBounds().contains(
                                       static_cast<sf::Vector2f>(mousePos))) {
                            aiDifficulty = AI_HARD;
                            aiThinkInterval = 1.f;
                            lives = 3;
                            lives2 = 3;
                            currentMode = TWO_PLAYERS;
                            vsMode = true;
                            isAI = true;
                            aiThinkTimer = aiThinkInterval;
                            initializePlayer();
                            initializePlayer2();
                        }
                    }
                }
            }

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            if (currentMode == MENU) {
                if (startButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    startButton.setFillColor(sf::Color(150, 150, 150));
                } else {
                    startButton.setFillColor(sf::Color(70, 70, 70));
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

                if (vsAIButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    vsAIButton.setFillColor(sf::Color(150, 150, 150));
                } else {
                    vsAIButton.setFillColor(sf::Color(100, 100, 100));
                }
            }

            else if (currentMode == DIFFICULTY_SELECT) {
                if (easyButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    easyButton.setFillColor(sf::Color(0, 200, 0));
                } else {
                    easyButton.setFillColor(sf::Color(0, 150, 0));
                }

                if (mediumButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    mediumButton.setFillColor(sf::Color(255, 200, 0));
                } else {
                    mediumButton.setFillColor(sf::Color(200, 150, 0));
                }

                if (hardButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                    hardButton.setFillColor(sf::Color(255, 0, 0));
                } else {
                    hardButton.setFillColor(sf::Color(200, 0, 0));
                }
            }

            // std::cout << "Current mode: " << currentMode << std::endl;
        } else if (lives > 0 || lives2 > 0) {
            // GAME MAIN
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
                    rotatePlayer(1.f);
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
                    // std::cout << "Disparo P1" << std::endl;
                    shoot();
                }

                if (bulletCD > 0) bulletCD--;

                if (ufoCD > 0)
                    ufoCD--;
                else if (ufoCD == 0) {
                    ufoCD = waves[currentWave].ufoCD;
                    initializeUfo();
                    // std::cout << "UFO" << std::endl;
                }
                updateUfos();

                ufoShoot();

                checkBulletUfoCollisions();

                if (playerRespawning > 0) playerRespawning--;
                if (playerRespawning == 0) initializePlayer();

                if (playerInvulnerable > 0) playerInvulnerable--;

                scoreText.setString("P1: " + std::to_string(score));
                if (checkPlayerCollisions()) lives--;
            }

            if (lives2 > 0 && currentMode == TWO_PLAYERS) {
                updatePlayer2();

                if (!isAI) {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
                        rotatePlayer2(-1.f);
                    }

                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
                        rotatePlayer2(1.f);
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
                        // std::cout << "Disparo P2" << std::endl;
                        shoot2();
                    }
                } else {
                    if (aiThinkTimer > 0) aiThinkTimer--;
                    aiThink();
                    if (aiThinkTimer == 0) {
                        aiThinkTimer = aiThinkInterval;
                    }
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

            if (vsMode || isAI) {
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
                        currentMode = MENU;
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
