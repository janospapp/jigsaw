#include "Utils/Helper.hpp"
#include "Utils/Debug.hpp"
#include "GameCore/GameApp.hpp"
#include <sstream>
#include <cmath>


int strToInt(std::string str)
{
    std::stringstream stream(str);
	int ret = 0;
	stream >> ret;
	return ret;
}


float strToFloat(std::string str)
{
    std::stringstream stream(str);
	float ret = 0.0f;
	stream >> ret;
	return ret;
}


std::string intToStr(const unsigned int number)
{
	std::stringstream stream;
	stream << number;
	return std::string(stream.str());
}


std::string floatToStr(const float number)
{
    std::stringstream stream;
    stream << number;
    return std::string(stream.str());
}


float vectorDist(const sf::Vector2i &v1, const sf::Vector2i &v2)
{
    return std::sqrt(float((v1.x-v2.x)*(v1.x-v2.x) + (v1.y-v2.y)*(v1.y-v2.y)));
}


float vectorDist(const sf::Vector2f &v1, const sf::Vector2f &v2)
{
	return std::sqrt((v1.x-v2.x)*(v1.x-v2.x) + (v1.y-v2.y)*(v1.y-v2.y));
}


int gridDist(const sf::Vector2i &v1, const sf::Vector2i &v2)
{
	return std::abs(v1.x - v2.x) + std::abs(v1.y - v2.y);
}


float gridDist(const sf::Vector2f &v1, const sf::Vector2f &v2)
{
    return std::abs(v1.x - v2.x) + std::abs(v1.y - v2.y);
}


sf::Vector2f operator*(const sf::Vector2f point, const sf::Transform &trafo)
{
	const float *matrix=trafo.getMatrix();
	float w=point.x*matrix[12]+point.y*matrix[13]+matrix[15];
	sf::Vector2f ret;
	ret.x=(point.x*matrix[0]+point.y*matrix[1]+matrix[3])/w;
	ret.y=(point.x*matrix[4]+point.y*matrix[5]+matrix[7])/w;

	return ret;
}



sf::Vector2f pointRotateRight(const sf::Vector2f &pt, const sf::Vector2f &around)
{
	sf::Transform rot(0,1,0,-1,0,0,around.x+around.y,around.y-around.x,1);
	return pt*rot;
}


sf::Vector2f pointRotateLeft(const sf::Vector2f &pt, const sf::Vector2f &around)
{
	sf::Transform rot(0,-1,0,1,0,0,around.x-around.y,around.x+around.y,1);
	return pt*rot;
}
