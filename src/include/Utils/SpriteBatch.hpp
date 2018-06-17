#ifndef SPRITEBATCH_HPP
#define SPRITEBATCH_HPP

#include "SFML/Graphics.hpp"

class SpriteBatch
{
public:
    SpriteBatch();
    void reset();

    void draw(const sf::Sprite &sprite_);
    void draw(const sf::Vector2f &position_, const sf::IntRect &rec_, const sf::Vector2f &origin_,
              sf::Vector2f scale_, const sf::Color &color_, float rotation_);
    void draw(const std::vector<sf::Vertex> &vertVec_);
    void render(sf::RenderTarget &target_);
    const std::vector<sf::Vertex> &getVertices() const;
private:
    std::vector<sf::Vertex> _vertices;
};



#endif
