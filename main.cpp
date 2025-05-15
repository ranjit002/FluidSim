#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

#include "globals.hpp"
#include "Ensemble.hpp"

const int WINDOW_HEIGHT = 800;
const int WINDOW_WIDTH = 800;
const int FRAME_RATE = 120;
const float DT = 1 / (float)FRAME_RATE;
const float COLLISION_DAMPING = 0.99; // Fractional
const sf::Vector2f GRAVITY = sf::Vector2f(0, 500);


int main() {

    sf::RenderWindow window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Simulation");
    window.setFramerateLimit(FRAME_RATE);

    int num_particles = 5000;
    Ensemble ensemble(num_particles);
    
    while (window.isOpen()) {

        while (const std::optional event = window.pollEvent()) {
            
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear();

        ensemble.set_acceleration(GRAVITY);
        ensemble.border_collision();
        ensemble.collideParticles();
        ensemble.update(DT);
        ensemble.draw(window);

        window.display();
    }
    
    return 0;
}
