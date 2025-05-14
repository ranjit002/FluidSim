#pragma once
#include <SFML/Graphics.hpp>
#include "Particle.hpp"
#include "globals.hpp"
#include <iostream>
#include <execution>


struct Ensemble
{

    std::vector<Particle> particles;
    
    public:
    
    Ensemble(std::vector<Particle>& particles_)
    : particles{particles_}
    {}

    void set_acceleration(sf::Vector2f acceleration)
    {
        for (auto& particle: particles)
        {
            particle.set_acceleration(acceleration);
        }
    }

    void draw(sf::RenderWindow& window)
    {
        for (auto& particle: particles)
        {
            particle.draw(window);
        }
    }

    void update()
    {
        for (auto& particle: particles)
        {
            particle.update(DT);
        }
    }

    void border_collision() 
    {
        for (auto& particle : particles) 
        {
            auto& position = particle.get_position();
            const float radius = particle.radius;
            auto& velocity = particle.get_velocity();

    
            // X axis
            if (position.x < radius)
            {
                position.x = radius;
                velocity.x = -velocity.x * (1.f - COLLISION_DAMPING);
            }
            else if (position.x > WINDOW_WIDTH - radius)
            {
            position.x = WINDOW_WIDTH - radius;
            velocity.x = -velocity.x * (1.f - COLLISION_DAMPING);
            }
            

            // Y axis
            if (position.y < radius)
            {
                position.y = radius;
                velocity.y = -velocity.y * (1.f - COLLISION_DAMPING);
            }
            else if (position.y > WINDOW_HEIGHT - radius)
            {
                position.y = WINDOW_HEIGHT - radius;
                velocity.y = -velocity.y * (1.f - COLLISION_DAMPING);
            }
        }
    }
    
    void handleCollision(Particle& a, Particle& b) 
    {
        sf::Vector2f separation = b.position - a.position;
        float dist = separation.length();
        float minDist = a.radius + b.radius;
        float tolerance = 0.1;

        bool collided = dist > 0.f && dist < minDist + tolerance;

        if (collided)
        {
            sf::Vector2f normal = separation / dist;
    
            sf::Vector2f relVel = b.velocity - a.velocity;
            float velAlongNormal = relVel.dot(normal);
    
            // Skip if they’re moving apart
            if (velAlongNormal > 0.f) return;
    
            // Compute impulse scalar (perfectly elastic)
            float impulseMag = (2.f * velAlongNormal) / (a.mass + b.mass);
            sf::Vector2f impulse = normal * impulseMag;
    
            a.velocity += impulse * (1.f / a.mass);
            b.velocity -= impulse * (1.f / b.mass);
    
            // Positional correction: push them to just be touching
            sf::Vector2f correction = normal * (minDist - dist) * 0.5f;
            a.position -= correction;
            b.position += correction;
        }
    }

    void collideParticles() 
    {
        for (size_t i = 0; i < particles.size(); ++i)
        {        
            for (size_t j = i + 1; j < particles.size(); ++j)
            {
                handleCollision(particles[i], particles[j]);
            }  
        }
    }
    
};