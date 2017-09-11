#include "Puzzle/Edge.hpp"  
#include "Utils/Helper.hpp"
#include "Utils/Debug.hpp"
#include "Puzzle/Line.hpp"

//////////
// Edge //
//////////

Edge::Edge():
	_isHorizontal(false),
	_inDirection(1)
{
}


Edge::Edge(const bool isHorizontal, const I8 direction):
	_isHorizontal(isHorizontal),
	_inDirection(direction)
{
}


Edge::Edge(const Edge &rhs):
	_isHorizontal(rhs._isHorizontal),
	_inDirection(rhs._inDirection)
{
}


void Edge::rotateRight(const sf::Vector2f &around)
{
	_isHorizontal=!_isHorizontal;

	if(_isHorizontal)
	{	//It was vertical before, so I have to change the inside direction
		_inDirection *= -1;
	}
}


void Edge::rotateLeft(const sf::Vector2f &around)
{
	_isHorizontal=!_isHorizontal;

	if(!_isHorizontal)
	{	//It was horizontal before, so I have to change the inside direction
		_inDirection *= -1;
	}
}


void Edge::flipInside()
{
	_inDirection*=-1.f;
}


void Edge::render(sf::RenderTarget &screen_, const sf::Color &color_, const float thickNess_) const
{
    std::vector<sf::Vector2f> points(getPoints());
    for(U8 i=0; i < points.size()-1; ++i)
    {
        Line current(points[i],points[i+1],color_,thickNess_);
        current.render(screen_);
    }
}


void Edge::addLinesToRender(std::vector<sf::Vertex> &vertexVec_, const sf::Color &color_, const float thickNess_) const
{
    std::vector<sf::Vector2f> points(getPoints());
    for(U8 i=0; i < points.size()-1; ++i)
    {
        Line current(points[i],points[i+1],color_,thickNess_);
        current.addVertices(vertexVec_);
    }
}


void Edge::setPosition(const sf::Vector2f &pos)
{   //Moves the edge to the new position
	sf::Vector2f currPos=getStartPoint();
	sf::Vector2f offset=pos-currPos;
	move(offset);
}


///////////////
// CurvyEdge //
///////////////

CurvyEdge::CurvyEdge(const bool isHorizontal, const I8 direction, const I8 faceDir, std::vector<sf::Vector2f> &ctrPts, std::vector<I16> &dirs):
	Edge(isHorizontal,direction),
	_faceDir(faceDir),
	_curves(10)
{
	for(int i=0; i<10; ++i)
	{
		_curves[i]=Curve(ctrPts[3*i],ctrPts[3*i+1],ctrPts[3*i+2],dirs[i]);
	}
}


CurvyEdge::CurvyEdge(const CurvyEdge &rhs):
	Edge(rhs),
	_curves(rhs._curves),
	_faceDir(rhs._faceDir)
{
}


sf::Vector2f CurvyEdge::getStartPoint() const
{
	return _curves[1].b0;
}


sf::Vector2f CurvyEdge::getEndPoint() const
{
	return _curves[9].b2;
}


void CurvyEdge::move(const sf::Vector2f &offset)
{
	for(auto &curve: _curves)
	{
		curve.move(offset);
	}
}


void CurvyEdge::rotateRight(const sf::Vector2f &around)
{
	Edge::rotateRight(around);
	for(int i=0; i<10; ++i)
	{
		_curves[i].rotateRight(around);
	}

	if(_isHorizontal)
	{	//It was a vertical edge before the rotation
		reverseEdge();
		_faceDir *= (I8)-1;
	}
}


void CurvyEdge::rotateLeft(const sf::Vector2f &around)
{
	Edge::rotateLeft(around);
	for(int i=0; i<10; ++i)
	{
		_curves[i].rotateLeft(around);
	}

	if(!_isHorizontal)
	{	//It was a horizontal edge before the rotation
		reverseEdge();
		_faceDir *= (I8)-1;
	}
}


void CurvyEdge::flipInside()
{
	Edge::flipInside();
	for(auto &curve: _curves)
	{
		curve.flipDirection();
	}
}


