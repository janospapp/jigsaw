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


int countChars(const std::string &str_, const std::vector<char> &chars_)
{
    int cnt = 0;
    for(auto &ch: chars_)
    {
        for(auto &strCh: str_)
        {
            if(strCh == ch)
            {
                ++cnt;
            }
        }
    }
    return cnt;
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


std::string toLower(const std::string &str_)
{
    std::string ret;
    ret.resize(str_.length());
    std::transform(str_.begin(),str_.end(),ret.begin(),::tolower);

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


float vectorLength(const sf::Vector2f &v_)
{
    return std::sqrt(v_.x*v_.x + v_.y*v_.y);
}


sf::FloatRect combineRectangles(const sf::FloatRect &r1_, const sf::FloatRect &r2_)
{
    sf::FloatRect ret;
    ret.top = std::min(r1_.top,r2_.top);
    ret.left = std::min(r1_.left,r2_.left);
    ret.width = std::max(r1_.left + r1_.width, r2_.left + r2_.width) - ret.left;
    ret.height = std::max(r1_.top + r1_.height, r2_.top + r2_.height) - ret.top;

    return ret;
}


sf::FloatRect getIntersectionRect(const sf::FloatRect &r1_, const sf::FloatRect &r2_)
{
    sf::FloatRect ret;
    ret.top = std::max(r1_.top,r2_.top);
    ret.left = std::max(r1_.left,r2_.left);
    ret.width = std::min(r1_.left + r1_.width, r2_.left + r2_.width) - ret.left;
    ret.height = std::min(r1_.top + r1_.height, r2_.top + r2_.height) - ret.top;

    return ret;
}


bool containsRectangle(const sf::FloatRect &big_, const sf::FloatRect &small_)
{
    return (big_.top <= small_.top) && (big_.left <= small_.left) && (big_.top + big_.height >= small_.top + small_.height) &&
           (big_.left + big_.width >= small_.left + small_.width);
}


void trimRectangle(sf::FloatRect &what_, const sf::FloatRect &trimmer_)
{
    sf::FloatRect trimmed = sf::FloatRect(std::max(what_.left,trimmer_.left), std::max(what_.top,trimmer_.top),1.f,1.f);
    const float right = std::min(what_.left + what_.width, trimmer_.left + trimmer_.width);
    const float bottom = std::min(what_.top + what_.height, trimmer_.top + trimmer_.height);
    trimmed.width = right - trimmed.left;
    trimmed.height = bottom - trimmed.top;

    what_ = trimmed;
}


void write(std::ostream &os, const std::vector<U8> &vec_)
{
    int size = vec_.size();
    write<int>(os, size);
    for(U8 i: vec_)
    {
        write<U8>(os, i);
    }
}


void read(std::istream &is, std::vector<U8> &vec_)
{
    int size;
    U8 temp;
    read<int>(is,&size);
    vec_.resize(size);
    for(int i = 0; i < size; ++i)
    {
        read<U8>(is,&temp);
        vec_[i] = temp;
    }
}
