#ifndef RENDERSYSTEM_HPP
#define RENDERSYSTEM_HPP

#include "Utils/Config.hpp"
#include "SFML/Graphics.hpp"
#include "TGUI/Gui.hpp"
#include <memory>

namespace WindowManager
{
    void initializeScreen();
    void initializeGame();
    void destroy();

    void beginRender();
    void endRender();
    void renderPlayArea();
    void scroll(const U8 dir, float pixel_);
    void moveCamera(const sf::Vector2f offset_);
    void zoomIn(int times=1);
    void zoomOut(int times=1);
    void resetView();
    void setCameraCenter(const sf::Vector2f centerPos_);
    const float getZoomFactor();
    void setZoomFactor(const float zoom_);
    void setMinZoomFactor(const float max_);

    bool pollEvent(sf::Event &evt);
    sf::Vector2i getMousePos();	//Returns the mouse position on the screen (target coordinates).
    sf::Vector2f getAbsMousePos();	//Returns the mouse position on the play area (world coordinates).
    void closeWindow();

    std::unique_ptr<tgui::Gui> createGui();
}

#endif
