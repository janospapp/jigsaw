#include "Utils/SpriteBatch.hpp"
#include "Managers/PuzzleManager.hpp"
#include "Managers/ImageManager.hpp"


#define M_PI 3.14159265358979323846
// This is 1/(180*Pi)
#define MULTFACTOR 0.01745329251994329576922222222222

SpriteBatch::SpriteBatch()
{
}

void SpriteBatch::reset()
{
    _vertices.clear();
    _vertices.reserve(PuzzleManager::getPieceNumber()*5*4);
}

void SpriteBatch::draw(const sf::Sprite &sprite_)
{
    draw(sprite_.getPosition(), sprite_.getTextureRect(), sprite_.getOrigin(), sprite_.getScale(), sprite_.getColor(), sprite_.getRotation());
}

void SpriteBatch::draw(const sf::Vector2f &position_, const sf::IntRect &rec_, const sf::Vector2f &origin_,
                       sf::Vector2f scale_, const sf::Color &color_, float rotation_)
{
    float sin=0, cos=1;

    if(rotation_ != 0) {
        rotation_ *= 0.0174532925199432957692f;
        sin = std::sin(rotation_);
        cos = std::cos(rotation_);
    }

    float pX = -origin_.x * scale_.x;
    float pY = -origin_.y * scale_.y;

    scale_.x *= rec_.width;
    scale_.y *= rec_.height;

    sf::Vertex v;

    v.position.x = pX * cos - pY * sin + position_.x;
    v.position.y = pX * sin + pY * cos + position_.y;
    v.texCoords.x = (float)rec_.left;
    v.texCoords.y = (float)rec_.top;
    v.color = color_;
    _vertices.push_back(v);

    pX += scale_.x;
    //v.position.x = pX * cos - pY * sin + position_.x;
    v.position.x += scale_.x * cos;
    //v.position.y = pX * sin + pY * cos + position_.y;
    v.position.y += scale_.x * sin;
    //v.texCoords.x = (float)(rec_.left + rec_.width);
    v.texCoords.x += rec_.width;
    //v.texCoords.y = (float)rec_.top;
    //v.color = color_;
    _vertices.push_back(v);

    pY += scale_.y;
    //v.position.x = pX * cos - pY * sin + position_.x;
    v.position.x -= scale_.y * sin;
    //v.position.y = pX * sin + pY * cos + position_.y;
    v.position.y += scale_.y * cos;
    //v.texCoords.x = (float)(rec_.left + rec_.width);
    //v.texCoords.y = (float)(rec_.top + rec_.height);
    v.texCoords.y += rec_.height;
    //v.color = color_;
    _vertices.push_back(v);

    pX -= scale_.x;
    //v.position.x = pX * cos - pY * sin + position_.x;
    v.position.x -= scale_.x * cos;
    //v.position.y = pX * sin + pY * cos + position_.y;
    v.position.y -= scale_.x * sin;
    //v.texCoords.x = (float)rec_.left;
    v.texCoords.x -= rec_.width;
    //v.texCoords.y = (float)(rec_.top + rec_.height);
    //v.color = color_;
    _vertices.push_back(v);
}

void SpriteBatch::draw(const std::vector<sf::Vertex> &vertVec_)
{
    _vertices.insert(_vertices.end(),vertVec_.begin(),vertVec_.end());
}

void SpriteBatch::render(sf::RenderTarget &target_)
{
    sf::RenderStates state = sf::RenderStates::Default;
    state.texture = ImageManager::getTexture("puzzleTexture");
    target_.draw(&_vertices[0], _vertices.size(), sf::Quads, state);
    _vertices.clear();
}

const std::vector<sf::Vertex> &SpriteBatch::getVertices() const
{
    return _vertices;
}
