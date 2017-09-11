#include "Utils/Random.hpp"
#include "GameCore/GameApp.hpp"
#include <ctime>


void Random::initialize()
{
	_generator.seed(time(NULL));
	_pixelGeneratorX.param(std::uniform_real_distribution<float>::param_type(0.f,(float)GameApp::Options.iScreenWidth));
	_pixelGeneratorY.param(std::uniform_real_distribution<float>::param_type(0.f,(float)GameApp::Options.iScreenHeight));
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
