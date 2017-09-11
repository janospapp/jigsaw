#ifndef GAMEAPP_HPP
#define GAMEAPP_HPP
#include "GameCore/GameClock.hpp"
#include "SFML/Graphics.hpp"
#include <stack>
#include <memory>

static struct TO
{
	static int mapTileX;	//Width of a map tile in pixels
	static int mapTileY;	//Height of a map tile in pixels
	static int frameTime;	//How long is one frame.
	static int maxRenderSkip;	//How many rendering can I skip maximum.
    static std::string pictureToSolve;  //The path for the picture.
    static float pieceSize;   //How big a piece should be.

    void load();
} TempOptions;

//These will be useful when I've got zones for caching. That mechanism would be the same as maptiles in the RTS.
//This function converts the given screen coordinate to mapTile coordinate. I need this for determine which tile the mouse is above.
//sf::Vector2i ConvertScreenCoordToMapCoord(sf::Vector2i coordToConvert);
//
//This is the same as the above function, except it wait for absolute coordinates.
//sf::Vector2i ConvertAbsCoordToMapCoord(sf::Vector2i coordToConvert);

class GameState;

class GameApp
{
public:
    struct s_Options
    {
		int iScreenHeight;
		int iScreenWidth;
        float fScaleX;
        float fScaleY;
		float fScrollSpeed;
        bool _showBorder;
		
        void loadOptions();
        void saveOptions();
	};

    static void InitializeGame();
	//I need to start the loop with a non empty state stack, because the loop runs until the stack becomes empty. So if it's empty at the
	//beginning, the loop will immediately quit.
    static void Gameloop();
	
    static GameState* GetState();
	//This checks whether the pointer is NULL. If not, it calls the state's init function.If it was successful I push the state to the stack.
    static void PushState(GameState *state);
    static void PushState(const U8 id_);
	//This function just pops the actual state from the top of the stack (end delete it), but don't call the state's Destroy function.
    static void PopState();
	//This calls the actual state's Destroy function, then PopState while the stack isn't empty, and at the end calls the CleanUp.
    static void Exit();
	//This closes the gamewindow and deletes the clock.
    static void CleanUp();
		
    static s_Options Options;
    static bool bIsAppActive;

private:
    static std::stack<GameState*> m_StateStack;

    //When I process Events in the State->Events() and an event causes State change, then I remove the state from the
    //StateStack, but there can be other events need to be processed, and they still need the object. I could delete
    //the state right after the Events() call, and don't need to wait till the end of the frame. (The other functions
    //won't be called bacuse StateChanged variable is true, and I check this instead the StateForDelete pointer.
    static GameState *m_pStateForDelete;	//In PopState I set this to the actual state's pointer, so I can delete it at the end of the frame.
    static GameClock *m_pClock;

	//This variable keeps track of gamestate changes. So if within an event handling the state changed (e.g. the player pressed the P key,
	//which unpaused the game), then I don't continue to call the function's of the new state in the same frame (just skip the update and 
	//render calls). I reset this to false at the begining of every frame.
    static bool m_bStateChanged;
};

#endif
