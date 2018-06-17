#ifndef GUI_MANAGER_HPP
#define GUI_MANAGER_HPP

#include <map>
#include "TGUI/Gui.hpp"
#include "Utils/Config.hpp"
#include <memory>

const U8 NOGUI = 0;
const U8 MAINMENUGUI = 1;
const U8 GAMESETUPGUI = 2;
const U8 GAMEPLAYGUI = 3;


// This class will handle all the guis in the game. Guis are identified with and unsigned int starting from 1.
// 0 means, there's no active gui.
namespace GuiManager
{
    void init();

    // Pass the event to the active Gui
    bool handleEvent(sf::Event &evt_);

    // This will change the active gui to the given one. Also loads a gui, when it is not loaded yet.
    bool activateGui(const U8 id_);

    void addGui(const U8 id_, std::unique_ptr<tgui::Gui> gui_);
    void removeGui(const U8 id_);

    // Draws the active gui. Will be called in render part of the gameloop.
    void render();

    tgui::Gui *getActiveGui();

    void addMsgBox(const std::vector<std::string> &labels, const std::vector<std::function<void()>> &fns,
                   std::string &&msg, tgui::Gui *gui = getActiveGui());

    void removeMsgBox();


    tgui::Theme *getTheme();
}




#endif
