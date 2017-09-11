#ifndef SELECTBOX_HPP
#define SELECTBOX_HPP

#include "SFML/Graphics.hpp"

struct SBoxReturn
{
	sf::Vector2i start;	//If it's just a click, then start is the pos. of click, otherwise it's the top left corner of the box
	sf::Vector2i end;	//If this is a box, then it's the bottom right corner, otherwise it's irrelevant
	bool isClick;
};


class SelectBox
{
public:
	SelectBox();
	~SelectBox();
	
    void Start(const sf::Vector2i &start);
    SBoxReturn End(const sf::Vector2i &end);
    void Update(const sf::Vector2i &upd);
	void Render() const;
	
private:
	sf::Vector2i m_vecStart;
	sf::Vector2i m_vecEnd;
	bool m_bActive;
	bool m_bBox;
};

#endif //SELECTBOX_HPP
