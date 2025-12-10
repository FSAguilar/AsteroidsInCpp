#include <SFML/Graphics.hpp>
#define _USE_MATH_DEFINES
#include <cmath>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML Basico");
    window.setFramerateLimit(60);

    sf::Texture textures[2];
    if (!textures[0].loadFromFile("images/sprite.png") || !textures[1].loadFromFile("images/sprite_move.png"))
        return -1;


    sf::Sprite player;
    player.setTexture(textures[0]);
    player.setPosition(375.f, 275.f);
    float scale = 0.3;
    sf::Vector2u size = textures[0].getSize();
    player.setOrigin(size.x / 2.f, 0.f);
    player.setScale(scale, scale);
    player.setRotation(0.f);
    sf::Vector2f velocity(0.f, 0.f);  
    float acceleration = 0.2f;    
    float friction = 0.98f; 
    float rotationSpeed = 5.f; 
    while (window.isOpen()) {
        player.setTexture(textures[0]);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            player.rotate(-rotationSpeed);
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            player.rotate(rotationSpeed);
        

        float angle = (player.getRotation() - 90.f) * M_PI / 180.f;
        

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            velocity.x += std::cos(angle) * acceleration;
            velocity.y += std::sin(angle) * acceleration;
            player.setTexture(textures[1]);
        }
        
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            velocity.x -= std::cos(angle) * acceleration;
            velocity.y -= std::sin(angle) * acceleration;
            player.setTexture(textures[0]);
        }
        

        velocity *= friction;
        

        player.move(velocity);
        
        window.clear(sf::Color::Black);
        window.draw(player);
        window.display();
    }
    
    return 0;
}
