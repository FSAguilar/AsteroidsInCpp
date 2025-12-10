#ifndef BULLET_H
#define BULLET_H

#include <SFML/Graphics.hpp>

class Bullet {
   private:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Vector2f currentSpeed;
    sf::Vector2f position;
    // float speed;
    float friction = 0.98f;

   public:
    Bullet(sf::Vector2f position, sf::Vector2f initialSpeed);
    ~Bullet();
    void update();
    void draw(sf::RenderWindow& window);
    bool shouldRemove() const;  // Retorna true si debe eliminarse
};

#endif