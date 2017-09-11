#ifndef GUI_MANAGER_HPP
#define GUI_MANAGER_HPP

#include <map>
#include "TGUI/Gui.hpp"
#include "Utils/Config.hpp"
#include <memory>

// This class will handle all the guis in the game. Guis are identified with and unsigned int starting from 1.
// 0 means, there's no active gui.
namespace GuiManager
{
    // Pass the event to the active Gui
    bool handleEvent(sf::Event &evt_);

    // This will change the active gui to the given one. Also loads a gui, when it is not loaded yet.
    bool activateGui(const U8 id_);

    // Draws the active gui. Will be called in render part of the gameloop.
    void render();
}




#endif
