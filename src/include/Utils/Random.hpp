#ifndef RANDOM_HPP
#define RANDOM_HPP

#include "SFML/Graphics.hpp"
#include <random>


class Random
{
public:
	static void initialize();
	static sf::Vector2f randomPixel();	//Always generate a random pixel on the screen.

private:
	static std::default_random_engine _generator;
	static std::uniform_real_distribution<float> _pixelGeneratorX;
	static std::uniform_real_distribution<float> _pixelGeneratorY;

};

#endif
