#include "GameCore/GameState.hpp"
#include "GameCore/GameApp.hpp"
#include "Managers/ImageManager.hpp"
#include "Managers/InputManager.hpp"
#include "Managers/PuzzleManager.hpp"
#include "Managers/WindowManager.hpp"
#include "Managers/GuiManager.hpp"
#include "Managers/GuiManager.hpp"
#include <Windows.h>



///////////////////////////
//// GameState (Base) /////
///////////////////////////

GameState::GameState()
{
}


GameState::~GameState()
{
}


bool GameState::Init()
{
	return true;
}


void GameState::Events()
{
    sf::Event currevt_;
    while(WindowManager::pollEvent(currevt_))
    {
        HandleOneEvent(currevt_);
	}
}


void GameState::RemainTime(const U32 deltaMilli)
{
    Sleep(deltaMilli);
}


bool GameState::HandleOneEvent(sf::Event &evt_)
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
    GameApp::CleanUp();
}


bool MainMenuState::Init()
{
    GuiManager::activateGui(1);
}


void MainMenuState::Input()
{
}


void MainMenuState::Update(const U32 deltaMilli)
{
}


void MainMenuState::Render() const
{
}


bool MainMenuState::HandleOneEvent(sf::Event &evt_)
{
	bool ret = false;
    if(!GameState::HandleOneEvent(evt_))
    {
		//The baseclass didn't handle the event.
        switch(evt_.type)
        {
		case sf::Event::KeyPressed:
            if(evt_.key.code == sf::Keyboard::Escape)
            GameApp::PopState();
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


///////////////////////////
////  GamePlayState   /////
///////////////////////////

GamePlayState::~GamePlayState()
{
    GuiManager::activateGui(1);

	InputManager::destroy();
    PuzzleManager::destroy();
	WindowManager::resetView();
}


bool GamePlayState::Init()
{
    // Now just disable main menu gui. We need to activate gameplay gui here later.
    GuiManager::activateGui(0);

    PuzzleManager::createPuzzle();
	InputManager::initialize();

	return true;
}


void GamePlayState::Input()
{
}


void GamePlayState::Update(const U32 deltaMilli)
{
    InputManager::gameConstantInput(deltaMilli);
}


void GamePlayState::Render() const
{
	//The order matters, renderPlayArea renders the background, so it must be called first.
	//It could be better if I have a seperate function to draw background, but it's ok now.
    WindowManager::renderPlayArea();
}

void GamePlayState::RemainTime(const U32 deltaMilli)
{
}


bool GamePlayState::HandleOneEvent(sf::Event &evt_)
{
	bool ret = false;
    if(!GameState::HandleOneEvent(evt_))
    {
        sf::Vector2i mouse=WindowManager::getMousePos();
		//The baseclass haven't handled the event.
        switch(evt_.type)
        {
		case sf::Event::KeyPressed:
            if(evt_.key.code == sf::Keyboard::Escape)
            {
                GameApp::PopState();
                ret = true;
            }
            else if(evt_.key.code == sf::Keyboard::B)
            {
                GameApp::Options._showBorder = !GameApp::Options._showBorder;
                ret = true;
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
