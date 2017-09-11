#include "Managers/ImageManager.hpp"
#include "Utils/Debug.hpp"
#include <iostream>
#include <utility>

namespace ImageManager
{
    static std::map<U32,std::pair<std::unique_ptr<sf::Image>,std::unique_ptr<sf::Texture>>> _mapImages;

    sf::Image* loadImage(const std::string &path, U32 hash);

    //I applied the FNV hash
    U32 Hash(const char *str)
    {
        int i = 0;
        U8 c = str[i];
        U32 h = 2166136261;
        while(c != 0)
        {
            h = (h * 16777619)^c;
            c = str[++i];
        }

        return h;

    }


    void addImage(const std::string &path_, sf::Image *img_)
    {
        if(img_ != nullptr && !path_.empty())
        {
            // Check for existence.
            U32 hash = Hash(path_.c_str());
            auto it = _mapImages.find(hash);
            if(it == _mapImages.end())
            {
                _mapImages.insert(std::pair<U32,std::pair<std::unique_ptr<sf::Image>,std::unique_ptr<sf::Texture>>>(
                      hash,std::pair<std::unique_ptr<sf::Image>,std::unique_ptr<sf::Texture>>(
                      std::unique_ptr<sf::Image>(img_),std::unique_ptr<sf::Texture>(nullptr))));
            }
        }
    }


    void addTexture(const std::string &path_, sf::Texture *tx_)
    {
        if(tx_ != nullptr && !path_.empty())
        {
            // Check for existence.
            U32 hash = Hash(path_.c_str());
            auto it = _mapImages.find(hash);
            if(it == _mapImages.end())
            {
                _mapImages.insert(std::pair<U32,std::pair<std::unique_ptr<sf::Image>,std::unique_ptr<sf::Texture>>>(
                      hash,std::pair<std::unique_ptr<sf::Image>,std::unique_ptr<sf::Texture>>(
                      std::unique_ptr<sf::Image>(nullptr),std::unique_ptr<sf::Texture>(tx_))));
            }
        }
    }

    //This function loads an image (if it's not loaded yet), and returns the texture pointer.
    //The 'path' is the file location and name relative to the Graphics folder. I store this path's hash
    //value to identify the textures.
    sf::Texture* getTexture(const std::string &path)
    {
        U32 hash = Hash(path.c_str());
        auto it = _mapImages.find(hash);
        if(it == _mapImages.end())
        {	//The image isn't loaded yet.
            if(loadImage(path,hash))
            {
                return _mapImages[hash].second.get();
            }
            else
            {
                return nullptr;
            }
        }
        else
        {
            sf::Texture *ret=it->second.second.get();
            if(ret==nullptr)
            {
                ret=new sf::Texture();
                if(ret->loadFromImage(*it->second.first.get()))
                {
                    return ret;
                }
                else
                {
                    return nullptr;
                }
            }
            else
            {
                return ret;
            }
        }
    }


    void releaseTextures()
    {
        for(auto &it: _mapImages)
        {
            it.second.second.reset();
        }
    }


    sf::Image* getImage(const std::string &path)
    {
        U32 hash = Hash(path.c_str());
        auto it = _mapImages.find(hash);
        if(it == _mapImages.end())
        {	//The image isn't loaded yet.
            sf::Image *img;
            if(img=loadImage(path,hash))
            {
                //This is only for debug reasons. TO DO: remove it from the release code!
                return img;
            }
            else
            {
                return nullptr;
            }
        }
        else
        {
            if(it->second.first != nullptr)
            {
                return it->second.first.get();
            }
            else if(it->second.second != nullptr)
            {   // Texture is set, but the image isn't loaded.
                sf::Image *newImg=new sf::Image();
                *newImg = it->second.second->copyToImage();
                it->second.first = std::move(std::unique_ptr<sf::Image>(newImg));
                return newImg;
            }
            else
            {
    #ifdef _DEBUG
                std::string dbg("The image, what you've wanted to create already exists! Path: ");
                dbg+=path;
                DebugPrint(std::move(dbg));
    #endif

                return nullptr;
            }
        }
    }


    sf::Image* newImage(const std::string &path)
    {
        U32 hash=Hash(path.c_str());
        auto it=_mapImages.find(hash);
        if(it != _mapImages.end())
        {

    #ifdef _DEBUG
            std::string dbg("The image, what you've wanted to create already exists! Path: ");
            dbg+=path;
            DebugPrint(std::move(dbg));
    #endif

            return it->second.first.get();
        }
        else
        {
            sf::Image *newImg=new sf::Image();

            _mapImages.insert(std::pair<U32,std::pair<std::unique_ptr<sf::Image>,std::unique_ptr<sf::Texture>>>
                                (hash,std::pair<std::unique_ptr<sf::Image>,std::unique_ptr<sf::Texture>>(std::unique_ptr<sf::Image>(newImg),
                                                                                                         std::unique_ptr<sf::Texture>(nullptr))));

            return newImg;
        }
    }


    void saveImage(const std::string &path)
    {
        U32 hash=Hash(path.c_str());
        std::map<U32,std::pair<std::unique_ptr<sf::Image>,std::unique_ptr<sf::Texture>>>::const_iterator it=_mapImages.find(hash);
        if(it != _mapImages.end())
        {
            std::string str("Graphics/");
            str += path;
            it->second.first->saveToFile(str);
        }
    }


    sf::Image* loadImage(const std::string &path, U32 hash)
    {
        sf::Image *img=new sf::Image();
        std::string str("Graphics/");
        str += path;
        if(!img->loadFromFile(str))
        {

    #ifdef _DEBUG
            //I check if the loading went well.
            std::string debugStr = std::string("Error while loading image from file: ")+str;
            DebugPrint(std::move(debugStr));
    #endif

            return nullptr;
        }
        else
        {	//I add the image to the map.
            sf::Texture *tex=new sf::Texture();
            if(tex->loadFromImage(*img))
            {
                _mapImages.insert(std::pair<U32,std::pair<std::unique_ptr<sf::Image>,std::unique_ptr<sf::Texture>>>
                                    (hash,std::pair<std::unique_ptr<sf::Image>,std::unique_ptr<sf::Texture>>(std::unique_ptr<sf::Image>(img),
                                                                                                             std::unique_ptr<sf::Texture>(tex))));
                return img;
            }
            else
            {

    #ifdef _DEBUG
                std::string debugStr = std::string("Error while loading texture from image: ")+str;
                DebugPrint(std::move(debugStr));
    #endif

                return nullptr;
            }
        }
    }
}
