#include <SFML/Graphics.hpp>
#include <cstdlib>

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

// Variables globales del jugador
sf::Sprite playerSprite;
sf::Vector2f playerCurrentSpeed(0.f, 0.f);
float playerSpeed = 0.2f;
float playerFriction = 0.98f;
float playerRotationSpeed = 5.f;
float playerScale = 0.3f;
sf::Texture textures[4];
sf::RectangleShape playerHitbox;
sf::Vector2f screenSize(1400.f, 900.f);

// Variables globales de las balas
std::vector<sf::Sprite> bullets;
std::vector<sf::Vector2f> bulletSpeeds;
std::vector<int> bulletTimers;
std::vector<sf::CircleShape> bulletHitboxes;
float bulletFriction = 0.98f;
float bulletRadius = 3.f;
int bulletCD = 0;

// Variables globales de los asteroides
std::vector<sf::Sprite> asteroids;
std::vector<sf::Vector2f> asteroidSpeeds;
std::vector<sf::CircleShape> asteroidHitboxes;
float asteroidRadius1 = 50.f;
float asteroidRadius2 = 25.f;
float asteroidRadius3 = 12.5;

// Funciones sin parámetros
void initializePlayer() {
    textures[0].loadFromFile("images/sprite.png");
    textures[1].loadFromFile("images/sprite_move.png");
    textures[2].loadFromFile("images/bullet.png");
    textures[3].loadFromFile("images/asteroid.png");

    playerSprite.setTexture(textures[0]);
    playerSprite.setPosition(375.f, 275.f);
    sf::Vector2u playerSize = textures[0].getSize();
    playerSprite.setOrigin(playerSize.x / 2.f, 0.f);
    playerSprite.setScale(playerScale, playerScale);
    playerSprite.setRotation(0.f);

    playerHitbox.setSize(sf::Vector2f(playerSize.x * playerScale / 2, playerSize.y * playerScale));
    playerHitbox.setOrigin(10.f, 0.f);
    playerHitbox.setFillColor(sf::Color::Transparent);
    playerHitbox.setOutlineColor(sf::Color::Green);
    playerHitbox.setOutlineThickness(1.f);
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
        newAsteroid.setPosition(rand() % 800, rand() % 600);
    } while (newAsteroid.getPosition().x > 200 && newAsteroid.getPosition().x < 600 &&
             newAsteroid.getPosition().y > 150 && newAsteroid.getPosition().y < 450);
    newAsteroid.setScale(1.f, 1.f);

    float angle = (rand() % 360) * M_PI / 180.f;
    sf::Vector2f bulletSpeed(cos(angle) * 62.5f / radius, sin(angle) * 62.5f / radius);

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
    sf::Vector2f bulletSpeed(cos(angle) * 62.5f / (radius * 1.5),
                             sin(angle) * 62.5f / (radius * 1.5));

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
    if (asteroids[index].getScale().x == 1.f) {
        spawnAsteroid(asteroidRadius2, asteroids[index]);
        spawnAsteroid(asteroidRadius2, asteroids[index]);

    } else if (asteroids[index].getScale().x == 0.5f) {
        spawnAsteroid(asteroidRadius3, asteroids[index]);
        spawnAsteroid(asteroidRadius3, asteroids[index]);
    }

    asteroids.erase(asteroids.begin() + index);
    asteroidSpeeds.erase(asteroidSpeeds.begin() + index);
    asteroidHitboxes.erase(asteroidHitboxes.begin() + index);
}
void checkAsteroidCollisions() {
    // Iterar hacia atrás para evitar problemas al eliminar elementos
    for (int i = asteroids.size() - 1; i >= 0; i--) {
        for (int j = bullets.size() - 1; j >= 0; j--) {
            if (checkCollision(asteroidHitboxes[i], bulletHitboxes[j])) {
                removeAsteroid(i);
                removeBullet(j);
                break;  // Salir del loop interno después de eliminar
            }
        }
    }
}

void render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    window.draw(playerSprite);
    window.draw(playerHitbox);

    for (size_t i = 0; i < bullets.size(); i++) {
        window.draw(bullets[i]);
        // window.draw(bulletHitboxes[i]);
    }

    for (size_t i = 0; i < asteroids.size(); i++) {
        window.draw(asteroids[i]);
        // window.draw(asteroidHitboxes[i]);
    }

    window.display();
}

int main() {
    sf::RenderWindow window(sf::VideoMode(screenSize.x, screenSize.y), "Asteroids");
    window.setFramerateLimit(60);

    initializePlayer();

    while (window.isOpen()) {
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
            bulletCD = 30;
            shoot();
        }

        if (bulletCD > 0) bulletCD--;

        if (asteroids.empty()) {
            for (int i = 0; i < 4; i++) spawnAsteroid(asteroidRadius1);
        }

        checkAsteroidCollisions();
        updateBullets();
        updateAsteroids();
        removeBullets();
        render(window);
    }

    return 0;
}
