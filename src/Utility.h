// Utility.h
#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>

inline float length(const sf::Vector2f& v)
{
    return std::sqrt(v.x * v.x + v.y * v.y);
}

inline float dot(const sf::Vector2f& a, const sf::Vector2f& b)
{
    return a.x * b.x + a.y * b.y;
}

inline sf::Vector2f normalize(const sf::Vector2f& v)
{
    float len = length(v);
    return len > 0.f ? sf::Vector2f(v.x / len, v.y / len)
                     : sf::Vector2f(0.f, 0.f);
}
