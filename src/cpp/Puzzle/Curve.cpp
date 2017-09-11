#include "Puzzle/Curve.hpp"
#include "Utils/Helper.hpp"

Curve::Curve()
{
}


Curve::Curve(const sf::Vector2f &b0_, const sf::Vector2f &b1_, const sf::Vector2f &b2_, const I8 dir):
	b0(b0_),
	b1(b1_),
	b2(b2_),
	_direction(dir)
{
}



Curve::Curve(const Curve &rhs):
	b0(rhs.b0),
	b1(rhs.b1),
	b2(rhs.b2),
	_direction(rhs._direction)
{
}


bool Curve::isPointInside(const sf::Vector2f &toTest) const
{
	sf::Transform B(b0.x,b0.y,1,b1.x,b1.y,1,b2.x,b2.y,1);
	static sf::Transform M2iF(0,0,1,0.5f,0,1,1,1,1);
	sf::Transform T(B.getInverse()*M2iF);
	sf::Vector2f p(toTest*T);
	return _direction*(p.x*p.x-p.y) < 0;
}


void Curve::flipDirection()
{
	_direction *= -1;
}


void Curve::move(const sf::Vector2f &offset)
{
	b0 += offset;
	b1 += offset;
	b2 += offset;
}


void Curve::rotateRight(const sf::Vector2f &around)
{
	b0=pointRotateRight(b0,around);
	b1=pointRotateRight(b1,around);
	b2=pointRotateRight(b2,around);
}


void Curve::rotateLeft(const sf::Vector2f &around)
{
	b0=pointRotateLeft(b0,around);
	b1=pointRotateLeft(b1,around);
	b2=pointRotateLeft(b2,around);
}


void Curve::reversePoints()
{
	sf::Vector2f temp=b0;
	b0=b2;
	b2=temp;
}


std::vector<sf::Vector2f> Curve::getPoints() const
{
    //return getParts(maxLength() / 5.f);
    return getParts(3);
}


float Curve::maxLength() const
{
    return vectorDist(b0,b1) + vectorDist(b1,b2);
}


std::vector<sf::Vector2f> Curve::getParts(const U8 parts_) const
{
    std::vector<sf::Vector2f> ret;
    const float frac = 1.f / (float) parts_;
    for(U8 i=0; i <= parts_; ++i)
    {
        float t = i*frac;
        sf::Vector2f thisPoint((1.f-t)*(1.f-t)*b0 + 2*(1.f-t)*t*b1 + t*t*b2);
        ret.push_back(thisPoint);
    }

    return ret;
}
