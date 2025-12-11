#ifndef BULLET_H
#define BULLET_H

#include <SFML/Graphics.hpp>

class Bullet {
   private:
    sf::Sprite sprite;
    static sf::Texture texture;
    sf::Vector2f currentSpeed;
    sf::Vector2f position;
    float friction = 0.98f;
    float radius;
    sf::CircleShape hitboxShape;

   public:
    Bullet(sf::Texture& texture, sf::Vector2f position, sf::Vector2f initialSpeed);
    ~Bullet();
    void update();
    void draw(sf::RenderWindow& window);
    bool shouldRemove() const;
    sf::Vector2f getPosition() const;
    float getRadius() const;
};

#endif