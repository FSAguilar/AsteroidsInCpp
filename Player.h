#ifndef PLAYER_H
#define PLAYER_H

#include <SFML/Graphics.hpp>
#include <vector>

#include "Bullet.h"

class Player {
   private:
    sf::Sprite sprite;
    sf::Vector2f currentSpeed;
    float speed;
    float friction;
    float rotationSpeed;
    float scale;
    sf::Texture textures[3];
    std::vector<Bullet> bullets;
    sf::RectangleShape hitboxShape;

   public:
    Player();
    void setupHitbox();
    sf::RectangleShape getHitbox();
    void update();
    void draw(sf::RenderWindow& window);
    void rotateLeft();
    void rotateRight();
    void moveForward();
    void moveBackward();
    void shoot();
};

#endif
