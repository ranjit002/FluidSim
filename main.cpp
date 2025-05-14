#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>

#include "Particle.hpp"
#include "globals.hpp"
#include "Ensemble.hpp"

const int WINDOW_HEIGHT = 800;
const int WINDOW_WIDTH = 800;
const int FRAME_RATE = 120;
const float DT = 1 / (float)FRAME_RATE;
const float COLLISION_DAMPING = 0.99; // Fractional
const sf::Vector2f GRAVITY = sf::Vector2f(0, 500);


int main()
{
    sf::RenderWindow window(sf::VideoMode({ WINDOW_WIDTH, WINDOW_HEIGHT }), "Simulation");
    window.setFramerateLimit(FRAME_RATE);

    int N_particles = 2000;
    float radius = 1.0f;
    std::vector<Particle> particles;
    particles.reserve(N_particles);

    // Random seed
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Keep particle coordinates between [radius, WINDOW_WIDTH-radius]
    //std::uniform_real_distribution<float> distPosX(radius, WINDOW_WIDTH - radius);
    //std::uniform_real_distribution<float> distPosY(radius, WINDOW_HEIGHT - radius);
    
    float w = 5;

    std::uniform_real_distribution<float> distPosX(WINDOW_WIDTH/2 - w, WINDOW_WIDTH/2 + w);
    std::uniform_real_distribution<float> distPosY( 10 * WINDOW_HEIGHT/ 100,  20 * WINDOW_HEIGHT / 100);
    
    // Velocity distribution
    std::uniform_real_distribution<float> distVel(-50.0f, 50.0f);

    // Create particles with random positions and velocities.
    for (size_t i = 0; i < N_particles; i++)
    {
        sf::Vector2f pos(distPosX(gen), distPosY(gen));
        sf::Vector2f vel(distVel(gen), distVel(gen));
        
        particles.emplace_back(pos, vel, radius);
    }

    
    Ensemble ensemble(particles);
    
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear();

        ensemble.set_acceleration(GRAVITY);
        ensemble.border_collision();
        ensemble.collideParticles();
        ensemble.update();
        ensemble.draw(window);

        window.display();
    }
    
    return 0;
}
