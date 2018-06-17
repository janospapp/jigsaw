#include "Puzzle/Edge.hpp"
#include "Puzzle/Line.hpp"
#include "Managers/PuzzleManager.hpp"
#include "Managers/ImageManager.hpp"
#include "Managers/WindowManager.hpp"
#include "Utils/Helper.hpp"
#include "Utils/Debug.hpp"

// Helper method for rotation
inline DirEnum getNextDir(const DirEnum dir_)
{
    if(dir_ == eUp)
        return eRight;
    if(dir_ == eRight)
        return eDown;
    if(dir_ == eDown)
        return eLeft;
    if(dir_ == eLeft)
        return eUp;
}

inline DirEnum getPrevDir(const DirEnum dir_)
{
    if(dir_ == eUp)
        return eLeft;
    if(dir_ == eRight)
        return eUp;
    if(dir_ == eDown)
        return eRight;
    if(dir_ == eLeft)
        return eDown;
}

inline DirEnum getOppositeDir(const DirEnum dir_)
{
    if(dir_ == eUp)
        return eDown;
    if(dir_ == eRight)
        return eLeft;
    if(dir_ == eDown)
        return eUp;
    if(dir_ == eLeft)
        return eRight;
}


//////////
// Edge //
//////////
/*
Edge::Edge():
	_isHorizontal(false),
    _inDirection(eRight)
{
}
*/

Edge::Edge(const bool isHorizontal_, const DirEnum inDirection_, const sf::Vector2f &scale_):
    _isHorizontal(isHorizontal_),
    _inDirection(inDirection_),
    _currentColor(sf::Color::Transparent),
    _sprite(sf::Vector2f(600.f,300.f))
{
    _sprite.scale(scale_);
    float dist = PuzzleManager::_pieceSize*0.25f;
    _sprite.move(sf::Vector2f(-dist,-dist));
}


Edge::Edge(const Edge &rhs_):
    _isHorizontal(rhs_._isHorizontal),
    _inDirection(rhs_._inDirection),
    _currentColor(rhs_._currentColor),
    _sprite(rhs_._sprite)
{
}

void Edge::move(const sf::Vector2f &offset_)
{
    _sprite.move(offset_);
}


void Edge::rotateRight(const sf::Vector2f &around_)
{
    _sprite.rotateRight(around_);
	_isHorizontal=!_isHorizontal;
    _inDirection = getNextDir(_inDirection);
}


void Edge::rotateLeft(const sf::Vector2f &around_)
{
    _sprite.rotateLeft(around_);
	_isHorizontal=!_isHorizontal;
    _inDirection = getPrevDir(_inDirection);
}


void Edge::flipInside()
{
    _inDirection = getOppositeDir(_inDirection);
}


void Edge::render(SpriteBatch &spBatch_, const sf::Color &color_) const
{
    if(_currentColor != color_)
    {
        _currentColor = color_;
        changeSprite();
    }
    spBatch_.draw(_sprite.getVertices());
}


void Edge::setPosition(const sf::Vector2f &pos_)
{   //Moves the edge to the new position
	sf::Vector2f currPos=getStartPoint();
    sf::Vector2f offset=pos_-currPos;
	move(offset);
}

void Edge::setTextureCoordOffset(const U16 coord_)
{
    _textureStartCoord = coord_;
}

U16 Edge::_textureStartCoord = 0;

///////////////
// CurvyEdge //
///////////////

CurvyEdge::CurvyEdge(const bool isHorizontal, const DirEnum insideDir_, const DirEnum faceDir_, std::vector<sf::Vector2f> &ctrPts_,
                     std::vector<signed char> &dirs_, const sf::Vector2f &scale_):
    Edge(isHorizontal,insideDir_,scale_),
    _faceDir(faceDir_),
    _curves(10)
{
	for(int i=0; i<10; ++i)
	{
        _curves[i]=Curve(ctrPts_[3*i],ctrPts_[3*i+1],ctrPts_[3*i+2],dirs_[i]);
    }
    changeSprite();
}


