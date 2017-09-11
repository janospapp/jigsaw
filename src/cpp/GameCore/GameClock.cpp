#include "GameCore/GameClock.hpp"
#include "GameCore/GameApp.hpp"
#include "Utils/Debug.hpp"
#include <iostream>

GameClock::GameClock():
	m_iRenderSkipped(0),
    m_iMaxRenderSkip(TempOptions.maxRenderSkip)
{
	m_ExcessTime = sf::milliseconds(0);
	m_LastUpdateTime = sf::milliseconds(0);
	m_FrameTime = sf::milliseconds(TempOptions.frameTime);
	
}


GameClock::~GameClock()
{
}


void GameClock::Reset()
{
	sf::Time elapsed = m_Clock.restart() + m_ExcessTime;
	m_ExcessTime = elapsed - m_FrameTime;
    if(m_ExcessTime < sf::milliseconds(0))
    {
        m_ExcessTime = sf::milliseconds(0);
	}
}


U32 GameClock::GetUpdateTime()
{
	sf::Time elapsedSinceRestart = m_Clock.getElapsedTime();
	sf::Time elapsedSinceUpdate = m_FrameTime - m_LastUpdateTime + m_ExcessTime + elapsedSinceRestart;
	m_LastUpdateTime = m_ExcessTime + elapsedSinceRestart;
	
	return elapsedSinceUpdate.asMilliseconds();
}


bool GameClock::CanRender()
{
    if(m_ExcessTime + m_Clock.getElapsedTime() < m_FrameTime)
    {
		//I've got enough time. There may be here some correction, because if I've left only 1 millisec, then I won't render.
		m_iRenderSkipped = 0;
		return true;
    }
    else if(m_iRenderSkipped == m_iMaxRenderSkip)
    {
        //I must render!
        debugPrint("Rendering is forced");
		m_iRenderSkipped = 0;
		return true;
    }
    else
    {
        //I won't render at this frame.
		++m_iRenderSkipped;
		return false;
	}
}


U32 GameClock::GetRemainTime()
{
    return (m_FrameTime - (m_Clock.getElapsedTime() + m_ExcessTime)).asMilliseconds();
}
