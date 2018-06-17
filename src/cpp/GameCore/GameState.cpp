#include "GameCore/GameState.hpp"
#include "GameCore/GameApp.hpp"
#include "Managers/ImageManager.hpp"
#include "Managers/InputManager.hpp"
#include "Managers/PuzzleManager.hpp"
#include "Managers/WindowManager.hpp"
#include "Managers/GuiManager.hpp"
#include "Utils/Helper.hpp"
#include <Windows.h>

#undef MessageBox
#undef MessageBoxA
#undef MessageBoxW

#include "TGUI/TGUI.hpp"

///////////////////////////
//// GameState (Base) /////
///////////////////////////

GameState::GameState()
{
}


GameState::~GameState()
{
}


bool GameState::init()
{
	return true;
}


void GameState::events()
{
    sf::Event currevt_;
    while(WindowManager::pollEvent(currevt_))
    {
        handleOneEvent(currevt_);
	}
}

/*
void GameState::remainTime(const U32 deltaMilli)
{
    Sleep(deltaMilli);
}
*/

bool GameState::handleOneEvent(sf::Event &evt_)
{
	bool ret = false;
    switch( evt_.type )
    {
	case sf::Event::Closed:
		//This will properly exit the game (destroy all the states).
        GameApp::Exit();
		ret = true;
		break;
	case sf::Event::LostFocus:
        //I release the mouse here, and Push an "empty" state, which receive but don't handle events.
        GameApp::bIsAppActive = false;
		ret = true;
		break;
    case sf::Event::GainedFocus:
        GameApp::bIsAppActive = true;
        ret = true;
        break;
	}

    // If none of the above events happened, I sent the event to the Gui.
    if(!ret)
    {
        ret = GuiManager::handleEvent(evt_);
    }

	return ret;
}

sf::Vector2i GameState::m_vecMousePosBeforeLost;

///////////////////////////
////  MainMenuState   /////
///////////////////////////

MainMenuState::~MainMenuState()
{
    //GuiManager::removeGui(MAINMENUGUI);
    GameApp::CleanUp();
}


bool MainMenuState::init()
{
    std::unique_ptr<tgui::Gui> gui = std::move(WindowManager::createGui());

    tgui::Button::Ptr start = GuiManager::getTheme()->load("Button");
    start->setPosition(150.0f, 75.0f);
    start->setSize(270.0f, 50.0f);
    start->setText("Start");
    start->connect("Pressed",&MainMenuState::openFileDialog,this);

    tgui::Button::Ptr load = GuiManager::getTheme()->load("Button");
    load->setPosition(150.0f, 175.0f);
    load->setSize(270.0f, 50.0f);
    load->connect("pressed",[](){GameApp::PushState(3);
                                 PuzzleManager::loadPuzzle();});
    load->setText("Load game");

    tgui::Button::Ptr exit = GuiManager::getTheme()->load("Button");
    exit->setPosition(150.0f, 275.0f);
    exit->setSize(270.0f, 50.0f);
    exit->setText("Exit");

    exit->connect("pressed",&showExitMsg,this);

    gui->add(start);
    gui->add(load);
    gui->add(exit);
    GuiManager::addGui(MAINMENUGUI,std::move(gui));
    GuiManager::activateGui(MAINMENUGUI);

    return true;
}


void MainMenuState::input()
{
}


void MainMenuState::update(const U32 deltaMilli)
{
    if(_isFileDialogOpen)
    {
        if(_pOpenDialog->isReady())
        {
            std::string file = _pOpenDialog->getValue();
            if(!file.empty())
            {
                GameApp::PushState(new GameSetupState(file));
            }
            _isFileDialogOpen = false;
        }
    }
}


void MainMenuState::render() const
{
}


void MainMenuState::restore()
{
    GuiManager::activateGui(MAINMENUGUI);
}


bool MainMenuState::handleOneEvent(sf::Event &evt_)
{
	bool ret = false;
    if(!GameState::handleOneEvent(evt_))
    {
		//The baseclass didn't handle the event.
        switch(evt_.type)
        {
		case sf::Event::KeyPressed:
            if(evt_.key.code == sf::Keyboard::Escape)
            {
                if(_isFileDialogOpen)
                {
                    _pOpenDialog->close();
                    _isFileDialogOpen = false;
                }
                else
                {
                    showExitMsg();
                }
            }
            else if(_isFileDialogOpen)
            {
                _pOpenDialog->handleKeyEvent(evt_);
            }
            ret = true;
            break;
		}
    }
    else
    {
		ret = true;
	}

	return ret;
}


