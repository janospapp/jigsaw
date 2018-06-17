#include "GameCore/GameApp.hpp"
#include "Managers/FileManager.hpp"
#include "Managers/GuiManager.hpp"
#include "Utils/Helper.hpp"
#include "TGUI/TGUI.hpp"
#include "boost/filesystem.hpp"
#include <vector>
#include <memory>
#include <fstream>
#include <io.h>

namespace FileManager{
    FileDialog::FileDialog(std::vector<std::string> ext_, const bool showDirs_):
        _file(""), _extensions(ext_), _includeDirs(showDirs_), _isReady(false)
    {
        _dialogWindow = GuiManager::getTheme()->load("ChildWindow");
        initialize();
    }

    FileDialog::~FileDialog()
    {
        _dialogWindow->destroy();
    }

    bool FileDialog::isReady() const
    {
        return _isReady;
    }

    std::string FileDialog::getValue() const
    {
        return _file;
    }

    void FileDialog::initialize()
    {
        tgui::Gui *gui = GuiManager::getActiveGui();
        _dialogWindow->setTitle("Open image");
        _dialogWindow->setSize(tgui::Layout2d(sf::Vector2f(600.f,400.f)));
        float posX = (gui->getSize().x-_dialogWindow->getSize().x)/2.f;
        float posY = (gui->getSize().y-_dialogWindow->getSize().y)/2.f;
        _dialogWindow->setPosition(tgui::Layout2d(sf::Vector2f(posX,posY)));

        update(GameApp::Options._defaultDirectory);

        _dialogWindow->connect("Closed",&FileDialog::close,this);
        gui->add(_dialogWindow);
    }

    void FileDialog::update(const std::string &path_)
    {
        // First remove all widgets from the window. There are buttons that are fix, but it's easier to
        // remove them as well and readd them, than remove only non-constant elements. Actually it would
        // be enough to update list items, and the path-buttons. I can improve the code with this if it
        // will turn out to be slow.
        _dialogWindow->removeAllWidgets();
        _currentDirContent.clear();
        tgui::ListBox::Ptr dirContentList = GuiManager::getTheme()->load("ListBox");
        boost::filesystem::path boostDir(path_);
        if(_includeDirs && boostDir.relative_path().string() != "")
        {
            // Don't include .. into root directory.
            dirContentList->addItem("..");
            _currentDirContent.push_back("..");
        }

        boost::system::error_code dir_err;
        for(boost::filesystem::directory_iterator it(boostDir,dir_err); it != boost::filesystem::directory_iterator();
            it.increment(dir_err))
        {
            if(dir_err.value() == 0)
            {
                bool isFileAllowed = true;
                if(!_extensions.empty())
                {
                    //Check the allowed extensions
                    isFileAllowed = find(_extensions.begin(),_extensions.end(),toLower(it->path().extension().string()))
                            != _extensions.end();
                }

                if((_includeDirs && boost::filesystem::is_directory(it->path())) || isFileAllowed)
                {
                    std::string file = it->path().filename().string();
                    dirContentList->addItem(file);
                    _currentDirContent.push_back(file);
                }
            }
        }

        _selectedItem = _currentDirContent.begin();
        dirContentList->setSelectedItem(*_selectedItem);

        _dialogWindow->add(dirContentList,"fileList");
        dirContentList->setPosition(0.0, 0.2 * tgui::bindHeight(_dialogWindow));
        dirContentList->setSize(0.7 * tgui::bindWidth(_dialogWindow), 0.8 * tgui::bindHeight(_dialogWindow));
        dirContentList->connect("DoubleClicked",&FileDialog::handleDialogList,this);

        // Add buttons for easier path navigation, like in windows explorer.
        const float pathLength = path_.length() - countChars(path_,{'/','\\'}) + 1; // +1 for root.
        float buttonStart = 0.f;

        {   // Add root path (it needs special handling on windows, because it's composed by "C:" + "/".
            tgui::Button::Ptr rootButton = GuiManager::getTheme()->load("Button");
            const float relativeWidth = boostDir.root_path().string().length() / pathLength;
            _dialogWindow->add(rootButton);
            rootButton->setText(boostDir.root_path().string());
            rootButton->setSize(0.7f*relativeWidth*tgui::bindWidth(_dialogWindow),0.1f*tgui::bindHeight(_dialogWindow));
            rootButton->setPosition(buttonStart*tgui::bindWidth(_dialogWindow),0.1f*tgui::bindHeight(_dialogWindow));
            buttonStart += 0.7*relativeWidth;
            rootButton->connect("Pressed",&FileDialog::handleDialogButton,this);

        }

        for(auto &it: boostDir.relative_path())
        {
            // Add the remaining path.
            tgui::Button::Ptr pathButton = GuiManager::getTheme()->load("Button");
            const float relativeWidth = it.string().length() / pathLength;
            pathButton->setText(it.string());
            pathButton->setSize(0.7f*relativeWidth*tgui::bindWidth(_dialogWindow),0.1f*tgui::bindHeight(_dialogWindow));
            pathButton->setPosition(buttonStart*tgui::bindWidth(_dialogWindow),0.1f*tgui::bindHeight(_dialogWindow));
            buttonStart += 0.7*relativeWidth;
            pathButton->connect("Pressed",&FileDialog::handleDialogButton,this);
            _dialogWindow->add(pathButton);
        }

        // Add Open and Cancel buttons.
        tgui::Button::Ptr openButton = GuiManager::getTheme()->load("Button");
        openButton->setText("Open");
        openButton->setSize(0.25f*tgui::bindWidth(_dialogWindow),0.1f*tgui::bindHeight(_dialogWindow));
        openButton->setPosition(0.725*tgui::bindWidth(_dialogWindow),0.3*tgui::bindHeight(_dialogWindow));
        openButton->connect("Pressed",&FileDialog::handleDialogButton,this);
        _dialogWindow->add(openButton);

        tgui::Button::Ptr cancelButton = GuiManager::getTheme()->load("Button");
        cancelButton->setText("Cancel");
        cancelButton->setSize(0.25f*tgui::bindWidth(_dialogWindow),0.1f*tgui::bindHeight(_dialogWindow));
        cancelButton->setPosition(0.725*tgui::bindWidth(_dialogWindow),0.45*tgui::bindHeight(_dialogWindow));
        cancelButton->connect("Pressed",&FileDialog::close,this);
        _dialogWindow->add(cancelButton);

        _pathToShow = boost::filesystem::path(path_);
    }


