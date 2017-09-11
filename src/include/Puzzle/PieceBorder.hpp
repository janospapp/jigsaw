#ifndef PIECEBORDER_HPP
#define PIECEBORDER_HPP

#include "Puzzle/Edge.hpp"
#include "SFML/Graphics.hpp"
#include <vector>
#include <memory>

class PieceBorder
{
public:
	PieceBorder();
    PieceBorder(const std::vector<Edge*> &edges_,const sf::IntRect &textureRect_,const sf::Vector2f &spritePos_);
    PieceBorder(PieceBorder &&rhs_);
    PieceBorder& operator=(PieceBorder &&rhs_);

    void move(const sf::Vector2f &offset_);
    void rotateRight(const sf::Vector2f &around_);
    bool isInside(const sf::Vector2f &pt_) const;

    void render(sf::RenderTarget &screen_, const sf::Color &color_) const;

    static void setColorTextures(const std::vector<std::pair<sf::Color,sf::Texture *>> &&textures_);
    static void clearColorTextures();
private:
	std::vector<std::unique_ptr<Edge>> _edges;
    static std::vector<std::pair<sf::Color,sf::Texture *>> _borderTextures;
    sf::IntRect _textureRect;
    sf::Sprite _borderSprite;
    sf::Color _currentColor;
};

#endif