void MainMenuState::openFileDialog()
{
    _pOpenDialog = std::move(std::unique_ptr<FileManager::FileDialog>(new FileManager::FileDialog(
        {".jpg",".jpeg",".png",".bmp",".pic"})));
    _isFileDialogOpen = true;
}


void MainMenuState::showExitMsg()
{
    const std::vector<std::string> labels = {std::string("Yes"),std::string("No")};
    const std::vector<std::function<void()>> fns{GameApp::Exit};
    GuiManager::addMsgBox(labels,fns,"Are you sure you want to quit?");
}

////////////////////////////
////  GameSetupState   /////
////////////////////////////

GameSetupState::GameSetupState(const std::string file_):
    _file(file_)
{
}

GameSetupState::~GameSetupState()
{
    GuiManager::removeGui(GAMESETUPGUI);
}

bool GameSetupState::init()
{
    std::unique_ptr<tgui::Gui> gui = std::move(WindowManager::createGui());

    tgui::Label::Ptr nameLabel = GuiManager::getTheme()->load("Label");
    nameLabel->setPosition(30.f,75.f);
    nameLabel->setTextSize(20);
    nameLabel->setText("Picture:");

    tgui::EditBox::Ptr nameBox = GuiManager::getTheme()->load("EditBox");
    nameBox->setPosition(150.f,75.f);
    nameBox->setSize(350.f, 50.f);
    nameBox->setText(_file);
    nameBox->setText(boost::filesystem::path(_file).stem().string());

    tgui::Label::Ptr sizeLabel = GuiManager::getTheme()->load("Label");
    sizeLabel->setPosition(30.f, 145.f);
    sizeLabel->setTextSize(20);
    sizeLabel->setText("Number of pieces:");

    tgui::EditBox::Ptr sizeBox = GuiManager::getTheme()->load("EditBox");
    sizeBox->setPosition(400.f,145.f);
    sizeBox->setSize(100.f,50.f);
    sizeBox->setText("90");
    sizeBox->setNumbersOnly(true);

    tgui::Button::Ptr backButton = GuiManager::getTheme()->load("Button");
    backButton->setPosition(400.f,215.f);
    backButton->setSize(200.f,50.f);
    backButton->setText("Back");
    backButton->connect("Pressed",&GameApp::PopState);

    tgui::Button::Ptr startButton = GuiManager::getTheme()->load("Button");
    startButton->setPosition(150.f,215.f);
    startButton->setSize(200.f,50.f);
    startButton->setText("Start");
    auto func = [](const std::string &filePath_, tgui::EditBox *name_, tgui::EditBox *size_)
    {
        PuzzleManager::createPuzzle(filePath_,name_->getText().toAnsiString(),strToInt(size_->getText().toAnsiString()));
        GameApp::PushState(3);
    };
    auto startFunc = std::bind(func,_file,nameBox.get(),sizeBox.get());
    startButton->connect("Pressed",startFunc);
    nameBox->connect("ReturnKeyPressed",startFunc);
    sizeBox->connect("ReturnKeyPressed",startFunc);

    gui->add(nameLabel);
    gui->add(nameBox);
    gui->add(sizeLabel);
    gui->add(sizeBox);
    gui->add(startButton);
    gui->add(backButton);

    GuiManager::addGui(GAMESETUPGUI,std::move(gui));
    GuiManager::activateGui(GAMESETUPGUI);

    return true;
}

void GameSetupState::input()
{
}


void GameSetupState::update(const U32 deltaMilli)
{
}

void GameSetupState::render() const
{
}


void GameSetupState::restore()
{
    GuiManager::activateGui(GAMESETUPGUI);
}


bool GameSetupState::handleOneEvent(sf::Event &evt_)
{
    bool ret = false;
    if(!GameState::handleOneEvent(evt_))
    {
        //The baseclass haven't handled the event.
        switch(evt_.type)
        {
        case sf::Event::KeyPressed:
            if(evt_.key.code == sf::Keyboard::Escape)
            {
                GameApp::PopState();
                ret = true;
            }
            break;
        }
    }
    else
    {
        ret = true;
    }

    return ret;
}


///////////////////////////
////  GamePlayState   /////
///////////////////////////

