#include "Managers/ImageManager.hpp"
#include "Utils/Debug.hpp"
#include <iostream>
#include <utility>

namespace ImageManager
{
    static std::map<U32,std::pair<std::unique_ptr<sf::Image>,std::unique_ptr<sf::Texture>>> _mapImages;

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
    sf::Texture* getTexture(const std::string &path_)
    {
        sf::Texture *ret = nullptr;
        sf::Image *img = nullptr;
        U32 hash = Hash(path_.c_str());
        auto it = _mapImages.find(hash);
        if(it == _mapImages.end())
        {
            img = loadImage(path_);
            addImage(path_,img);
        }
        else
        {
            img = it->second.first.get();
            ret = it->second.second.get();
        }

        if(ret == nullptr)
        {
            ret=new sf::Texture();
            if(img != nullptr && ret->loadFromImage(*img))
            {
                auto it2 = _mapImages.find(hash);
                it2->second.second = std::move(std::unique_ptr<sf::Texture>(ret));
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
            if(img = loadImage(path))
            {
                addImage(path,img);
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


    void saveImage(const std::string &path_)
    {
        U32 hash=Hash(path_.c_str());
        auto it=_mapImages.find(hash);
        if(it != _mapImages.end())
        {
            it->second.first->saveToFile(path_);
        }
    }


    sf::Image* loadImage(const std::string &path_)
    {
        sf::Image *img=new sf::Image();
        if(!img->loadFromFile(path_))
        {
            img = nullptr;
        }

        return img;
    }

    sf::Image* scaleImage(const sf::Image &origImg_, const float scaleFactor_)
    {
        sf::Image *newImg = new sf::Image();
        sf::Vector2u origSize(origImg_.getSize());
        sf::Vector2u newSize(scaleFactor_*origSize.x,scaleFactor_*origSize.y);
        newImg->create(newSize.x, newSize.y);

        for(U16 i=0; i < newSize.x; ++i)
        {
            for(U16 j=0; j < newSize.y; ++j)
            {
                newImg->setPixel(i,j,origImg_.getPixel(i/scaleFactor_,j/scaleFactor_));
            }
        }

        return newImg;
    }

    void removeTexture(const std::string &name_)
    {
        U32 hash = Hash(name_.c_str());
        _mapImages.erase(hash);
    }

}
