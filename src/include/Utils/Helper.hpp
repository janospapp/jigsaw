#ifndef HELPER_HPP
#define HELPER_HPP

#include <string>
#include <iostream>
#include "SFML/Graphics.hpp"
#include "Utils/Config.hpp"

int strToInt(std::string str);
float strToFloat(std::string str);
std::string intToStr(const unsigned int number);
std::string floatToStr(const float number);

int countChars(const std::string &str_, const std::vector<char> &chars_);

float vectorDist(const sf::Vector2i &v1, const sf::Vector2i &v2);
float vectorDist(const sf::Vector2f &v1, const sf::Vector2f &v2);

int gridDist(const sf::Vector2i &v1, const sf::Vector2i &v2);
float gridDist(const sf::Vector2f &v1, const sf::Vector2f &v2);

float vectorLength(const sf::Vector2f &v_);

sf::Vector2f operator*(const sf::Vector2f point, const sf::Transform &trafo);
sf::Vector2f pointRotateRight(const sf::Vector2f &pt, const sf::Vector2f &around);
sf::Vector2f pointRotateLeft(const sf::Vector2f &pt, const sf::Vector2f &around);

std::string toLower(const std::string &str_);

sf::FloatRect combineRectangles(const sf::FloatRect &r1_, const sf::FloatRect &r2_);
sf::FloatRect getIntersectionRect(const sf::FloatRect &r1_, const sf::FloatRect &r2_);
bool containsRectangle(const sf::FloatRect &big_, const sf::FloatRect &small_);
void trimRectangle(sf::FloatRect &what_, const sf::FloatRect &trimmer_);

// Writing numbers into binary files.
template<typename T>
void write(std::ostream &os, T &n)
{
    os.write((char *)&n,sizeof(T));
}

void write(std::ostream &os, const std::vector<U8> &vec_);

template<typename T>
void read(std::istream &is, T *n)
{
    is.read((char *)n,sizeof(T));
}

void read(std::istream &is, std::vector<U8> &vec_);

#endif // HELPER_HPP
