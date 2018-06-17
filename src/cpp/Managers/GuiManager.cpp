#include "GameCore/GameApp.hpp"
#include "GameCore/GameState.hpp"
#include "Managers/FileManager.hpp"
#include "Managers/GuiManager.hpp"
#include "Managers/PuzzleManager.hpp"
#include "Managers/WindowManager.hpp"
#include "Utils/Debug.hpp"
#include "Utils/Helper.hpp"
#include "TGUI/TGUI.hpp"
#include <utility>


namespace GuiManager
{
    static std::map<U8,std::unique_ptr<tgui::Gui>> _guiMap;
    static tgui::Theme::Ptr _theme;
    static U8 _activeGui = NOGUI;
    static bool isMsgBox = false;

    void init()
    {
        _theme = std::make_shared<tgui::Theme>("Gui/Black.txt");
    }

    bool handleEvent(sf::Event &evt_)
    {
        bool ret = false;
        auto gui = _guiMap.find(_activeGui);
        if(_activeGui != NOGUI && gui != _guiMap.end())
        {
            // There's an active gui so try to handle the event
            ret = (*gui).second->handleEvent(evt_);

            if(isMsgBox)
            {
                // If a message box is shown then don't let the event further (means background is irresponsive)
                // This means there's no other game logic processed till the gui is open.
                ret = true;
            }
        }
        return ret;
    }


    bool activateGui(const U8 id_)
    {
        // The given gui is already activated.
        if(_activeGui == id_)
        {
            return true;
        }

        auto gui = _guiMap.find(id_);
        if(gui == _guiMap.end() && id_ != NOGUI)
        {
            // The gui haven't been added yet.
            return false;
        }
        else
        {
            _activeGui = id_;
            return true;
        }
    }


    void render()
    {
        if(_activeGui != NOGUI)
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


    void addGui(const U8 id_, std::unique_ptr<tgui::Gui> gui_)
    {
        auto it = _guiMap.find(id_);
        if(it == _guiMap.end())
        {
            _guiMap.insert(std::make_pair(id_, std::move(gui_)));
        }
    }

    void removeGui(const U8 id_)
    {
        _guiMap.erase(id_);
    }

    tgui::Gui* getActiveGui()
    {
        auto gui = _guiMap.find(_activeGui);
        return gui->second.get();
    }

    void msgBoxHandler(const std::vector<std::string> &labels,const std::vector<std::function<void()>> &fns,
                       tgui::Gui *gui,tgui::MessageBox::Ptr msgBox, const sf::String &btn)
    {
        for(int i=0; i<labels.size(); ++i)
        {
            if(labels[i] == btn.toAnsiString() && i < fns.size())
            {
                // Call the respective handler function
                fns[i]();
            }
        }

        // Always hide the msgBox after it was clicked
        gui->remove(msgBox);
        isMsgBox = false;
    }

    void addMsgBox(const std::vector<std::string> &labels,const std::vector<std::function<void()>> &fns,
                   std::string &&msg,tgui::Gui *gui)
    {
        tgui::MessageBox::Ptr msgB = _theme->load("MessageBox");
        msgB->setText(sf::String(msg));
        for(auto &str: labels)
        {
            msgB->addButton((sf::String(str)));
        }
        msgB->connect("ButtonPressed",msgBoxHandler,labels,fns,gui,msgB);
        msgB->setTextSize(24);

        sf::Vector2f size = msgB->getSize();
        msgB->setPosition((GameApp::Options.iScreenWidth-size.x)/2.0f,(GameApp::Options.iScreenHeight-size.y)/2.0f);

        gui->add(msgB,"messageBox");
        isMsgBox = true;
    }

    void removeMsgBox()
    {
        tgui::Gui *gui = getActiveGui();
        if(gui != nullptr)
        {
            gui->remove(gui->get("messageBox"));
        }
        isMsgBox = false;
    }

    tgui::Theme *getTheme()
    {
        return _theme.get();
    }
}
