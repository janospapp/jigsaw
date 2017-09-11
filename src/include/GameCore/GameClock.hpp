#pragma once
#include "Utils/Config.hpp"
#include "SFML/System.hpp"

//The clock always returns the time as milliseconds. My update functions also wait for time in millisec.
class GameClock
{
public:
	GameClock();	//Need to load tempoptions first!!
	~GameClock();
	
	void Reset();
    U32 GetUpdateTime();
	bool CanRender();
    U32 GetRemainTime();

private:
	sf::Clock m_Clock;
	sf::Time m_ExcessTime;
	sf::Time m_LastUpdateTime;
	sf::Time m_FrameTime;
    U8 m_iRenderSkipped;
    U8 m_iMaxRenderSkip;
	
};
