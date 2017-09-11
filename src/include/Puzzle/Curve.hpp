#ifndef CURVE_HPP
#define CURVE_HPP

#include "Utils/Config.hpp"
#include "SFML/Graphics.hpp"

class Curve
{
public:
	Curve();
	Curve(const sf::Vector2f &b0_, const sf::Vector2f &b1_, const sf::Vector2f &b2_, const I8 dir=1);
	Curve(const Curve &rhs);
	
	bool isPointInside(const sf::Vector2f &toTest) const;
	void flipDirection();
	void move(const sf::Vector2f &offset);
	void rotateRight(const sf::Vector2f &around);
	void rotateLeft(const sf::Vector2f &around);
	void reversePoints();

    // Cut the curve into the given number of parts, and returns the cutpoints.
    std::vector<sf::Vector2f> getPoints() const;


//member variables (they are also public for simplicity
	sf::Vector2f b0;
	sf::Vector2f b1;
	sf::Vector2f b2;
	I8 _direction;

private:
    std::vector<sf::Vector2f> getParts(const U8 parts_) const;

    // Returns the possible maximum length of the curves (= length of the b0->b1->b2 linestrip).
    float maxLength() const;

};

#endif