GamePlayState::~GamePlayState()
{
    InputManager::destroy();
    PuzzleManager::destroy();
    WindowManager::destroy();
    WindowManager::resetView();
    GuiManager::removeGui(GAMEPLAYGUI);
}


bool GamePlayState::init()
{
    // I need an empty gui for showing message boxes.
    GuiManager::addGui(GAMEPLAYGUI,WindowManager::createGui());
    GuiManager::activateGui(GAMEPLAYGUI);
    InputManager::initialize();
    WindowManager::resetView();
    WindowManager::initializeGame();

    _changedSinceLastSave = false;

	return true;
}


void GamePlayState::input()
{
}


void GamePlayState::update(const U32 deltaMilli)
{
    InputManager::gameConstantInput(deltaMilli);
}


void GamePlayState::render() const
{
	//The order matters, renderPlayArea renders the background, so it must be called first.
	//It could be better if I have a seperate function to draw background, but it's ok now.
    WindowManager::renderPlayArea();
}


void GamePlayState::restore()
{
    GuiManager::activateGui(NOGUI);
}

/*
void GamePlayState::remainTime(const U32 deltaMilli)
{
}
*/

bool GamePlayState::handleOneEvent(sf::Event &evt_)
{
	bool ret = false;
    if(!GameState::handleOneEvent(evt_))
    {
        sf::Vector2i mouse=WindowManager::getMousePos();

        auto yesFn = [](){PuzzleManager::savePuzzle();GameApp::PopState();};
        auto noFn = [](){GameApp::PopState();};
        typedef std::vector<std::function<void()>> FuncVector;

        const FuncVector handlers = {yesFn,noFn};
        const std::vector<std::string> labels = {"Yes","No","Cancel"};

		//The baseclass haven't handled the event.
        switch(evt_.type)
        {
        case sf::Event::KeyPressed:
            switch(evt_.key.code)
            {
            case sf::Keyboard::Escape:
                if(_changedSinceLastSave)
                {
                    GuiManager::addMsgBox(labels,handlers,"Do you want to save your puzzle before quit?");
                }
                else
                {
                    GuiManager::addMsgBox(std::vector<std::string>{"Yes","No"},FuncVector{noFn},
                                          "Are you sure you want to quit?");
                }
                ret = true;
                break;
            case sf::Keyboard::B:
                if(GameApp::Options._showBorder == eAll)
                {
                    GameApp::Options._showBorder = eNone;
                }
                else
                {
                    GameApp::Options._showBorder = (BorderRenderingMode)(GameApp::Options._showBorder + 1);
                }
                ret = true;
                break;
            case sf::Keyboard::S:
                if(_changedSinceLastSave)
                {
                    PuzzleManager::savePuzzle();
                    _changedSinceLastSave = false;
                }
                ret = true;
                break;
            case sf::Keyboard::L:
                PuzzleManager::loadPuzzle();
                _changedSinceLastSave = false;
                ret = true;
                break;
            case sf::Keyboard::C:
                WindowManager::setCameraCenter(sf::Vector2f(0.f, 0.f));
                ret = true;
                break;
            case sf::Keyboard::E:
                InputManager::selectEdgePieces();
                ret = true;
                break;
            }
            break;
        case sf::Event::MouseButtonPressed:
            if(mouse.x >= 0 && mouse.x <= GameApp::Options.iScreenWidth && mouse.y >= 0
                    && mouse.y <= GameApp::Options.iScreenHeight)
            {   
                if(evt_.mouseButton.button == sf::Mouse::Left)
                {
                    InputManager::gameLeftClick();
                }
                else if(evt_.mouseButton.button == sf::Mouse::Right)
                {
                    InputManager::gameRightClick();
                }
            }
            ret = true;
            break;
        case sf::Event::MouseButtonReleased:
            if(mouse.x >= 0 && mouse.x <= GameApp::Options.iScreenWidth && mouse.y >= 0
                    && mouse.y <= GameApp::Options.iScreenHeight)
            {
                if(evt_.mouseButton.button == sf::Mouse::Left)
                {
                    InputManager::gameLeftReleased();
                }
            }
			ret = true;
			break;
        case sf::Event::MouseWheelMoved:
            InputManager::gameMouseWheel(evt_.mouseWheel.delta);
            break;
		}
    }
    else
    {
		ret = true;
	}

	return ret;
}