    void FileDialog::close()
    {
        _dialogWindow->destroy();
        _file = "";
        _isReady = true;
    }

    void FileDialog::handleKeyEvent(const sf::Event keyEvent_)
    {
        tgui::ListBox::Ptr dirContentList = std::static_pointer_cast<tgui::ListBox>(_dialogWindow->get("fileList"));
        switch(keyEvent_.key.code)
        {
        case sf::Keyboard::Up:
            if(_selectedItem == _currentDirContent.begin())
            {
                _selectedItem = _currentDirContent.end() - 1;
            }
            else
            {
                --_selectedItem;
            }
            dirContentList->setSelectedItem(*_selectedItem);
            break;
        case sf::Keyboard::Down:
            if(_selectedItem == _currentDirContent.end())
            {
                _selectedItem = _currentDirContent.begin();
            }
            else
            {
                ++_selectedItem;
            }
            dirContentList->setSelectedItem(*_selectedItem);
            break;
            break;
        case sf::Keyboard::Return:
            handleDialogList(*_selectedItem);
            break;
        case sf::Keyboard::BackSpace:
            if(boost::filesystem::path(_pathToShow).relative_path().string() != "")
            {
                update(_pathToShow.parent_path().string());
            }
            break;
        }
    }


    void FileDialog::handleDialogButton(const std::string &caption_)
    {
        if(caption_ == "Open")
        {
            tgui::Widget::Ptr widget = _dialogWindow->get("fileList");
            tgui::ListBox::Ptr list = std::static_pointer_cast<tgui::ListBox>(widget);
            finalizeWithFilename(list->getSelectedItem().toAnsiString());
        }
        else
        {
            std::string resultDir;

            // A path button was pressed.
            boost::filesystem::path rootPath = boost::filesystem::path(_pathToShow).root_path();
            if(caption_ == rootPath.string())
            {
                resultDir = caption_;
            }
            else
            {
                boost::filesystem::path newPath;
                for(auto &it: _pathToShow)
                {
                    // Adding current dir to the new path.
                    newPath /= it;
                    if(it.string() == caption_)
                    {
                        // Stop if we found the clicked dir name.
                        resultDir = newPath.string();
                        break;
                    }
                }
            }
            update(resultDir);
        }
    }


    void FileDialog::handleDialogList(const std::string &item_)
    {
        finalizeWithFilename(item_);
    }


    void FileDialog::finalizeWithFilename(const std::string &filename_)
    {
        if(filename_ == "..")
        {
            update(_pathToShow.parent_path().string());
        }
        else
        {
            boost::filesystem::path fullPath = _pathToShow / boost::filesystem::path(filename_);
            if(boost::filesystem::is_directory(fullPath))
            {
                update(fullPath.string());
            }
            else
            {
                _file = fullPath.string();
                _isReady = true;
                _dialogWindow->destroy();
            }
        }
    }
}
