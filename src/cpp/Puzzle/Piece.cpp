#include "Puzzle/Piece.hpp"
#include "GameCore/GameApp.hpp"
#include "Managers/WindowManager.hpp"
#include "Utils/Debug.hpp"
#include "Utils/Helper.hpp"
#include <utility>

Piece::Piece(const U16 x, const U16 y, const U16 size, const U16 linkID, const sf::Sprite &sprite, PieceBorder &&border, const U8 dir):
	_sprite(sprite),
	_border(std::move(border)),
	_iPosInPuzzleX(x),
	_iPosInPuzzleY(y),
    _size(size),
	_iLinkId(linkID),
	_iFreeSides(4),
    _direction(dir),
    _selected(false)
{
}


U16 Piece::getLinkId() const
{
	return _iLinkId;
}


void Piece::setLinkId(const U16 id)
{
	_iLinkId = id;
}


const sf::Vector2f& Piece::getCenter() const
{
	return _sprite.getPosition();
}


const sf::FloatRect Piece::getBound() const
{
	return _sprite.getGlobalBounds();
}


void Piece::setPosition(const sf::Vector2f &pos)
{
	sf::Vector2f beforePos=_sprite.getPosition();
	sf::Vector2f offset=pos-beforePos;

    _sprite.move(offset);
	_border.move(offset);
}


sf::Vector2i Piece::getPosInPuzzle() const
{
	return sf::Vector2i(_iPosInPuzzleX,_iPosInPuzzleY);
}


U8 Piece::getFreeSides() const
{
	return _iFreeSides;
}


void Piece::move(const sf::Vector2f &offset)
{
	_sprite.move(offset);
	_border.move(offset);
}


void Piece::rotateRight(const sf::Vector2f &around)
{
	//TO DO: Rotate the sprite too!!
	_border.rotateRight(around);
	_direction=(++_direction)%4;
}


bool Piece::isPointInside(const sf::Vector2f &p) const
{
    if(_sprite.getGlobalBounds().contains(p))
    {
        return _border.isInside(p);
    }
    else
    {   // Lazy check, if it's outside the bounding square we don't have to check the border.
        return false;
    }
}


void Piece::decreaseFreeSides()
{
#ifdef _DEBUG
     if(_iFreeSides == 0)
	 {
     	std::string dbg("You want to decrease the number of freeside of a piece, which has already got all neighbours!");
        debugPrint(std::move(dbg));
	 }
#endif
	_iFreeSides--;
}


bool Piece::checkMatch(const Piece *rhs) const
{
    if(_iFreeSides > 0 && rhs->_iFreeSides > 0 && _iLinkId != rhs->_iLinkId &&
            gridDist(this->getPosInPuzzle(),rhs->getPosInPuzzle())==1 && _direction == rhs->_direction)
    {
        U8 side=getRelativeDirection(rhs);

        sf::FloatRect thisBound=_sprite.getGlobalBounds();
        sf::FloatRect rhsBound=rhs->_sprite.getGlobalBounds();

        //The distance of the piece and the sprite border.
        float pieceSpriteBorderDist=(thisBound.height-_size)/2.f;
        float maxDist=15.f;
        float thisHorizontalBorder,rhsHorizontalBorder,thisVerticalBorder,rhsVerticalBorder;

        //Later consider the pieces' direction too!!
        switch(side){
        case 0:
            thisHorizontalBorder=thisBound.top+pieceSpriteBorderDist;
            rhsHorizontalBorder=rhsBound.top+rhsBound.height-pieceSpriteBorderDist;

            //It doesn't matter in this case, if I check the piece or the sprite border.
            thisVerticalBorder=thisBound.left;
            rhsVerticalBorder=rhsBound.left;
            break;
        case 1:
            thisVerticalBorder=thisBound.left+thisBound.width-pieceSpriteBorderDist;
            rhsVerticalBorder=rhsBound.left+pieceSpriteBorderDist;

            thisHorizontalBorder=thisBound.top;
            rhsHorizontalBorder=rhsBound.top;
            break;
        case 2:
            thisHorizontalBorder=thisBound.top+thisBound.height-pieceSpriteBorderDist;
            rhsHorizontalBorder=rhsBound.top+pieceSpriteBorderDist;

            thisVerticalBorder=thisBound.left;
            rhsVerticalBorder=rhsBound.left;
            break;
        case 3:
            thisVerticalBorder=thisBound.left+pieceSpriteBorderDist;
            rhsVerticalBorder=rhsBound.left+rhsBound.width-pieceSpriteBorderDist;

            thisHorizontalBorder=thisBound.top;
            rhsHorizontalBorder=rhsBound.top;
            break;
        }

        if(std::abs(thisHorizontalBorder-rhsHorizontalBorder) <= maxDist &&
                std::abs(thisVerticalBorder-rhsVerticalBorder) <= maxDist)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}


sf::Vector2f Piece::getOffsetToMatch(const Piece *rhs) const
{
    U8 side=getRelativeDirection(rhs);
    sf::Vector2f offset=rhs->_sprite.getPosition()-_sprite.getPosition();
    switch(side)
    {
    case 0:
        offset.y += _size;
        break;
    case 1:
        offset.x -= _size;
        break;
    case 2:
        offset.y -= _size;
        break;
    case 3:
        offset.x += _size;
        break;
    }

    return offset;
}


void Piece::render(sf::RenderWindow &screen_, const bool drawBorder_, const sf::Color &color_) const
{
    float zoomFactor=WindowManager::getZoomFactor();
    sf::Vector2f size(GameApp::Options.iScreenWidth*zoomFactor, GameApp::Options.iScreenHeight*zoomFactor);
    if(_sprite.getGlobalBounds().intersects(sf::FloatRect(screen_.mapPixelToCoords(sf::Vector2i(0,0)),size)))
	{
        screen_.draw(_sprite);

        if(drawBorder_)
        {
                _border.render(screen_,color_);
        }
//        debugRender(screen);
	}
}


U8 Piece::getRelativeDirection(const Piece *rhs) const
{
    U8 side;
    if(_iPosInPuzzleY > rhs->_iPosInPuzzleY)
    {
        side=0;
    }
    else if(_iPosInPuzzleY == rhs->_iPosInPuzzleY)
    {
        if(_iPosInPuzzleX < rhs->_iPosInPuzzleX)
        {
            side=1;
        }
        else
        {
            side=3;
        }
    }
    else
    {
        side=2;
    }

    return side;
}


void Piece::debugRender(sf::RenderWindow &screen) const
{
    sf::FloatRect outLine=_sprite.getGlobalBounds();
    sf::RectangleShape rect(sf::Vector2f(outLine.width,outLine.height));
    rect.setFillColor(sf::Color::Transparent);
    rect.setOutlineColor(sf::Color::Green);
    rect.setOutlineThickness(3.f);
    sf::Vector2f p=_sprite.getPosition();
    rect.setPosition(p);
    screen.draw(rect);

    static sf::Font font;
    font.loadFromFile("Font/arial.ttf");
    std::string coord("(");
    coord += floatToStr(p.x)+","+floatToStr(p.y)+")";
    sf::Text pos(coord,font,10);
    pos.setPosition(sf::Vector2f(p.x+outLine.width/2+100.f,p.y+outLine.height/2+100.f));
    pos.setColor(sf::Color::Red);
    screen.draw(pos);
}
