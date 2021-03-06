#ifndef IMAGEMANAGER_HPP
#define IMAGEMANAGER_HPP

#include "Utils/Config.hpp"
#include "SFML/Graphics.hpp"
#include <map>
#include <memory>
#include <string>


namespace ImageManager
{
    void addImage(const std::string &path_, sf::Image *img_);
    void addTexture(const std::string &path_, sf::Texture *tx_);
    sf::Texture* getTexture(const std::string &path_);
    void releaseTextures();	//Removes the textures from video memory.
    void removeTexture(const std::string &name_);
    sf::Image* getImage(const std::string &path_);
    sf::Image* newImage(const std::string &path_);
    sf::Image* loadImage(const std::string &path_);
    void saveImage(const std::string &path_);
    sf::Image* scaleImage(const sf::Image &origImg_, const float scaleFactor_);
}

#endif //IMAGEMANAGER_HPP
