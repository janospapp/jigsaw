#include "Managers/WindowManager.hpp"
#include "Managers/InputManager.hpp"
#include "Managers/PuzzleManager.hpp"
#include "GameCore/GameApp.hpp"

namespace WindowManager
{
    static sf::RenderWindow *_pScreen;
    static float _zoomFactor = 1.f;

    void initializeScreen()
    {
        _pScreen = new sf::RenderWindow(sf::VideoMode(GameApp::Options.iScreenWidth, GameApp::Options.iScreenHeight, 32),
                                      "Jigsaw Puzzle");
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
        sf::RectangleShape background(_pScreen->getView().getSize());
        background.setPosition(_pScreen->mapPixelToCoords(sf::Vector2i(0,0)));
        background.setFillColor(sf::Color::Blue);
        _pScreen->draw(background);

        // Get the pieces from the play area.
        for(auto piece: PuzzleManager::getPieceOrderForRender())
        {
            piece->render(*_pScreen, GameApp::Options._showBorder, sf::Color::Black);
        }

        // Draw the selected pieces at the top.
        for(auto piece: InputManager::getSelectedPieces())
        {
            piece->render(*_pScreen, true, sf::Color::Red);
        }
    }


    void scroll(const U8 dir, const float &pixel)
    {
        sf::Vector2f offset(0.f,0.f);
        switch(dir)
        {
            case 0:
                offset.y=-pixel;
                break;
            case 1:
                offset.x=pixel;
                break;
            case 2:
                offset.y=pixel;
                break;
            case 3:
                offset.x=-pixel;
                break;
        }

        sf::View view=_pScreen->getView();
        view.move(offset);
        _pScreen->setView(view);
    }


    void zoomIn(int times)
    {
        sf::View view=_pScreen->getView();
        for(int i=0; i<times; ++i)
        {
            if(_zoomFactor>0.5f)
            {
                _zoomFactor /= 1.1f;
                view.zoom(1.f/1.1f);
            }
        }

        _pScreen->setView(view);
    }


    void zoomOut(int times)
    {
        sf::View view=_pScreen->getView();
        for(int i=0; i<times; ++i)
        {
            if(_zoomFactor<10.f)
            {
                _zoomFactor *= 1.1f;
                view.zoom(1.1f);

            }
        }

        _pScreen->setView(view);
    }


    void resetView()
    {
        _pScreen->setView(_pScreen->getDefaultView());
        _zoomFactor=1.f;
    }


    const float getZoomFactor()
    {
        return _zoomFactor;
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
        return std::unique_ptr<tgui::Gui>(new tgui::Gui(*_pScreen));
    }
}
