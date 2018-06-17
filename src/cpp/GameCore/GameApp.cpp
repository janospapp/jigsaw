#include "SFML/Window.hpp"	//For event handling.
#include "GameCore/GameApp.hpp"
#include "GameCore/GameState.hpp"
#include "Utils/INIParser.hpp"
#include "Utils/Random.hpp"
#include "Utils/Debug.hpp"
#include "Managers/WindowManager.hpp"
#include "Managers/GuiManager.hpp"
#include <Windows.h>

int TO::frameTime;
int TO::maxRenderSkip;

void TO::load()
{
    INIParser ini("Temp.ini");
    frameTime = ini.findValue<int>("Game", "frameTime");
    maxRenderSkip = ini.findValue<int>("Game", "maxRenderSkip");
}


void GameApp::s_Options::loadOptions()
{
    INIParser ip("Options.ini");
	
    iScreenWidth = ip.findValue<int>("Screen", "Width");
    iScreenHeight = ip.findValue<int>("Screen", "Height");
    fScrollSpeed = ip.findValue<float>("Game", "ScrollSpeed");
    _defaultDirectory = ip.findValue<std::string>("Game", "DefaultDir");
    int borderMode = ip.findValue<int>("Game", "ShowPieceBorder");
    _showBorder = static_cast<BorderRenderingMode>(borderMode);
    fScaleX = iScreenWidth/1366.f;
    fScaleY = iScreenHeight/768.f;
}


void GameApp::s_Options::saveOptions()
{
	std::ofstream fout;
    fout.open("Options.ini");

	fout << "[Screen]" << std::endl << std::endl << "Width=" << iScreenWidth  << std::endl;
	fout << "Height=" << iScreenHeight << std::endl << std::endl;
    fout << "[Game]" << std::endl << std::endl << "ScrollSpeed=" << fScrollSpeed;
    fout << "DefaultDir=" << _defaultDirectory << std::endl;
    fout << "ShowPieceBorder=" << _showBorder << std::endl;

	fout.close();
}


void GameApp::initializeGame()
{
	//Load the options, and the menus from files.
    Options.loadOptions();
    TempOptions.load();

	//Create the game window, and set the view properly.
	WindowManager::initializeScreen();
	
	//Create a new GameClock.
	m_pClock = new GameClock();
    GuiManager::init();
    PushState(new MainMenuState());

    m_pStateForDelete = nullptr;
}


void GameApp::Gameloop()
{
    while(!m_StateStack.empty())
    {
		m_bStateChanged = false;

		m_pClock->Reset();
        m_StateStack.top()->events();	//This is the first function where the state can change, so I don't need to check it before the call.
		
        if(!m_bStateChanged)
        {
            m_StateStack.top()->input();
		}

        if(!m_bStateChanged)
        {
#ifdef _DEBUG
		int time = m_pClock->GetUpdateTime();
        if(time > TempOptions.frameTime)
        {
			//If I stopped at a breakpoint, then I measure a very large updatetime, so I make it correct here.
            m_StateStack.top()->update(TempOptions.frameTime);
        }
        else
        {
            m_StateStack.top()->update(time);
		}
#else
        m_StateStack.top()->update(m_pClock->GetUpdateTime());
#endif
		}

        if(m_pClock->CanRender() && !m_bStateChanged)
        {
			WindowManager::beginRender();
            m_StateStack.top()->render();
            GuiManager::render();
			WindowManager::endRender();
		}

        /*
        int milliSecRemained = m_pClock->GetRemainTime();
        if(!m_bStateChanged && milliSecRemained > 0)
        {
            m_StateStack.top()->remainTime(milliSecRemained);
		}
        */

        if(m_pStateForDelete != nullptr)
        {
			delete m_pStateForDelete;
            m_pStateForDelete = nullptr;
		}
	}
}


GameState* GameApp::GetState()
{
    if(m_StateStack.empty())
    {
        return nullptr;
    }
    else
    {
        return m_StateStack.top();
	}
}


void GameApp::PushState(GameState *state)
{
    if(state != nullptr && state->init())
    {
        m_StateStack.push(state);
		m_bStateChanged = true;
	}
}


void GameApp::PushState(const U8 id_)
{
    GameState *newState = nullptr;
    switch(id_)
    {
    case 1:
        newState = new MainMenuState();
        break;
    case 2:
        newState = new GameSetupState("");
        break;
    case 3:
        newState = new GamePlayState();
        break;
    }

    PushState(newState);
}


void GameApp::PopState()
{
    if(!m_StateStack.empty())
    {
		m_pStateForDelete = m_StateStack.top();
		m_StateStack.pop();
        if(!m_StateStack.empty())
        {
            m_StateStack.top()->restore();
        }
	}

	m_bStateChanged = true;
}


void GameApp::Exit()
{
    while(!m_StateStack.empty())
    {
		PopState();
		delete m_pStateForDelete;
        m_pStateForDelete = nullptr;
	}
}


void GameApp::CleanUp()
{
	delete m_pClock;
	WindowManager::closeWindow();
}


GameApp::s_Options GameApp::Options;
std::stack<GameState*> GameApp::m_StateStack;
GameState *GameApp::m_pStateForDelete = nullptr;
GameClock *GameApp::m_pClock = nullptr;
bool GameApp::m_bStateChanged = false;
bool GameApp::bIsAppActive = true;
