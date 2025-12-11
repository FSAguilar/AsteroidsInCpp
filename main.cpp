#include <SFML/Graphics.hpp>

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Basico");
    window.setFramerateLimit(60);

    // ========== VARIABLES DEL JUGADOR ==========
    sf::Sprite playerSprite;
    sf::Vector2f playerCurrentSpeed(0.f, 0.f);
    float playerSpeed = 0.2f;
    float playerFriction = 0.98f;
    float playerRotationSpeed = 5.f;
    float playerScale = 0.3f;

    // Texturas del jugador
    sf::Texture playerTextures[3];
    playerTextures[0].loadFromFile("images/sprite.png");
    playerTextures[1].loadFromFile("images/sprite_move.png");
    playerTextures[2].loadFromFile("images/bullet.png");

    // Configurar sprite del jugador
    playerSprite.setTexture(playerTextures[0]);
    playerSprite.setPosition(375.f, 275.f);
    sf::Vector2u playerSize = playerTextures[0].getSize();
    playerSprite.setOrigin(playerSize.x / 2.f, 0.f);
    playerSprite.setScale(playerScale, playerScale);
    playerSprite.setRotation(0.f);

    // Hitbox del jugador
    sf::RectangleShape playerHitbox;
    playerHitbox.setSize(sf::Vector2f(playerSize.x * playerScale / 2, playerSize.y * playerScale));
    playerHitbox.setOrigin(10.f, 0.f);
    playerHitbox.setFillColor(sf::Color::Transparent);
    playerHitbox.setOutlineColor(sf::Color::Green);
    playerHitbox.setOutlineThickness(1.f);

    // ========== VARIABLES DE LAS BALAS ==========
    std::vector<sf::Sprite> bullets;
    std::vector<sf::Vector2f> bulletSpeeds;
    std::vector<sf::CircleShape> bulletHitboxes;
    float bulletFriction = 0.98f;
    float bulletRadius = 3.f;
    int bulletCD = 0;

    // ========== GAME LOOP ==========
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        // ========== ACTUALIZAR JUGADOR ==========
        playerSprite.setTexture(playerTextures[0]);
        playerCurrentSpeed *= playerFriction;
        playerSprite.move(playerCurrentSpeed);
        playerHitbox.setPosition(playerSprite.getPosition());
        playerHitbox.setRotation(playerSprite.getRotation());

        // ========== INPUT DEL JUGADOR ==========
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            playerSprite.rotate(-playerRotationSpeed);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            playerSprite.rotate(+playerRotationSpeed);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            playerCurrentSpeed.x +=
                playerSpeed * cos((playerSprite.getRotation() - 90) * M_PI / 180.f);
            playerCurrentSpeed.y +=
                playerSpeed * sin((playerSprite.getRotation() - 90) * M_PI / 180.f);
            playerSprite.setTexture(playerTextures[1]);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            playerCurrentSpeed.x -=
                playerSpeed * cos((playerSprite.getRotation() - 90) * M_PI / 180.f);
            playerCurrentSpeed.y -=
                playerSpeed * sin((playerSprite.getRotation() - 90) * M_PI / 180.f);
            playerSprite.setTexture(playerTextures[1]);
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && bulletCD == 0) {
            // Crear nueva bala
            sf::Sprite newBullet;
            newBullet.setTexture(playerTextures[2]);
            newBullet.setOrigin(64.f, 64.f);
            newBullet.setPosition(playerSprite.getPosition());
            newBullet.setScale(0.1f, 0.1f);

            // Velocidad inicial de la bala
            sf::Vector2f bulletSpeed(cos((playerSprite.getRotation() - 90) * M_PI / 180.f) * 10.f,
                                     sin((playerSprite.getRotation() - 90) * M_PI / 180.f) * 10.f);

            // Hitbox de la bala
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

            bulletCD = 30;
        }

        if (bulletCD > 0) bulletCD--;

        // ========== ACTUALIZAR BALAS ==========
        for (size_t i = 0; i < bullets.size(); i++) {
            bullets[i].move(bulletSpeeds[i]);
            bulletSpeeds[i].x *= bulletFriction;
            bulletSpeeds[i].y *= bulletFriction;
            bulletHitboxes[i].setPosition(bullets[i].getPosition());
        }

        // ========== ELIMINAR BALAS LENTAS ==========
        for (size_t i = 0; i < bullets.size();) {
            if (std::abs(bulletSpeeds[i].x) < 0.01f && std::abs(bulletSpeeds[i].y) < 0.01f) {
                bullets.erase(bullets.begin() + i);
                bulletSpeeds.erase(bulletSpeeds.begin() + i);
                bulletHitboxes.erase(bulletHitboxes.begin() + i);
            } else {
                i++;
            }
        }

        // ========== RENDERIZAR ==========
        window.clear(sf::Color::Black);

        // Dibujar jugador
        window.draw(playerSprite);
        window.draw(playerHitbox);

        // Dibujar balas
        for (size_t i = 0; i < bullets.size(); i++) {
            window.draw(bullets[i]);
            // window.draw(bulletHitboxes[i]); // Descomentar para ver hitboxes
        }

        window.display();
    }

    return 0;
}