CurvyEdge::CurvyEdge(const CurvyEdge &rhs_):
    Edge(rhs_),
    _curves(rhs_._curves),
    _faceDir(rhs_._faceDir)
{
}


sf::Vector2f CurvyEdge::getStartPoint() const
{
    return _curves[0].b0;
}


sf::Vector2f CurvyEdge::getEndPoint() const
{
	return _curves[9].b2;
}


void CurvyEdge::move(const sf::Vector2f &offset_)
{
    Edge::move(offset_);
	for(auto &curve: _curves)
	{
        curve.move(offset_);
    }
}


void CurvyEdge::rotateRight(const sf::Vector2f &around_)
{
    Edge::rotateRight(around_);
	for(int i=0; i<10; ++i)
	{
        _curves[i].rotateRight(around_);
	}

	if(_isHorizontal)
	{	//It was a vertical edge before the rotation
        reverseEdge();
	}
    _faceDir = getNextDir(_faceDir);
}


void CurvyEdge::rotateLeft(const sf::Vector2f &around_)
{
    Edge::rotateLeft(around_);
	for(int i=0; i<10; ++i)
	{
        _curves[i].rotateLeft(around_);
	}

	if(!_isHorizontal)
	{	//It was a horizontal edge before the rotation
        reverseEdge();
	}
    _faceDir = getPrevDir(_faceDir);
}


void CurvyEdge::flipInside()
{
	Edge::flipInside();
	for(auto &curve: _curves)
	{
		curve.flipDirection();
	}
}


