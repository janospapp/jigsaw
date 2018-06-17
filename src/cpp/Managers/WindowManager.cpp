#include "Managers/WindowManager.hpp"
#include "Managers/ImageManager.hpp"
#include "Managers/InputManager.hpp"
#include "Managers/PuzzleManager.hpp"
#include "GameCore/GameApp.hpp"
#include "Utils/Helper.hpp"
#include "Utils/SpriteBatch.hpp"
#include <set>
#include <mutex>
#include <thread>

namespace WindowManager
{
    static sf::RenderWindow *_pScreen;
    static float _zoomFactor = 1.f;
    static float _minZoomFactor = 0.5f;

    struct RectLess
    {
        bool operator()(const sf::FloatRect lhs_, const sf::FloatRect rhs_)
        {
            return lhs_.left < rhs_.left || (lhs_.left == rhs_.left && lhs_.top < rhs_.top);
        }
    };

    sf::FloatRect getCameraRect();

    void initializeScreen()
    {
        _pScreen = new sf::RenderWindow(sf::VideoMode(GameApp::Options.iScreenWidth, GameApp::Options.iScreenHeight, 32),
                                      "Jigsaw Puzzle");
    }

    void initializeGame()
    {
    }

    void destroy()
    {
    }

    void beginRender()
    {
        _pScreen->clear();
    }


    void endRender()
    {
        _pScreen->display();
    }


    void renderPlayArea()
    {
        _pScreen->clear(sf::Color::Blue);

        // Render only for debug purposes:
        sf::RectangleShape play;
        const sf::FloatRect &area = PuzzleManager::getPlayArea();
        play.setOutlineThickness(3.f);
        play.setPosition(area.left, area.top);
        play.setSize(sf::Vector2f(area.width, area.height));
        play.setOutlineColor(sf::Color::Black);
        play.setFillColor(sf::Color::Transparent);
        _pScreen->draw(play);

        static SpriteBatch spBatch;
        spBatch.reset();

        // Draw the selected pieces at the top.
        const sf::FloatRect cameraRect = getCameraRect();
        for(auto piece: PuzzleManager::getPieceOrderForRender())
        {
            if(piece->getBoundRect().intersects(cameraRect))
            {
                piece->render(spBatch, GameApp::Options._showBorder, sf::Color::Black);
            }
        }

        for(auto piece: InputManager::getSelectedPieces())
        {
            if(piece->getBoundRect().intersects(cameraRect))
            {
                piece->render(spBatch, BorderRenderingMode::eAll, sf::Color::Red);
            }
        }

        spBatch.render(*_pScreen);
    }


    void scroll(const U8 dir, float pixel_)
    {
        sf::Vector2f offset(0.f,0.f);
        pixel_ *= _zoomFactor;
        switch(dir)
        {
            case 0:
                offset.y=-pixel_;
                break;
            case 1:
                offset.x=pixel_;
                break;
            case 2:
                offset.y=pixel_;
                break;
            case 3:
                offset.x=-pixel_;
                break;
        }
        moveCamera(offset);
    }


    void moveCamera(const sf::Vector2f offset_)
    {
        sf::View camera = _pScreen->getView();
        camera.move(offset_);
        _pScreen->setView(camera);
    }


    void zoomIn(int times)
    {
        sf::View camera = _pScreen->getView();
        for(int i=0; i<times; ++i)
        {
            if(_zoomFactor > _minZoomFactor)
            {
                _zoomFactor /= 1.1f;
                camera.zoom(1.f/1.1f);
            }
        }
        _pScreen->setView(camera);
    }


    void zoomOut(int times)
    {
        sf::View camera = _pScreen->getView();
        for(int i=0; i<times; ++i)
        {
            if(_zoomFactor < 10.f)
            {
                _zoomFactor *= 1.1f;
                camera.zoom(1.1f);
            }
        }
        _pScreen->setView(camera);
    }


    void resetView()
    {
        _zoomFactor = 1.f;
        _pScreen->setView(_pScreen->getDefaultView());
    }


    const float getZoomFactor()
    {
        return _zoomFactor;
    }


    void setZoomFactor(const float zoom_)
    {
        _zoomFactor = zoom_;
        sf::View camera = _pScreen->getView();
        camera.zoom(zoom_);
        _pScreen->setView(camera);
    }


    bool pollEvent(sf::Event &evt)
    {
        return _pScreen->pollEvent(evt);
    }


    sf::Vector2i getMousePos()
    {
        return sf::Mouse::getPosition(*_pScreen);
    }


    sf::Vector2f getAbsMousePos()
    {
        sf::Vector2i mp=sf::Mouse::getPosition(*_pScreen);
        return _pScreen->mapPixelToCoords(mp);
    }


    void closeWindow()
    {
        _pScreen->close();
    }

    // Create a new gui using the game screen. This should be called only in GuiManager::loadGui() function.
    std::unique_ptr<tgui::Gui> createGui()
    {
        tgui::Gui *gui = new tgui::Gui(*_pScreen);
        gui->setFont("Fonts/arial.ttf");
        return std::unique_ptr<tgui::Gui>(gui);
    }

    void setMinZoomFactor(const float max_)
    {
        _minZoomFactor = max_;
    }

    sf::FloatRect getCameraRect()
    {
        const sf::View &camera = _pScreen->getView();
        return sf::FloatRect(camera.getCenter() - 0.5f * camera.getSize(), camera.getSize());
    }

    void setCameraCenter(const sf::Vector2f centerPos_)
    {
        sf::View camera = _pScreen->getView();
        camera.setCenter(centerPos_);
        _pScreen->setView(camera);
    }
}
