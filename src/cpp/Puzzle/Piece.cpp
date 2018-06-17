#include "Puzzle/Piece.hpp"
#include "GameCore/GameApp.hpp"
#include "Managers/WindowManager.hpp"
#include "Managers/PuzzleManager.hpp"
#include "Utils/Debug.hpp"
#include "Utils/Helper.hpp"
#include <utility>

Piece::Piece(const U16 x_, const U16 y_, const U16 linkID_, const sf::Sprite &sprite_, PieceBorder &&border_, const U8 dir_, const std::vector<U8> &freeSides_):
    _sprite(sprite_),
    _border(std::move(border_)),
    _freeSides(freeSides_),
    _iPosInPuzzleX(x_),
    _iPosInPuzzleY(y_),
    _iLinkId(linkID_),
    _direction(dir_),
    _selected(false)
{
}


U16 Piece::getLinkId() const
{
	return _iLinkId;
}


void Piece::setLinkId(const U16 id_)
{
    _iLinkId = id_;
}


const sf::Vector2f& Piece::getCenter() const
{
	return _sprite.getPosition();
}


void Piece::setPosition(const sf::Vector2f &pos_)
{
	sf::Vector2f beforePos=_sprite.getPosition();
    sf::Vector2f offset=pos_-beforePos;

    _sprite.move(offset);
	_border.move(offset);
}


sf::Vector2i Piece::getPosInPuzzle() const
{
	return sf::Vector2i(_iPosInPuzzleX,_iPosInPuzzleY);
}


U8 Piece::getNumFreeSides() const
{
    return _freeSides.size();
}


void Piece::move(const sf::Vector2f &offset_)
{
    _sprite.move(offset_);
    _border.move(offset_);
}


void Piece::rotateRight(const sf::Vector2f &around_)
{
	//TO DO: Rotate the sprite too!!
    _border.rotateRight(around_);
	_direction=(++_direction)%4;
    _sprite.rotate(90.f);
    const sf::Vector2f newPos = pointRotateRight(_sprite.getPosition(),around_);
    _sprite.setPosition(newPos);
}


bool Piece::isPointInside(const sf::Vector2f &p_) const
{
    if(_sprite.getGlobalBounds().contains(p_))
    {
        return _border.isInside(p_);
    }
    else
    {   // Lazy check, if it's outside the bounding square we don't have to check the border.
        return false;
    }
}


void Piece::decreaseFreeSides(const U8 dir_)
{
    auto pos = std::find(_freeSides.cbegin(), _freeSides.cend(), dir_);
    if(pos != _freeSides.cend())
    {
        _freeSides.erase(pos);
    }
}


bool Piece::checkMatch(const Piece *rhs_) const
{
    if(_freeSides.size() > 0 && rhs_->_freeSides.size() > 0 && _iLinkId != rhs_->_iLinkId &&
            gridDist(this->getPosInPuzzle(),rhs_->getPosInPuzzle())==1 && _direction == rhs_->_direction)
    {
        float maxDist=15.f;
        bool checkHorizontalMatch;
        const sf::Vector2f thisPos = this->getCenter();
        const sf::Vector2f otherPos = rhs_->getCenter();

        if(std::fabs(thisPos.x-otherPos.x) <= maxDist && std::fabs(thisPos.y-otherPos.y) <= maxDist + PuzzleManager::_pieceSize)
        {
            checkHorizontalMatch = true;
        }
        else if(std::fabs(thisPos.y-otherPos.y) <= maxDist && std::fabs(thisPos.x-otherPos.x) <= maxDist + PuzzleManager::_pieceSize)
        {
            checkHorizontalMatch = false;
        }
        else
        {
            // They are too far from each other.
            return false;
        }

        const Piece *p1,*p2;
        p1 = checkHorizontalMatch ? (thisPos.y < otherPos.y ? this : rhs_) : (thisPos.x < otherPos.x ? this : rhs_);
        p2 = (p1 == this) ? rhs_ : this;

        float c1 = 0.0, c2 = 0.0;
        bool goodPosition = false;

        switch(_direction)
        {
        case 0:
            if(checkHorizontalMatch && p1->_iPosInPuzzleY < p2->_iPosInPuzzleY)
            {
                // Position: P1
                //           P2
                goodPosition = true;
                c1 = p1->_border.getEdge(2)->getStartPoint().y; // bottom border - bottom side
                c2 = p2->_border.getEdge(0)->getStartPoint().y; // top border - up side
            }
            if(!checkHorizontalMatch && p1->_iPosInPuzzleX < p2->_iPosInPuzzleX)
            {
                // Position: P1 P2
                goodPosition = true;
                c1 = p1->_border.getEdge(1)->getStartPoint().x; // right border - right side
                c2 = p2->_border.getEdge(3)->getStartPoint().x; // left border - left side
            }
            break;
        case 1:
            // They are facing right ->
            if(checkHorizontalMatch && p1->_iPosInPuzzleX < p2->_iPosInPuzzleX)
            {
                goodPosition = true;
                c1 = p1->_border.getEdge(1)->getStartPoint().y; // right border - bottom side
                c2 = p2->_border.getEdge(3)->getStartPoint().y; // left border - up side
            }
            if(!checkHorizontalMatch && p1->_iPosInPuzzleY > p2->_iPosInPuzzleY)
            {
                goodPosition = true;
                c1 = p1->_border.getEdge(0)->getStartPoint().x; // top border - right side
                c2 = p2->_border.getEdge(2)->getStartPoint().x; // bottom border - left side
            }
            break;
        case 2:
            // They are facing down
            if(checkHorizontalMatch && p1->_iPosInPuzzleY > p2->_iPosInPuzzleY)
            {
                goodPosition = true;
                c1 = p1->_border.getEdge(0)->getStartPoint().y; // top border - bottom side
                c2 = p2->_border.getEdge(2)->getStartPoint().y; // bottom border - up side
            }
            if(!checkHorizontalMatch && p1->_iPosInPuzzleX > p2->_iPosInPuzzleX)
            {
                goodPosition = true;
                c1 = p1->_border.getEdge(3)->getStartPoint().x; // left border - right side
                c2 = p2->_border.getEdge(1)->getStartPoint().x; // right border - left side
            }
            break;
        case 3:
            // They are facing left
            if(checkHorizontalMatch && p1->_iPosInPuzzleX > p2->_iPosInPuzzleX)
            {
                goodPosition = true;
                c1 = p1->_border.getEdge(3)->getStartPoint().y; // left border - bottom side
                c2 = p2->_border.getEdge(1)->getStartPoint().y; // right border - up side
            }
            if(!checkHorizontalMatch && p1->_iPosInPuzzleY < p2->_iPosInPuzzleY)
            {
                goodPosition = true;
                c1 = p1->_border.getEdge(2)->getStartPoint().x; // bottom border - right side
                c2 = p2->_border.getEdge(0)->getStartPoint().x; // up border - left side
            }
            break;
        }

        return goodPosition && std::fabs(c1-c2) <= maxDist;
    }
    else
    {
        return false;
    }
}


