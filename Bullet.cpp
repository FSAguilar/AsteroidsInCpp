#include "Bullet.h"

#include <cmath>

Bullet::Bullet(sf::Vector2f position, sf::Vector2f initialSpeed) {
    texture.loadFromFile("images/bullet.png");
    sprite.setTexture(texture);
    sprite.setPosition(position);
    sprite.setOrigin(0.f, 0.f);
    sprite.setScale(0.1f, 0.1f);
    currentSpeed = initialSpeed;
}

void Bullet::update() {
    sprite.move(currentSpeed);
    currentSpeed.x *= friction;
    currentSpeed.y *= friction;
}

void Bullet::draw(sf::RenderWindow& window) { window.draw(sprite); }

bool Bullet::shouldRemove() const {
    // Eliminar si la velocidad es muy baja
    return (std::abs(currentSpeed.x) < 0.01f && std::abs(currentSpeed.y) < 0.01f);
}

Bullet::~Bullet() {
    // Destructor - se llama automáticamente cuando se elimina del vector
    // Aquí podrías liberar recursos si tuvieras punteros o archivos abiertos
}