bool CurvyEdge::isInside(const sf::Vector2f &pt_) const
{
    I8 faceDir;
    if(_faceDir == eUp || _faceDir == eRight)
    {
        faceDir = 1;
    }
    else
    {
        faceDir = -1;
    }

    I8 inDirection;
    if(_inDirection == eUp || _inDirection == eRight)
    {
        inDirection = 1;
    }
    else
    {
        inDirection = -1;
    }


	if(_isHorizontal)
	{
		//Horizontal and vertical strip lines:
		float h1=_curves[5].b0.y;
		float h2=_curves[4].b0.y;
		float h3=_curves[3].b0.y;
		float h4=_curves[2].b0.y;
		float h5=_curves[1].b0.y;

		float v1=_curves[0].b0.x;
		float v2=_curves[1].b0.x;
		float v3=_curves[4].b0.x;
		float v4=_curves[3].b0.x;
		float v5=_curves[2].b0.x;
		float v6=_curves[5].b0.x;
		float v7=_curves[8].b0.x;
		float v8=_curves[7].b0.x;
		float v9=_curves[6].b0.x;
		float v10=_curves[9].b0.x;
		float v11=_curves[9].b2.x;

		U8 strip;
        if((pt_.y - h1)*faceDir <= 0)
		{
            return inDirection*faceDir > 0;
		}
        if((pt_.y - h5)*faceDir >= 0)
		{
            return inDirection*faceDir < 0;
		}

        if((pt_.y - h2)*faceDir < 0)
		{
			strip=1;
		}
        else if((pt_.y-h3)*faceDir < 0)
		{
			strip=2;
		}
        else if((pt_.y-h4)*faceDir < 0)
		{
			strip=3;
		}
		else
		{
			strip=4;
		}

		std::string dbg;
		switch(strip)
		{
			case 1:
                if(pt_.x <= v3)
				{
                    return inDirection*faceDir > 0;
				}
                if(pt_.x >= v9)
				{
                    return inDirection*faceDir > 0;
				}
                if(pt_.x < v6)
				{
                    return _curves[4].isPointInside(pt_);
				}
				else
				{
                    return _curves[5].isPointInside(pt_);
				}
				dbg=std::string("Point inside edge check: Unhandled (Horizontal) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
			case 2:
                if(pt_.x <= v3)
				{
                    return inDirection*faceDir > 0;
				}
                if(pt_.x >= v9)
				{
                    return inDirection*faceDir > 0;
				}
                if(pt_.x < v4)
				{	//Point is between v3-v4
                    return _curves[3].isPointInside(pt_);
				}
                else if(pt_.x < v8)
				{	//Between v4-v8
                    return inDirection*faceDir < 0;
				}
				else
				{	//Between v8-v9
                    return _curves[6].isPointInside(pt_);
				}
				dbg=std::string("Point inside edge check: Unhandled (Horizontal) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
			case 3:
                if(pt_.x <= v4)
				{
                    return inDirection*faceDir > 0;
				}
                if(pt_.x >= v8)
				{
                    return inDirection*faceDir > 0;
				}
                if(pt_.x < v5)
				{	//Between v4-v5
                    return _curves[2].isPointInside(pt_);
				}
                if(pt_.x < v7)
				{	//Between v5-v7
                    return inDirection*faceDir < 0;
				}
                if(pt_.x < v8)
				{
                    return _curves[7].isPointInside(pt_);
				}
				dbg=std::string("Point inside edge check: Unhandled (Horizontal) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
			case 4:
                if(pt_.x <= v1)
				{
                    return inDirection*faceDir < 0;
				}
                if(pt_.x >= v11)
				{
                    return inDirection*faceDir < 0;
				}
                if(pt_.x < v2)
				{   //Between v1-v2
                    return _curves[0].isPointInside(pt_);
				}
                if(pt_.x < v5)
				{   //Between v2-v5
                    return _curves[1].isPointInside(pt_);
				}
                if(pt_.x < v7)
				{	//Between v5-v7
                    return inDirection*faceDir < 0;
				}
                if(pt_.x < v10)
				{	//Between v7-v10
                    return _curves[8].isPointInside(pt_);
				}
                if(pt_.x < v11)
				{	//Between v10-v11
                    return _curves[9].isPointInside(pt_);
				}
				dbg=std::string("Point inside edge check: Unhandled (Horizontal) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
		}
	}
	else
	{	//Edge is vertical:
		float v1=_curves[1].b0.x;
		float v2=_curves[2].b0.x;
		float v3=_curves[3].b0.x;
		float v4=_curves[4].b0.x;
		float v5=_curves[5].b0.x;
		
		float h1=_curves[0].b0.y;
		float h2=_curves[1].b0.y;
		float h3=_curves[4].b0.y;
		float h4=_curves[3].b0.y;
		float h5=_curves[2].b0.y;
		float h6=_curves[5].b0.y;
		float h7=_curves[8].b0.y;
		float h8=_curves[7].b0.y;
		float h9=_curves[6].b0.y;
		float h10=_curves[9].b0.y;
		float h11=_curves[9].b2.y;

		U8 strip;
        if((pt_.x-v1)*faceDir <= 0)
		{
            return inDirection*faceDir < 0;
		}
        if((pt_.x-v5)*faceDir >= 0)
		{
            return inDirection*faceDir > 0;
		}
        if((pt_.x-v2)*faceDir < 0)
		{
			strip=1;
		}
        else if((pt_.x-v3)*faceDir < 0)
		{
			strip=2;
		}
        else if((pt_.x-v4)*faceDir < 0)
		{
			strip=3;
		}
        else if((pt_.x-v5)*faceDir < 0)
		{
			strip=4;
		}

		std::string dbg;
		switch(strip)
		{
			case 1:
                if(pt_.y <= h1)
				{
                    return inDirection*faceDir < 0;
				}
                if(pt_.y >= h11)
				{
                    return inDirection*faceDir < 0;
				}
                if(pt_.y < h2)
				{	//Between h1-h2
                    return _curves[0].isPointInside(pt_);
				}
                if(pt_.y < h5)
				{	//Between h2-h5
                    return _curves[1].isPointInside(pt_);
				}
                if(pt_.y < h7)
				{	//Between h5-h7
                    return inDirection*faceDir < 0;
				}
                if(pt_.y < h10)
				{	//Between h7-h10
                    return _curves[8].isPointInside(pt_);
				}
                if(pt_.y < h11)
				{	//Between h10-h11
                    return _curves[9].isPointInside(pt_);
				}
				break;
				dbg=std::string("Point inside edge check: Unhandled (Vertical) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
			case 2:
                if(pt_.y <= h4)
				{
                    return inDirection*faceDir > 0;
				}
                if(pt_.y >= h8)
				{
                    return inDirection*faceDir > 0;
				}
                if(pt_.y < h5)
				{	//Between h4-h5
                    return _curves[2].isPointInside(pt_);
				}
                if(pt_.y < h7)
				{	//Between h5-h7
                    return inDirection*faceDir < 0;
				}
                if(pt_.y < h8)
				{	//Between h7-h8
                    return _curves[7].isPointInside(pt_);
				}
				dbg=std::string("Point inside edge check: Unhandled (Vertical) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
			case 3:
                if(pt_.y <= h3)
				{
                    return inDirection*faceDir > 0;
				}
                if(pt_.y >= h9)
				{
                    return inDirection*faceDir > 0;
				}
                if(pt_.y < h4)
				{	//Between h3-h4
                    return _curves[3].isPointInside(pt_);
				}
                if(pt_.y < h8)
				{	//Between h4-h8
                    return inDirection*faceDir < 0;
				}
                if(pt_.y < h9)
				{	//Between h8-h9
                    return _curves[6].isPointInside(pt_);
				}
				dbg=std::string("Point inside edge check: Unhandled (Vertical) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
			case 4:
                if(pt_.y <= h3)
				{
                    return inDirection*faceDir > 0;
				}
                if(pt_.y >= h9)
				{
                    return inDirection*faceDir > 0;
				}
                if(pt_.y < h6)
				{	//Between h3-h6
                    return _curves[4].isPointInside(pt_);
				}
                if(pt_.y < h9)
				{	//Between h6-h9
                    return _curves[5].isPointInside(pt_);
				}
				dbg=std::string("Point inside edge check: Unhandled (Vertical) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
		}
    }
}

void CurvyEdge::rawRender(sf::RenderTarget &screen_, const sf::Color &color_, const float thickness_) const
{
    std::vector<sf::Vector2f> allPoints;
    for(auto &c: _curves)
    {
        std::vector<sf::Vector2f> curvePoints = c.getPoints();
        allPoints.insert(allPoints.end(),curvePoints.begin(),curvePoints.end());
    }

    for(U8 i=0; i < allPoints.size()-1; ++i)
    {
        Line currLine(allPoints[i],allPoints[i+1],color_,thickness_);
        currLine.render(screen_);
    }
}

void CurvyEdge::writeOut(std::ostream &os_) const
{
    write<const EdgeType>(os_,eCurvy);
    write<const DirEnum>(os_,_faceDir);
    write<const DirEnum>(os_,_inDirection);

    sf::Vector2f pos = this->getStartPoint();
    write<const float>(os_,pos.x);
    write<const float>(os_,pos.y);
}

Edge *CurvyEdge::readIn(std::istream &is_)
{
    DirEnum face,inside;
    sf::Vector2f pos;

    read<DirEnum>(is_,&face);
    read<DirEnum>(is_,&inside);
    read<float>(is_,&pos.x);
    read<float>(is_,&pos.y);

    CurvyEdge *ret = new CurvyEdge(PuzzleManager::getBaseCurvyEdge(true));

    while(ret->_faceDir != face)
    {
        ret->rotateRight();
    }

    if(ret->_inDirection != inside)
    {
        ret->flipInside();
    }

    ret->setPosition(pos);

    return ret;
}

void CurvyEdge::reverseEdge()
{
	std::reverse(_curves.begin(),_curves.end());
	for(auto &curve: _curves)
	{
		curve.reversePoints();
    }
}

void CurvyEdge::changeSprite() const
{
    auto it = std::find(PuzzleManager::_colors.begin(),PuzzleManager::_colors.end(),_currentColor);
    if(it != PuzzleManager::_colors.end())
    {
        U8 colorIndex = it - PuzzleManager::_colors.begin();

        // Textures are set for 400 wide pieces. Sprites will be scaled down
        _sprite.setTextureRect(sf::IntRect(0,_textureStartCoord + colorIndex*300,600,300));
    }
}

//////////////
// LineEdge //
//////////////

LineEdge::LineEdge(const bool isHorizontal_, const DirEnum inDirection_, const sf::Vector2f &startPoint_, const sf::Vector2f &endPoint_,
                   const sf::Vector2f &scale_):
    Edge(isHorizontal_,inDirection_,scale_),
    _startPt(startPoint_),
    _endPt(endPoint_)
{
    changeSprite();
}


LineEdge::LineEdge(const LineEdge &rhs_):
    Edge(rhs_),
    _startPt(rhs_._startPt),
    _endPt(rhs_._endPt)
{
}


sf::Vector2f LineEdge::getStartPoint() const
{
	return _startPt;
}


sf::Vector2f LineEdge::getEndPoint() const
{
	return _endPt;
}


void LineEdge::move(const sf::Vector2f &offset_)
{
    Edge::move(offset_);
    _startPt+=offset_;
    _endPt+=offset_;
}


void LineEdge::rotateRight(const sf::Vector2f &around_)
{
    Edge::rotateRight(around_);
    _startPt=pointRotateRight(_startPt,around_);
    _endPt=pointRotateRight(_endPt,around_);
}


void LineEdge::rotateLeft(const sf::Vector2f &around_)
{
    Edge::rotateLeft(around_);
    _startPt=pointRotateLeft(_startPt,around_);
    _endPt=pointRotateLeft(_endPt,around_);
}


bool LineEdge::isInside(const sf::Vector2f &pt_) const
{
    bool ret;
    switch(_inDirection)
    {
    case eUp:
        ret = _startPt.y-pt_.y >= 0;
        break;
    case eRight:
        ret = _startPt.x-pt_.x <= 0;
        break;
    case eDown:
        ret = _startPt.y-pt_.y <= 0;
        break;
    case eLeft:
        ret = _startPt.x-pt_.x >= 0;
        break;
    }

    return ret;
}

void LineEdge::rawRender(sf::RenderTarget &screen_, const sf::Color &color_, const float thickness_) const
{
    Line edge(_startPt,_endPt,color_,thickness_);
    edge.render(screen_);
}

void LineEdge::writeOut(std::ostream &os_) const
{
    write<const EdgeType>(os_,eLine);
    write<const bool>(os_,_isHorizontal);
    write<const DirEnum>(os_,_inDirection);
    write<const float>(os_,_startPt.x);
    write<const float>(os_,_startPt.y);
}

Edge *LineEdge::readIn(std::istream &is_)
{
    sf::Vector2f start;
    bool horizontal;
    DirEnum inside;

    read<bool>(is_,&horizontal);
    read<DirEnum>(is_,&inside);
    read<float>(is_,&start.x);
    read<float>(is_,&start.y);

    LineEdge *ret = new LineEdge(PuzzleManager::getBaseLineEdge(true));
    if(!horizontal)
    {
        ret->rotateRight();
    }
    if(ret->_inDirection != inside)
    {
        ret->flipInside();
    }
    ret->move(start);

    return ret;
}

void LineEdge::changeSprite() const
{
    auto it = std::find(PuzzleManager::_colors.begin(),PuzzleManager::_colors.end(),_currentColor);
    if(it != PuzzleManager::_colors.end())
    {
        U8 colorIndex = it - PuzzleManager::_colors.begin();

        // Textures are set for 400 wide pieces. Sprites will be scaled down
        _sprite.setTextureRect(sf::IntRect(600,_textureStartCoord + colorIndex*300,600,300));
    }
}
