#ifndef VERTEXSPRITE_HPP
#define VERTEXSPRITE_HPP

#include "SFML/Graphics.hpp"
#include <vector>

class VertexSprite
{
public:
    VertexSprite(const sf::Vector2f size_);
    VertexSprite(const sf::Vector2f size_, const sf::IntRect textRect_);
    void move(const sf::Vector2f offset_);
    void rotate(const float angle_, const sf::Vector2f &around_);
    void rotateRight(const sf::Vector2f &around_);
    void rotateLeft(const sf::Vector2f &around_);
    void scale(const sf::Vector2f &factor_);
    void setTextureRect(const sf::IntRect &textRect_);
    const std::vector<sf::Vertex>& getVertices() const;

private:
    std::vector<sf::Vertex> _vertices;
};





#endif
