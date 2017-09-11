#pragma once

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"
#include "Utils/Config.hpp"

///////////////////////////
//// GameState (Base) /////
///////////////////////////

//A baseclass for every game state:
class GameState
{
public:
	GameState();
    virtual ~GameState();
    virtual bool Init();

	//This function will simply call the HandleOneEvent, on every sf::Event, what are generated.
	void Events();

	//The next three function will be unique for every GameState, so I must overload them.
	virtual void Input() = 0;					//This is for constant input (check every frame a key state for example).
    virtual void Update(const U32 deltaMilli) = 0;
	virtual void Render() const = 0;
	
	//This function will decide what do I do in the remained time of the frame. In the main menu this function doesn't do anything, but
	//in the gameplay this will update the A* algorithm, the AI etc. It doesn't do anything by default.
    virtual void RemainTime(const U32 deltaMilli);

protected:
	//This will handle the events. The Event function will run a while loop, and always call this function.
	//With this, I can handle events in this baseclass and in inherited classes too. So I won't break the main while loop, and the
	//other hand it can handle common events (implemented in this baseclass) and state specific events too.
	//It returns true, if the event has handled. (So if the baseclass' function handled the event, I won't run the overloaded code.)
    virtual bool HandleOneEvent(sf::Event &evt);
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
    bool Init();

	void Input();
    void Update(const U32 deltaMilli);
	void Render() const;

private:
    bool HandleOneEvent(sf::Event &evt);
	//This is only for testing and temporary store the pieces' picture.
	sf::Texture *forTest;
};


///////////////////////////
////  GamePlayState   /////
///////////////////////////

class GamePlayState: public GameState
{
public:
    ~GamePlayState();
    bool Init();

	void Input();
    void Update(const U32 deltaMilli);
	void Render() const;

    void RemainTime(const U32 deltaMilli);
private:
    bool HandleOneEvent(sf::Event &evt);
};
