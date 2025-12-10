#include "Player.h"
#define _USE_MATH_DEFINES
#include <algorithm>
#include <cmath>

Player::Player() {
    speed = 0.2f;
    currentSpeed = sf::Vector2f(0.f, 0.f);
    friction = 0.98f;
    rotationSpeed = 5.f;

    textures[0].loadFromFile("images/sprite.png");
    textures[1].loadFromFile("images/sprite_move.png");
    sprite.setTexture(textures[0]);
    sprite.setPosition(375.f, 275.f);

    float scale = 0.3;
    sf::Vector2u size = textures[0].getSize();
    sprite.setOrigin(size.x / 2.f, 0.f);
    sprite.setScale(scale, scale);
    sprite.setRotation(0.f);
}

void Player::update() {
    sprite.setTexture(textures[0]);
    currentSpeed *= friction;
    sprite.move(currentSpeed);

    // Actualizar todas las balas
    for (auto& bullet : bullets) {
        bullet.update();
    }

    // Eliminar balas que deben ser removidas
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                                 [](const Bullet& b) { return b.shouldRemove(); }),
                  bullets.end());
}

void Player::draw(sf::RenderWindow& window) {
    window.draw(sprite);

    // Dibujar todas las balas
    for (auto& bullet : bullets) {
        bullet.draw(window);
    }
}

void Player::rotateLeft() { sprite.rotate(-rotationSpeed); }

void Player::rotateRight() { sprite.rotate(+rotationSpeed); }

void Player::moveForward() {
    currentSpeed.x += speed * cos((sprite.getRotation() - 90) * M_PI / 180.f);
    currentSpeed.y += speed * sin((sprite.getRotation() - 90) * M_PI / 180.f);
    sprite.setTexture(textures[1]);
}

void Player::moveBackward() {
    currentSpeed.x -= speed * cos((sprite.getRotation() - 90) * M_PI / 180.f);
    currentSpeed.y -= speed * sin((sprite.getRotation() - 90) * M_PI / 180.f);
    sprite.setTexture(textures[1]);
}

void Player::shoot() {
    bullets.push_back(Bullet(sprite.getPosition(),
                             sf::Vector2f(cos((sprite.getRotation() - 90) * M_PI / 180.f) * 5.f,
                                          sin((sprite.getRotation() - 90) * M_PI / 180.f) * 5.f)));
}