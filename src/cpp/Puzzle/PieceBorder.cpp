#include "Puzzle/PieceBorder.hpp"
#include <utility>

PieceBorder::PieceBorder():
    _edges(4),_currentColor(sf::Color::Transparent)
{
}


PieceBorder::PieceBorder(const std::vector<Edge*> &edges_, const sf::IntRect &textureRect_, const sf::Vector2f &spritePos_):
    _edges(4),_currentColor(sf::Color::Transparent),_textureRect(textureRect_)
{
	for(int i=0; i<4; ++i)
	{
        _edges[i]=std::unique_ptr<Edge>(edges_[i]);
	}
    _borderSprite.setPosition(spritePos_);
}


PieceBorder::PieceBorder(PieceBorder &&rhs_):
    _edges(std::move(rhs_._edges)),_textureRect(std::move(rhs_._textureRect)),
    _currentColor(std::move(rhs_._currentColor)),_borderSprite(rhs_._borderSprite)
{
}


PieceBorder& PieceBorder::operator=(PieceBorder &&rhs_)
{
    if(this != &rhs_)
	{
		_edges.clear();
        _edges=std::move(rhs_._edges);
        _currentColor = std::move(rhs_._currentColor);
        _textureRect = std::move(rhs_._textureRect);
        _borderSprite = std::move(rhs_._borderSprite);
    }
	
	return *this;
}


void PieceBorder::move(const sf::Vector2f &offset_)
{
	for(auto &edge: _edges)
	{
        edge->move(offset_);
	}
    _borderSprite.move(offset_);
}


void PieceBorder::rotateRight(const sf::Vector2f &around_)
{
	for(auto &edge: _edges)
	{
        edge->rotateRight(around_);
	}
}


bool PieceBorder::isInside(const sf::Vector2f &pt_) const
{
	bool inside=true;
	for(auto &edge: _edges)
	{
        bool isOk=edge->isInside(pt_);	//This is only for debug purpose, cut it out from the working code (put it inside the if check)
        if(!isOk)
		{
			inside=false;
			break;
		}
	}

	return inside;
}


void PieceBorder::render(sf::RenderTarget &screen_, const sf::Color &color_) const
{
    if(_currentColor != color_)
    {
        auto it = std::find_if(_borderTextures.begin(),_borderTextures.end(),
                               [color_](std::pair<sf::Color,sf::Texture *> &elem){return elem.first==color_;});
        if(it != _borderTextures.end())
        {
            const_cast<PieceBorder *>(this)->_currentColor = color_;
            const_cast<PieceBorder *>(this)->_borderSprite.setTexture(*(it->second));
            const_cast<PieceBorder *>(this)->_borderSprite.setTextureRect(_textureRect);
        }
    }

    if(_currentColor != sf::Color::Transparent)
    {
        screen_.draw(_borderSprite);
    }
}


void PieceBorder::setColorTextures(const std::vector<std::pair<sf::Color, sf::Texture *> > &&textures_)
{
    _borderTextures = textures_;
}


void PieceBorder::clearColorTextures()
{
    _borderTextures.clear();
}

std::vector<std::pair<sf::Color,sf::Texture *>> PieceBorder::_borderTextures;
