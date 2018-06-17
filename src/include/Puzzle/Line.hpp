#ifndef LINE_HPP
#define LINE_HPP

#include "SFML/Graphics.hpp"
#include "Utils/SpriteBatch.hpp"

class Line
{
public:
    Line(sf::Vector2f start_, sf::Vector2f end_, sf::Color color_, float thickness_ = 1.0f);

    void render(SpriteBatch &spBatch_);
    void render(sf::RenderTarget &target_);
    void addVertices(std::vector<sf::Vertex> &vertexVec_) const;

private:
    const sf::Color _color;
    sf::Vector2f _start;
    sf::Vector2f _end;
    const float _thickness;
};

#endif