sf::Vector2f Piece::getOffsetToMatch(const Piece *rhs_) const
{
    U8 side=getRelativeDirection(rhs_);
    sf::Vector2f offset=rhs_->_sprite.getPosition()-_sprite.getPosition();
    U16 size = PuzzleManager::_pieceSize;
    switch(side)
    {
    case 0:
        offset.y += size;
        break;
    case 1:
        offset.x -= size;
        break;
    case 2:
        offset.y -= size;
        break;
    case 3:
        offset.x += size;
        break;
    }

    return offset;
}


void Piece::render(SpriteBatch &spBatch_,const BorderRenderingMode &renderMode_, const sf::Color &color_) const
{
    spBatch_.draw(_sprite);

    switch(renderMode_)
    {
    case eAll:
        _border.render(spBatch_, color_);
        break;
    case eOnlyFree:
        _border.render(spBatch_, color_, _freeSides);
        break;
    default:
        break;
    }
}

const sf::FloatRect Piece::getBoundRect() const
{
    sf::FloatRect bound = _sprite.getGlobalBounds();
    return bound;
}

void Piece::writeOut(std::ostream &os_) const
{
    _border.writeOut(os_);
    write(os_,_freeSides);
    write<const float>(os_,_sprite.getPosition().x);
    write<const float>(os_,_sprite.getPosition().y);
    write<const U16>(os_,_iPosInPuzzleX);
    write<const U16>(os_,_iPosInPuzzleY);
    write<const U16>(os_,_iLinkId);
    write<const U8>(os_,_direction);
}


PieceData Piece::readIn(std::istream &is_)
{
    PieceData ret;

    ret._border = std::move(PieceBorder::readIn(is_));
    read(is_,ret._freeSides);
    read<float>(is_,&ret._position.x);
    read<float>(is_,&ret._position.y);
    read<U16>(is_,&ret._puzzlePosX);
    read<U16>(is_,&ret._puzzlePosY);
    read<U16>(is_,&ret._linkId);
    read<U8>(is_,&ret._direction);

    return ret;
}


U8 Piece::getRelativeDirection(const Piece *rhs_, const bool respectRotation_) const
{
    U8 side;
    if(_iPosInPuzzleY > rhs_->_iPosInPuzzleY)
    {
        side=0;
    }
    else if(_iPosInPuzzleY == rhs_->_iPosInPuzzleY)
    {
        if(_iPosInPuzzleX < rhs_->_iPosInPuzzleX)
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

    if(respectRotation_)
    {
        return (side+_direction)%4;
    }
    else
    {
        return side;
    }
}


void Piece::debugRender(sf::RenderWindow &screen) const
{
    sf::FloatRect outLine = getBoundRect();
    sf::Vector2f p(outLine.left, outLine.top);

    static sf::Font font;
    font.loadFromFile("Fonts/arial.ttf");
    std::string coord("(");
    coord += floatToStr(outLine.width)+","+floatToStr(outLine.height)+")";
    sf::Text pos(coord,font,36);
    pos.setPosition(sf::Vector2f(p.x+outLine.width/2+100.f,p.y+outLine.height/2+100.f));
    pos.setColor(sf::Color::White);
    screen.draw(pos);
}
