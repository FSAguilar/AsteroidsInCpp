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
    sf::Texture textures[2];
    std::vector<Bullet> bullets;

   public:
    Player();
    void update();
    void draw(sf::RenderWindow& window);
    void rotateLeft();
    void rotateRight();
    void moveForward();
    void moveBackward();
    void shoot();
};

#endif
