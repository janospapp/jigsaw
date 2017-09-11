#include "Utils/SelectBox.hpp"
#include "GameCore/GameApp.hpp"
#include <cstdlib>


SelectBox::SelectBox():
    m_bActive(false),
    m_bBox(false)
{
}


SelectBox::~SelectBox()
{
}


void SelectBox::Start(const sf::Vector2i &start)
{
	m_vecStart = start;
	m_bActive = true;
}


SBoxReturn SelectBox::End(const sf::Vector2i &end)
{
	m_vecEnd = end;
	SBoxReturn ret;
	
    if(!m_bBox && (std::abs(m_vecStart.x - m_vecEnd.x) > 3 || std::abs(m_vecStart.y - m_vecEnd.y) > 3))
    {	//It's a box, not a regular click
		m_bBox = true;
	}
	
    if(m_bBox)
    {
		ret.isClick = false;
        ret.start.x = (m_vecStart.x <= m_vecEnd.x ? m_vecStart.x : m_vecEnd.x);
        ret.start.y = (m_vecStart.y <= m_vecEnd.y ? m_vecStart.y : m_vecEnd.y);
        ret.end.x = (m_vecStart.x <= m_vecEnd.x ? m_vecEnd.x : m_vecStart.x);
        ret.end.y = (m_vecStart.y <= m_vecEnd.y ? m_vecEnd.y : m_vecStart.y);
    }
    else
    {
		ret.isClick = true;
		ret.start = m_vecStart;
	}
	
	m_bActive = false;
	m_bBox = false;
	
	return ret;
}


void SelectBox::Update(const sf::Vector2i &upd)
{
    if(m_bActive)
    {
		m_vecEnd = upd;
		
        if(!m_bBox && (std::abs(m_vecStart.x - m_vecEnd.x) > 3 || std::abs(m_vecStart.y - m_vecEnd.y) > 3))
        {	//If it's not a box, I check whether it is or not
			m_bBox = true;
            //InputSystem::CaptureMouseOnGamePlayArea();
		}
		
	}
}


void SelectBox::Render() const
{
    if(m_bActive && m_bBox)
    {
        sf::RectangleShape selectBox(sf::Vector2f((float)std::abs(m_vecStart.x - m_vecEnd.x),
                                                  (float)std::abs(m_vecStart.y - m_vecEnd.y)));
        selectBox.setFillColor(sf::Color::Transparent);
        selectBox.setOutlineColor(sf::Color::Black);
        selectBox.setOutlineThickness(1.0f);
        float x = (float)(m_vecStart.x <= m_vecEnd.x ? m_vecStart.x : m_vecEnd.x);
        float y = (float)(m_vecStart.y <= m_vecEnd.y ? m_vecStart.y : m_vecEnd.y);
        selectBox.setPosition(x, y);
		
        //GameApp::Screen->draw(selectBox);
	}
}
