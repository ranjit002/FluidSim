#include <SFML/Graphics.hpp>
#include <iostream>
#include "globals.h"
#include "Ensemble.h"

const int WINDOW_HEIGHT = 800;
const int WINDOW_WIDTH = 800;
const int FRAME_RATE = 120;
const float DT = 1 / (float)FRAME_RATE;
const float COLLISION_DAMPING = 0.999; // Fractional
const sf::Vector2f GRAVITY = sf::Vector2f(0, 700);


int main() {

    sf::RenderWindow window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Simulation");
    window.setFramerateLimit(FRAME_RATE);

    int numParticles = 10000;
    float radius = 2.0f;
    Ensemble ensemble(numParticles, radius);
    
    ensemble.setAcceleration(GRAVITY);

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear();

        ensemble.collideBorder();
        ensemble.collideParticles();
        ensemble.update(DT);
        ensemble.draw(window);

        window.display();
    }
    
    return 0;
}
