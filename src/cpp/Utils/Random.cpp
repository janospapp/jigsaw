#include "Utils/Random.hpp"
#include <ctime>


void Random::initialize(sf::Vector2f xLimit_, sf::Vector2f yLimit_)
{
	_generator.seed(time(NULL));
    _pixelGeneratorX.param(std::uniform_real_distribution<float>::param_type(xLimit_.x,xLimit_.y));
    _pixelGeneratorY.param(std::uniform_real_distribution<float>::param_type(yLimit_.x,yLimit_.y));
}


sf::Vector2f Random::randomPixel()
{
	sf::Vector2f pix;
	pix.x=_pixelGeneratorX(_generator);
	pix.y=_pixelGeneratorY(_generator);
	return pix;
}

std::default_random_engine Random::_generator;
std::uniform_real_distribution<float> Random::_pixelGeneratorX;
std::uniform_real_distribution<float> Random::_pixelGeneratorY;
