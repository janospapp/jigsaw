#ifndef HELPER_HPP
#define HELPER_HPP

#include <string>
#include "SFML/Graphics.hpp"

int strToInt(std::string str);
float strToFloat(std::string str);
std::string intToStr(const unsigned int number);
std::string floatToStr(const float number);

float vectorDist(const sf::Vector2i &v1, const sf::Vector2i &v2);
float vectorDist(const sf::Vector2f &v1, const sf::Vector2f &v2);

int gridDist(const sf::Vector2i &v1, const sf::Vector2i &v2);
float gridDist(const sf::Vector2f &v1, const sf::Vector2f &v2);

//void CopyImage(const sf::Image &src, sf::Image &dest, sf::IntRect copyWhat, sf::Vector2i copyWhere, float scale=1.0f);
//void MaskColor(sf::Image &pic, const sf::Color what = sf::Color(0,255,0), const sf::Color replace = sf::Color(0,0,0,0));

sf::Vector2f operator*(const sf::Vector2f point, const sf::Transform &trafo);
sf::Vector2f pointRotateRight(const sf::Vector2f &pt, const sf::Vector2f &around);
sf::Vector2f pointRotateLeft(const sf::Vector2f &pt, const sf::Vector2f &around);

#endif // HELPER_HPP
