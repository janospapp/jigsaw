#pragma once

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Utils/Config.hpp"
#include "Managers/FileManager.hpp"

///////////////////////////
//// GameState (Base) /////
///////////////////////////

//A baseclass for every game state:
class GameState
{
public:
	GameState();
    virtual ~GameState();
    virtual bool init();

    //This function will simply call the handleOneEvent, on every sf::Event, what are generated.
    void events();

	//The next three function will be unique for every GameState, so I must overload them.
    virtual void input() = 0;					//This is for constant input (check every frame a key state for example).
    virtual void update(const U32 deltaMilli) = 0;
    virtual void render() const = 0;
    virtual void restore() = 0;
	
    //This function was used in the RTS. I think I don't need it here, but keep it just in case.
    //virtual void remainTime(const U32 deltaMilli);

protected:
	//This will handle the events. The Event function will run a while loop, and always call this function.
	//With this, I can handle events in this baseclass and in inherited classes too. So I won't break the main while loop, and the
	//other hand it can handle common events (implemented in this baseclass) and state specific events too.
	//It returns true, if the event has handled. (So if the baseclass' function handled the event, I won't run the overloaded code.)
    virtual bool handleOneEvent(sf::Event &evt);
private:
    static sf::Vector2i m_vecMousePosBeforeLost;
};


///////////////////////////
////  MainMenuState   /////
///////////////////////////

class MainMenuState: public GameState
{
public:
    ~MainMenuState();
    bool init();

    void input();
    void update(const U32 deltaMilli);
    void render() const;
    void restore();
    void openFileDialog();

private:
    bool handleOneEvent(sf::Event &evt);
    void showExitMsg();

    std::unique_ptr<FileManager::FileDialog> _pOpenDialog;
    bool _isFileDialogOpen;
};


////////////////////////////
////  GameSetupState   /////
////////////////////////////

class GameSetupState: public GameState
{
public:
    GameSetupState(const std::string file_);
    ~GameSetupState();
    bool init();

    void input();
    void update(const U32 deltaMilli);
    void render() const;
    void restore();

private:
    bool handleOneEvent(sf::Event &evt);
    std::string _file;
};


///////////////////////////
////  GamePlayState   /////
///////////////////////////

class GamePlayState: public GameState
{
public:
    ~GamePlayState();
    bool init();

    void input();
    void update(const U32 deltaMilli);
    void render() const;
    void restore();

    bool _changedSinceLastSave;
private:
    bool handleOneEvent(sf::Event &evt);
};
