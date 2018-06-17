#include "Utils/VertexSprite.hpp"
#include "Utils/Helper.hpp"


VertexSprite::VertexSprite(const sf::Vector2f size_):
    _vertices(4)
{
    _vertices[0].position = sf::Vector2f(0.f, 0.f);
    _vertices[1].position = sf::Vector2f(size_.x, 0.f);
    _vertices[2].position = size_;
    _vertices[3].position = sf::Vector2f(0.f,size_.y);
}

VertexSprite::VertexSprite(const sf::Vector2f size_, const sf::IntRect textRect_):
    VertexSprite(size_)
{
    setTextureRect(textRect_);
}

void VertexSprite::move(const sf::Vector2f offset_)
{
    for(auto &v: _vertices)
    {
        v.position += offset_;
    }
}

void VertexSprite::rotateRight(const sf::Vector2f &around_)
{
    for(auto &v: _vertices)
    {
        v.position = pointRotateRight(v.position, around_);
    }
}

void VertexSprite::rotateLeft(const sf::Vector2f &around_)
{
    for(auto &v: _vertices)
    {
        v.position = pointRotateLeft(v.position, around_);
    }
}

void VertexSprite::scale(const sf::Vector2f &factor_)
{
    sf::Vector2f scaleX = _vertices[1].position - _vertices[0].position;
    sf::Vector2f scaleY = _vertices[3].position - _vertices[0].position;
    //const float scaleFactorX = factor_.x * vectorLength(scaleX) / vectorLength(scaleX);
    //const float scaleFactorY = factor_.y * vectorLength(scaleY) / vectorLength(scaleY);

    scaleX *= factor_.x;//scaleFactorX;
    scaleY *= factor_.y;//scaleFactorY;

    _vertices[1].position = _vertices[0].position + scaleX;
    _vertices[2].position = _vertices[0].position + scaleX + scaleY;
    _vertices[3].position = _vertices[0].position + scaleY;
}

void VertexSprite::setTextureRect(const sf::IntRect &textRect_)
{
    _vertices[0].texCoords = sf::Vector2f(textRect_.left,textRect_.top);
    _vertices[1].texCoords = sf::Vector2f(textRect_.left + textRect_.width,textRect_.top);
    _vertices[2].texCoords = sf::Vector2f(textRect_.left + textRect_.width,textRect_.top + textRect_.height);
    _vertices[3].texCoords = sf::Vector2f(textRect_.left,textRect_.top + textRect_.height);
}

const std::vector<sf::Vertex> &VertexSprite::getVertices() const
{
    return _vertices;
}