bool CurvyEdge::isInside(const sf::Vector2f &pt) const
{
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
        if((pt.y - h1)*_faceDir <= 0)
		{
			return _inDirection*_faceDir > 0;
		}
		if((pt.y - h5)*_faceDir >= 0)
		{
			return _inDirection*_faceDir < 0;
		}

		if((pt.y - h2)*_faceDir < 0)
		{
			strip=1;
		}
		else if((pt.y-h3)*_faceDir < 0)
		{
			strip=2;
		}
		else if((pt.y-h4)*_faceDir < 0)
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
				if(pt.x <= v3)
				{
					return _inDirection*_faceDir > 0;
				}
				if(pt.x >= v9)
				{
					return _inDirection*_faceDir > 0;
				}
				if(pt.x < v6)
				{
					return _curves[4].isPointInside(pt);
				}
				else
				{
					return _curves[5].isPointInside(pt);
				}
				dbg=std::string("Point inside edge check: Unhandled (Horizontal) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
			case 2:
				if(pt.x <= v3)
				{
					return _inDirection*_faceDir > 0;
				}
				if(pt.x >= v9)
				{
					return _inDirection*_faceDir > 0;
				}
                if(pt.x < v4)
				{	//Point is between v3-v4
					return _curves[3].isPointInside(pt);
				}
				else if(pt.x < v8)
				{	//Between v4-v8
					return _inDirection*_faceDir < 0;
				}
				else
				{	//Between v8-v9
					return _curves[6].isPointInside(pt);
				}
				dbg=std::string("Point inside edge check: Unhandled (Horizontal) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
			case 3:
				if(pt.x <= v4)
				{
					return _inDirection*_faceDir > 0;
				}
				if(pt.x >= v8)
				{
					return _inDirection*_faceDir > 0;
				}
				if(pt.x < v5)
				{	//Between v4-v5
					return _curves[2].isPointInside(pt);
				}
				if(pt.x < v7)
				{	//Between v5-v7
					return _inDirection*_faceDir < 0;
				}
				if(pt.x < v8)
				{
					return _curves[7].isPointInside(pt);
				}
				dbg=std::string("Point inside edge check: Unhandled (Horizontal) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
			case 4:
				if(pt.x <= v1)
				{
					return _inDirection*_faceDir < 0;
				}
				if(pt.x >= v11)
				{
					return _inDirection*_faceDir < 0;
				}
				if(pt.x < v2)
				{   //Between v1-v2
					return _curves[0].isPointInside(pt);
				}
				if(pt.x < v5)
				{   //Between v2-v5
					return _curves[1].isPointInside(pt);
				}
				if(pt.x < v7)
				{	//Between v5-v7
					return _inDirection*_faceDir < 0;
				}
				if(pt.x < v10)
				{	//Between v7-v10
					return _curves[8].isPointInside(pt);
				}
				if(pt.x < v11)
				{	//Between v10-v11
					return _curves[9].isPointInside(pt);
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
		if((pt.x-v1)*_faceDir <= 0)
		{
			return _inDirection*_faceDir < 0;
		}
		if((pt.x-v5)*_faceDir >= 0)
		{
			return _inDirection*_faceDir > 0;
		}
		if((pt.x-v2)*_faceDir < 0)
		{
			strip=1;
		}
		else if((pt.x-v3)*_faceDir < 0)
		{
			strip=2;
		}
		else if((pt.x-v4)*_faceDir < 0)
		{
			strip=3;
		}
		else if((pt.x-v5)*_faceDir < 0)
		{
			strip=4;
		}

		std::string dbg;
		switch(strip)
		{
			case 1:
				if(pt.y <= h1)
				{
					return _inDirection*_faceDir < 0;
				}
				if(pt.y >= h11)
				{
					return _inDirection*_faceDir < 0;
				}
				if(pt.y < h2)
				{	//Between h1-h2
					return _curves[0].isPointInside(pt);
				}
				if(pt.y < h5)
				{	//Between h2-h5
					return _curves[1].isPointInside(pt);
				}
				if(pt.y < h7)
				{	//Between h5-h7
					return _inDirection*_faceDir < 0;
				}
				if(pt.y < h10)
				{	//Between h7-h10
					return _curves[8].isPointInside(pt);
				}
				if(pt.y < h11)
				{	//Between h10-h11
					return _curves[9].isPointInside(pt);
				}
				break;
				dbg=std::string("Point inside edge check: Unhandled (Vertical) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
			case 2:
				if(pt.y <= h4)
				{
					return _inDirection*_faceDir > 0;
				}
				if(pt.y >= h8)
				{
					return _inDirection*_faceDir > 0;
				}
				if(pt.y < h5)
				{	//Between h4-h5
					return _curves[2].isPointInside(pt);
				}
				if(pt.y < h7)
				{	//Between h5-h7
					return _inDirection*_faceDir < 0;
				}
				if(pt.y < h8)
				{	//Between h7-h8
					return _curves[7].isPointInside(pt);
				}
				dbg=std::string("Point inside edge check: Unhandled (Vertical) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
			case 3:
				if(pt.y <= h3)
				{
					return _inDirection*_faceDir > 0;
				}
				if(pt.y >= h9)
				{
					return _inDirection*_faceDir > 0;
				}
				if(pt.y < h4)
				{	//Between h3-h4
					return _curves[3].isPointInside(pt);
				}
				if(pt.y < h8)
				{	//Between h4-h8
					return _inDirection*_faceDir < 0;
				}
				if(pt.y < h9)
				{	//Between h8-h9
					return _curves[6].isPointInside(pt);
				}
				dbg=std::string("Point inside edge check: Unhandled (Vertical) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
			case 4:
				if(pt.y <= h3)
				{
					return _inDirection*_faceDir > 0;
				}
				if(pt.y >= h9)
				{
					return _inDirection*_faceDir > 0;
				}
				if(pt.y < h6)
				{	//Between h3-h6
					return _curves[4].isPointInside(pt);
				}
				if(pt.y < h9)
				{	//Between h6-h9
					return _curves[5].isPointInside(pt);
				}
				dbg=std::string("Point inside edge check: Unhandled (Vertical) case when strip is");
                dbg += intToStr(strip);
				dbg += ".\n";
                debugPrint(std::move(dbg));
				break;
		}
	}
}


void CurvyEdge::reverseEdge()
{
	std::reverse(_curves.begin(),_curves.end());
	for(auto &curve: _curves)
	{
		curve.reversePoints();
	}
}


std::vector<sf::Vector2f> CurvyEdge::getPoints() const
{
    auto ret = std::vector<sf::Vector2f>();
    for(auto &component: _curves)
    {
        auto points = component.getPoints();
        ret.insert(ret.end(), points.begin(), points.end());
    }

    return ret;
}


//////////////
// LineEdge //
//////////////

LineEdge::LineEdge(const bool isHorizontal, const I8 direction, const sf::Vector2f &startPoint, const sf::Vector2f &endPoint):
	Edge(isHorizontal,direction),
	_startPt(startPoint),
	_endPt(endPoint)
{
}


LineEdge::LineEdge(const LineEdge &rhs):
	Edge(rhs),
	_startPt(rhs._startPt),
	_endPt(rhs._endPt)
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


void LineEdge::move(const sf::Vector2f &offset)
{
	_startPt+=offset;
	_endPt+=offset;
}


void LineEdge::rotateRight(const sf::Vector2f &around)
{
	Edge::rotateRight(around);
	_startPt=pointRotateRight(_startPt,around);
	_endPt=pointRotateRight(_endPt,around);
}


void LineEdge::rotateLeft(const sf::Vector2f &around)
{
	Edge::rotateLeft(around);
	_startPt=pointRotateLeft(_startPt,around);
	_endPt=pointRotateLeft(_endPt,around);
}


bool LineEdge::isInside(const sf::Vector2f &pt) const
{
	if(_isHorizontal)
	{
		return (_startPt.y-pt.y)*_inDirection >= 0;
	}
	else
	{   //it's vertical
		return (_startPt.x-pt.x)*_inDirection <= 0;
	}
}


std::vector<sf::Vector2f> LineEdge::getPoints() const
{
    std::vector<sf::Vector2f> ret = {_startPt, _endPt};
    return ret;
}
