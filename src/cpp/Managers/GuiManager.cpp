#include "GameCore/GameApp.hpp"
#include "Managers/GuiManager.hpp"
#include "Managers/WindowManager.hpp"
#include "Utils/Debug.hpp"
#include "Utils/Helper.hpp"
#include "TGUI/TGUI.hpp"
#include <utility>


namespace GuiManager
{
    // Load the given gui and put it into the map.
    bool loadGui(const U8 id_);

    static std::map<U8,std::unique_ptr<tgui::Gui>> _guiMap;
    static U8 _activeGui = 0;

    bool handleEvent(sf::Event &evt_)
    {
        auto gui = _guiMap.find(_activeGui);
        if(_activeGui == 0 || gui == _guiMap.end())
        {
            return false;
        }
        else
        {
            return (*gui).second->handleEvent(evt_);
        }
    }


    bool activateGui(const U8 id_)
    {
        // The given gui is already activated.
        if(_activeGui == id_)
        {
            return true;
        }

        auto gui = _guiMap.find(id_);
        if(gui == _guiMap.end() && id_ != 0)
        {   // We need to load the gui. If it's 0, then it's not in the map, but we just
            // disable all guis.
            if(loadGui(id_))
            {
                _activeGui = id_;
                return true;
            }
            else
            {   // Loading failed.
                return false;
            }
        }
        else
        {
            _activeGui = id_;
            return true;
        }
    }


    void render()
    {
        if(_activeGui != 0)
        {
            auto current = _guiMap.find(_activeGui);
            if(current != _guiMap.end())
            {
                (*current).second->draw();
            }
            else
            {
                debugPrint("We want to draw an unloaded GUI with id" + _activeGui);
            }
        }
    }


    bool loadGui(const U8 id_)
    {
        std::unique_ptr<tgui::Gui> gui = std::move(WindowManager::createGui());
        gui->setGlobalFont("Fonts/arial.ttf");

        switch(id_)
        {
        case 1:
        {
            tgui::Button::Ptr start = tgui::Button::create("Gui/Black.conf");
            start->setPosition(150.0f, 75.0f);
            start->setSize(270.0f, 50.0f);
            start->connect("pressed",[](){GameApp::PushState(2);});
            start->setText("Start");

            tgui::Button::Ptr exit = tgui::Button::create("Gui/Black.conf");
            exit->setPosition(150.0f, 175.0f);
            exit->setSize(270.0f, 50.0f);
            exit->connect("pressed",[](){GameApp::Exit();});
            exit->setText("Exit");

            gui->add(start);
            gui->add(exit);
        }
            break;
        default:
            debugPrint(std::string("Loading gui with id ") + intToStr(id_) + std::string(" failed."));
            return false;
        }

        _guiMap.insert(std::make_pair(id_, std::move(gui)));
        return true;
    }
}
