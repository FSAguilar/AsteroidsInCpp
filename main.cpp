#include <SFML/Graphics.hpp>

#include "Bullet.h"
#include "Player.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Basico");
    window.setFramerateLimit(60);

    int bulletCD = 0;

    Player player;
    // sf::Sprite player;

    while (window.isOpen()) {
        sf::Event event;
        player.update();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) player.rotateLeft();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) player.rotateRight();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) player.moveForward();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) player.moveBackward();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && bulletCD == 0) {
            player.shoot();
            bulletCD = 30;
        };

        if (bulletCD > 0) bulletCD--;

        window.clear(sf::Color::Black);
        player.draw(window);
        window.display();
    }

    return 0;
}
