#include "Bullet.h"

#include <cmath>

// Inicializar textura estática
sf::Texture Bullet::texture;

Bullet::Bullet(sf::Texture& texture, sf::Vector2f position, sf::Vector2f initialSpeed) {
    currentSpeed = initialSpeed;

    sprite.setTexture(texture);
    sprite.setOrigin(64.f, 64.f);
    sprite.setPosition(position);
    sprite.setScale(0.1f, 0.1f);

    // Configurar hitbox circular
    radius = 3.f;
    hitboxShape.setRadius(radius);
    hitboxShape.setOrigin(radius, radius);
    hitboxShape.setPosition(position);
    hitboxShape.setFillColor(sf::Color::Yellow);
    hitboxShape.setOutlineColor(sf::Color::Green);
    hitboxShape.setOutlineThickness(1.f);
}

void Bullet::update() {
    sprite.move(currentSpeed);
    currentSpeed.x *= friction;
    currentSpeed.y *= friction;

    hitboxShape.setPosition(sprite.getPosition());
}

void Bullet::draw(sf::RenderWindow& window) {
    window.draw(sprite);
    // window.draw(hitboxShape);
}

bool Bullet::shouldRemove() const {
    return (std::abs(currentSpeed.x) < 0.01f && std::abs(currentSpeed.y) < 0.01f);
}

Bullet::~Bullet() {
    // Destructor, eliminar punteros y cosas
}

sf::Vector2f Bullet::getPosition() const { return sprite.getPosition(); }

float Bullet::getRadius() const { return radius; }
