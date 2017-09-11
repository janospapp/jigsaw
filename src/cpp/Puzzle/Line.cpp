#include "Utils/Helper.hpp"
#include "Puzzle/Line.hpp"

Line::Line(sf::Vector2f start_, sf::Vector2f end_, sf::Color color_, float thickness_):
    _start(start_),
    _end(end_),
    _color(color_),
    _thickness(thickness_)
{
}


void Line::addVertices(std::vector<sf::Vertex> &vertexVec_) const
{
    sf::Vector2f angular = _end - _start;
    // Normalize the vector.
    angular /= vectorDist(_end, _start);
    sf::Vector2f perpendicular(angular.y, -angular.x);
    perpendicular *= _thickness/2.0f;

    vertexVec_.push_back(sf::Vertex(_start + perpendicular,_color));
    vertexVec_.push_back(sf::Vertex(_end + perpendicular,_color));
    vertexVec_.push_back(sf::Vertex(_end - perpendicular,_color));
    vertexVec_.push_back(sf::Vertex(_start - perpendicular,_color));
}


void Line::render(sf::RenderTarget &screen_)
{
    sf::Vertex points[4];
    sf::Vector2f angular = _end - _start;
    // Normalize the vector.
    angular /= vectorDist(_end, _start);
    sf::Vector2f perpendicular(angular.y, -angular.x);
    perpendicular *= _thickness/2.0f;

    points[0] = sf::Vertex(_start + perpendicular,_color);
    points[1] = sf::Vertex(_end + perpendicular,_color);
    points[2] = sf::Vertex(_end - perpendicular,_color);
    points[3] = sf::Vertex(_start - perpendicular,_color);

    screen_.draw(points,4,sf::PrimitiveType::Quads);
}
