#ifndef FILEMANAGER_HPP
#define FILEMANAGER_HPP

#include "TGUI/TGUI.hpp"
#include "boost/filesystem.hpp"
#include <string>
#include <vector>

namespace FileManager{
    class FileDialog
    {
    public:
        FileDialog(std::vector<std::string> ext_, const bool showDirs_ = true);
        ~FileDialog();
        void update(const std::string &path_);
        void close();
        void handleKeyEvent(const sf::Event keyEvent_);
        bool isReady() const;
        std::string getValue() const;

    private:
        void initialize();
        void handleDialogButton(const std::string &caption_);
        void handleDialogList(const std::string &item_);
        void finalizeWithFilename(const std::string &filename_);

        tgui::ChildWindow::Ptr _dialogWindow;
        boost::filesystem::path _pathToShow;
        std::string _file;
        std::vector<std::string> _extensions;
        std::vector<std::string> _currentDirContent;
        std::vector<std::string>::iterator _selectedItem;
        const bool _includeDirs;
        bool _isReady;
    };


}

#endif
