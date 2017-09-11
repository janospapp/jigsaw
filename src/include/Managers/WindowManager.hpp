#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "Utils/Config.hpp"
#include "SFML/Graphics.hpp"
#include "TGUI/Gui.hpp"
#include <memory>

namespace WindowManager
{
    void initializeScreen();

    void beginRender();	//This function clears the previous screen.
    void endRender();	//This function displays the actual screen.
    void renderPlayArea();
    void scroll(const U8 dir, const float &pixel);
    void zoomIn(int times=1);
    void zoomOut(int times=1);
    void resetView();
    const float getZoomFactor();

    bool pollEvent(sf::Event &evt);
    sf::Vector2i getMousePos();	//Returns the mouse position on the screen (target coordinates).
    sf::Vector2f getAbsMousePos();	//Returns the mouse position on the play area (world coordinates).
    void closeWindow();

    std::unique_ptr<tgui::Gui> createGui();
}

#endif
