#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>

// Euclidean norm (length) of a 2D vector
float length(const sf::Vector2f& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

class Particle 
{
public:
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    sf::Color color{64, 164, 223};
    float radius;
    float mass;
    
    Particle(sf::Vector2f position_, float radius_)
        : position{position_}
        , velocity{0.0f, 0.0f}
        , acceleration{0.0f, 0.0f}
        , radius{radius_}
        , mass{1.0}
        {}
        
        Particle(sf::Vector2f position_,  sf::Vector2f velocity_, float radius_)
        : position{position_}
        , velocity{velocity_}
        , acceleration{0.0f, 0.0f}
        , radius{radius_}
        , mass{1.0}
{}


    // Verlet integration
    void update(float dt) 
    {
        position += velocity * dt + 0.5f * acceleration * (dt * dt);
        velocity += 0.5f * acceleration * dt;
        acceleration = {};
    }

    void draw(sf::RenderWindow& window)
    {
        sf::CircleShape shape(radius);
        shape.setFillColor(color);
        shape.setPosition(position);
        window.draw(shape);
    }


    void set_position(sf::Vector2f position_)
    {
        position = position_;
    }

    sf::Vector2f& get_position()
    {
        return position;
    }

    void set_velocity(sf::Vector2f v)
    {
        velocity = v;
    }
    
    sf::Vector2f& get_velocity()
    {
        return velocity;
    }

    void add_velocity(sf::Vector2f v)
    {
        velocity += v;
    }

    void set_acceleration(sf::Vector2f a)
    {
        acceleration = a;
    }
    
    void add_acceleration(sf::Vector2f a)
    {
        acceleration += a;
    }
};
