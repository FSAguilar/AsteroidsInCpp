#include <SFML/Graphics.hpp>
#include <iostream>

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

sf::Sprite playerSprite;
sf::Vector2f playerCurrentSpeed(0.f, 0.f);
float playerSpeed = 0.2f;
float playerFriction = 0.98f;
float playerRotationSpeed = 5.f;
float playerScale = 0.3f;
int lives = -1;
sf::Sprite livesSprites[3];
int playerRespawning = -1;
sf::Texture textures[4];
sf::RectangleShape playerHitbox;
sf::Vector2f screenSize(1400.f, 900.f);

std::vector<sf::Sprite> bullets;
std::vector<sf::Vector2f> bulletSpeeds;
std::vector<int> bulletTimers;
std::vector<sf::CircleShape> bulletHitboxes;
float bulletFriction = 0.98f;
float bulletRadius = 4.f;
int bulletCD = 0;

std::vector<sf::Sprite> asteroids;
std::vector<sf::Vector2f> asteroidSpeeds;
std::vector<sf::CircleShape> asteroidHitboxes;
float asteroidScale = 2.f;
float asteroidRadius1 = 100.f;
float asteroidRadius2 = 50.f;
float asteroidRadius3 = 25.f;

sf::Font font;
sf::Text scoreText;
sf::RectangleShape restartButton;
sf::Text restartButtonText;
sf::RectangleShape startButton;
sf::Text startButtonText;

int score = 0;

bool checkPlayerCollisions();

void initializePlayer() {
    lives = 3;
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
    } else {
        playerRespawning = 0;
        lives++;
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
}

void teleportInSides(sf::Sprite& sprite) {
    if (sprite.getPosition().x < -50) sprite.setPosition(screenSize.x + 50, sprite.getPosition().y);
    if (sprite.getPosition().x > screenSize.x + 50) sprite.setPosition(-50, sprite.getPosition().y);
    if (sprite.getPosition().y < -50) sprite.setPosition(sprite.getPosition().x, screenSize.y + 50);
    if (sprite.getPosition().y > screenSize.y + 50) sprite.setPosition(sprite.getPosition().x, -50);
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
    sf::Sprite newAsteroid;
    newAsteroid.setTexture(textures[3]);
    newAsteroid.setOrigin(64.f, 64.f);
    do {
        newAsteroid.setPosition(rand() % (int)screenSize.x, rand() % (int)screenSize.y);
    } while (newAsteroid.getPosition().x > 100 &&
             newAsteroid.getPosition().x < screenSize.x - 100 &&
             newAsteroid.getPosition().y > 100 && newAsteroid.getPosition().y < screenSize.y - 100);
    newAsteroid.setScale(asteroidScale, asteroidScale);

    float angle = (rand() % 360) * M_PI / 180.f;
    sf::Vector2f bulletSpeed(cos(angle) * 200.f / radius, sin(angle) * 200.f / radius);

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

void removeAsteroid(size_t index) {
    if (asteroids[index].getScale().x == asteroidScale) {
        spawnAsteroid(asteroidRadius2, asteroids[index]);
        spawnAsteroid(asteroidRadius2, asteroids[index]);
        score += 20;

    } else if (asteroids[index].getScale().x == asteroidScale / 2.f) {
        spawnAsteroid(asteroidRadius3, asteroids[index]);
        spawnAsteroid(asteroidRadius3, asteroids[index]);
        score += 50;
    } else {
        score += 100;
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
                removeAsteroid(i);
                break;
            }
        }
    }
}

bool checkPlayerCollisions() {
    for (size_t i = 0; i < asteroids.size(); i++) {
        if (checkCollision(playerHitbox, asteroidHitboxes[i])) {
            removeAsteroid(i);
            playerHitbox.setSize(sf::Vector2f(0.f, 0.f));
            playerSprite.setScale(sf::Vector2f(0.f, 0.f));
            std::cout << "Colisión detectada con el asteroide" << std::endl;
            playerRespawning = 120;
            lives--;
            return true;
            break;
        }
    }
    return false;
}

