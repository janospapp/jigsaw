#include "SFML/Window.hpp"	//For event handling.
#include "GameCore/GameApp.hpp"
#include "GameCore/GameState.hpp"
#include "Utils/INIParser.hpp"
#include "Utils/Random.hpp"
#include "Utils/Debug.hpp"
#include "Managers/WindowManager.hpp"
#include "Managers/GuiManager.hpp"
#include <Windows.h>

int TO::mapTileX;
int TO::mapTileY;
int TO::frameTime;
int TO::maxRenderSkip;
std::string TO::pictureToSolve;
float TO::pieceSize;


void TO::load()
{
    INIParser ini("Temp.ini");
    mapTileX = ini.findValue<int>("Map", "TileX");
    mapTileY = ini.findValue<int>("Map", "TileY");
    frameTime = ini.findValue<int>("Game", "frameTime");
    maxRenderSkip = ini.findValue<int>("Game", "maxRenderSkip");
    pictureToSolve = ini.findValue<std::string>("Game", "picture");
    pieceSize = ini.findValue<float>("Game", "pieceSize");
}


//sf::Vector2i ConvertScreenCoordToMapCoord(sf::Vector2i coordToConvert)
//{
//    sf::Vector2f ret = GameApp::Screen->mapPixelToCoords(coordToConvert, GameApp::g_View);
//    return sf::Vector2i((int)ret.x/TempOptions.mapTileX, (int)ret.y/TempOptions.mapTileY);
//}
//
//
//sf::Vector2i ConvertAbsCoordToMapCoord(sf::Vector2i coordToConvert)
//{
//    return sf::Vector2i(coordToConvert.x/TempOptions.mapTileX, coordToConvert.y/TempOptions.mapTileY);
//}


void GameApp::s_Options::loadOptions()
{
    INIParser ip("Options.ini");
	
    iScreenWidth = ip.findValue<int>("Screen", "Width");
    iScreenHeight = ip.findValue<int>("Screen", "Height");
    fScrollSpeed = ip.findValue<float>("Game", "ScrollSpeed");
    _showBorder = ip.findValue<int>("Game", "ShowPieceBorder") == 1;
    fScaleX = iScreenWidth/1366.f;
    fScaleY = iScreenHeight/768.f;
}


void GameApp::s_Options::saveOptions()
{
	std::ofstream fout;
    fout.open("Options.ini");

	fout << "[Screen]" << std::endl << std::endl << "Width=" << iScreenWidth  << std::endl;
	fout << "Height=" << iScreenHeight << std::endl << std::endl;
	fout << "[Game]" << std::endl << std::endl << "ScrollSpeed=" << fScrollSpeed << std::endl;
    fout << "ShowPieceBorder=" << (_showBorder ? 1 : 0) << std::endl;

	fout.close();
}


void GameApp::InitializeGame()
{
	//Load the options, and the menus from files.
    Options.loadOptions();
    TempOptions.load();

	//Initialize random number generator.
	Random::initialize();

	//Create the game window, and set the view properly.
	WindowManager::initializeScreen();
	
	//Create a new GameClock.
	m_pClock = new GameClock();
    PushState(new MainMenuState());

    m_pStateForDelete = nullptr;
}


void GameApp::Gameloop()
{
    while(!m_StateStack.empty())
    {
		m_bStateChanged = false;

		m_pClock->Reset();
		m_StateStack.top()->Events();	//This is the first function where the state can change, so I don't need to check it before the call.
		
        if(!m_bStateChanged)
        {
			m_StateStack.top()->Input();
		}

        if(!m_bStateChanged)
        {
#ifdef _DEBUG
		int time = m_pClock->GetUpdateTime();
        if(time > TempOptions.frameTime)
        {
			//If I stopped at a breakpoint, then I measure a very large updatetime, so I make it correct here.
            m_StateStack.top()->Update(TempOptions.frameTime);
        }
        else
        {
            m_StateStack.top()->Update(time);
		}
#else
        m_StateStack.top()->Update(m_pClock->GetUpdateTime());
#endif
		}

        if(m_pClock->CanRender() && !m_bStateChanged)
        {
			WindowManager::beginRender();
			m_StateStack.top()->Render();
            GuiManager::render();
			WindowManager::endRender();
		}

		int milliSecRemained = m_pClock->GetRemainTime();
        if(!m_bStateChanged && milliSecRemained > 0)
        {
            m_StateStack.top()->RemainTime(milliSecRemained);
		}

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
    if(state != nullptr && state->Init())
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

	//CleanUp(); The main menu state is always at the bottom of the stack, and its destroy will call CleanUp.
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
