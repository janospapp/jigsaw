#ifndef PIECE_HPP
#define PIECE_HPP

#include "Utils/Config.hpp"
#include "Puzzle/PieceBorder.hpp"
#include "SFML/Graphics.hpp"


class Piece 
{
public:
    Piece(const U16 x, const U16 y, const U16 size, const U16 linkID, const sf::Sprite &sprite, PieceBorder &&border, const U8 dir=0);

	U16 getLinkId() const;
	void setLinkId(const U16 id);
	const sf::Vector2f& getCenter() const;
    const sf::FloatRect getBound() const;
	void setPosition(const sf::Vector2f &pos);
    void setSelection(const bool sel_);
	void move(const sf::Vector2f &offset);
	void rotateRight(const sf::Vector2f &around);

    // First checks whether the point is inside the bounding square and then checks the actual borders
    // of the piece if necessary.
	bool isPointInside(const sf::Vector2f &p) const;
	sf::Vector2i getPosInPuzzle() const;
	U8 getFreeSides() const;
	void decreaseFreeSides();
    bool checkMatch(const Piece *rhs) const;
    sf::Vector2f getOffsetToMatch(const Piece *rhs) const;

    // Only renders the piece if it's visible. Color and thickness used for border drawing.
    // I put the border vertexes in the vertexVec and draw them once in the windowManager.
    void render(sf::RenderWindow &screen, const bool drawBorder_, const sf::Color &color_) const;

private:
	sf::Sprite _sprite;
	PieceBorder _border;
	U16 _iPosInPuzzleX;
	U16 _iPosInPuzzleY;
    U16 _size;
	U16 _iLinkId;
	U8 _iFreeSides;
	U8 _direction;	//0 is the original direction, it is increased by a rotation.
    bool _selected;

    U8 getRelativeDirection(const Piece *rhs) const;

    //This is only for debugging purpose:
    void debugRender(sf::RenderWindow &screen) const;
};

#endif