void render(sf::RenderWindow& window) {
    if (lives == -1) {
        window.clear(sf::Color::Black);

        sf::Text titleText;
        titleText.setFont(font);
        titleText.setString("ASTEROIDS");
        titleText.setCharacterSize(80);
        titleText.setFillColor(sf::Color::White);
        sf::FloatRect titleBounds = titleText.getLocalBounds();
        titleText.setOrigin(titleBounds.left + titleBounds.width / 2.f,
                            titleBounds.top + titleBounds.height / 2.f);
        titleText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f - 150.f);
        window.draw(titleText);

        window.draw(startButton);
        window.draw(startButtonText);
    } else if (lives > 0) {
        window.clear(sf::Color::Black);
        window.draw(playerSprite);
        window.draw(playerHitbox);

        for (size_t i = 0; i < bullets.size(); i++) {
            window.draw(bullets[i]);
        }

        for (size_t i = 0; i < asteroids.size(); i++) {
            window.draw(asteroids[i]);
            window.draw(asteroidHitboxes[i]);
        }

        window.draw(scoreText);

        for (int i = 0; i < lives; i++) {
            window.draw(livesSprites[i]);
        }

    } else {
        window.clear(sf::Color::Black);

        sf::Text gameoverText;
        gameoverText.setFont(font);
        gameoverText.setString("Game Over");
        gameoverText.setCharacterSize(60);
        gameoverText.setFillColor(sf::Color::White);
        sf::FloatRect gameoverBounds = gameoverText.getLocalBounds();
        gameoverText.setOrigin(gameoverBounds.left + gameoverBounds.width / 2.f,
                               gameoverBounds.top + gameoverBounds.height / 2.f);
        gameoverText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f - 100.f);
        window.draw(gameoverText);

        sf::Text finalScoreText;
        finalScoreText.setFont(font);
        finalScoreText.setString("Puntaje: " + std::to_string(score));
        finalScoreText.setCharacterSize(36);
        finalScoreText.setFillColor(sf::Color::White);
        sf::FloatRect scoreBounds = finalScoreText.getLocalBounds();
        finalScoreText.setOrigin(scoreBounds.left + scoreBounds.width / 2.f,
                                 scoreBounds.top + scoreBounds.height / 2.f);
        finalScoreText.setPosition(screenSize.x / 2.f, screenSize.y / 2.f - 20.f);
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
                        initializePlayer();
                    }
                }
            }

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (startButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                startButton.setFillColor(sf::Color(150, 150, 150));
            } else {
                startButton.setFillColor(sf::Color(100, 100, 100));
            }

            render(window);
        } else if (lives > 0) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();
            }

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

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && bulletCD == 0) {
                bulletCD = 15;
                std::cout << "Disparo" << std::endl;
                shoot();
            }

            if (bulletCD > 0) bulletCD--;

            if (asteroids.empty()) {
                for (int i = 0; i < 5; i++) spawnAsteroid(asteroidRadius1);
            }

            if (playerRespawning > 0) playerRespawning--;
            if (playerRespawning == 0) initializePlayer();

            scoreText.setString(std::to_string(score));
            checkAsteroidCollisions();
            checkPlayerCollisions();
            updateBullets();
            updateAsteroids();
            removeBullets();
            render(window);
        } else {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) window.close();

                if (event.type == sf::Event::MouseButtonReleased &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    if (restartButton.getGlobalBounds().contains(
                            static_cast<sf::Vector2f>(mousePos))) {
                        lives = 3;
                        score = 0;
                        asteroids.clear();
                        asteroidSpeeds.clear();
                        asteroidHitboxes.clear();
                        bullets.clear();
                        bulletSpeeds.clear();
                        bulletHitboxes.clear();
                        bulletTimers.clear();
                        initializePlayer();
                    }
                }
            }

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            if (restartButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos))) {
                restartButton.setFillColor(sf::Color(100, 100, 100));
            } else {
                restartButton.setFillColor(sf::Color(70, 70, 70));
            }

            render(window);
        }
    }

    return 0;
}
