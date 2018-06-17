#ifndef PIECE_HPP
#define PIECE_HPP

#include "Utils/Config.hpp"
#include "Puzzle/PieceBorder.hpp"
#include "SFML/Graphics.hpp"
#include "Utils/SpriteBatch.hpp"

// This is a compact piece data used by loading. I don't need the image and border information. They will be
// loaded elsewhere and the pieces will be contructed from those and these data.
struct PieceData
{
    PieceBorder _border;
    std::vector<U8> _freeSides;
    sf::Vector2f _position;
    U16 _puzzlePosX;
    U16 _puzzlePosY;
    U16 _linkId;
    U8 _direction;
};


enum BorderRenderingMode
{
    eNone,
    eOnlyFree,
    eAll
};


class Piece 
{
public:
    Piece(const U16 x_, const U16 y_, const U16 linkID_, const sf::Sprite &sprite_,PieceBorder &&border_, const U8 dir_=0,
          const std::vector<U8> &freeSides_ = {0,1,2,3});

	U16 getLinkId() const;
    void setLinkId(const U16 id_);
    const sf::Vector2f& getCenter() const;
    void setPosition(const sf::Vector2f &pos_);
    void setSelection(const bool sel_);
    void move(const sf::Vector2f &offset_);
    void rotateRight(const sf::Vector2f &around_);

    // First checks whether the point is inside the bounding square and then checks the actual borders
    // of the piece if necessary.
    bool isPointInside(const sf::Vector2f &p_) const;
	sf::Vector2i getPosInPuzzle() const;
    U8 getNumFreeSides() const;
    void decreaseFreeSides(const U8 dir_);
    bool checkMatch(const Piece *rhs_) const;
    U8 getRelativeDirection(const Piece *rhs_, const bool respectRotation_ = true) const;
    sf::Vector2f getOffsetToMatch(const Piece *rhs_) const;

    void render(SpriteBatch &spBatch_, const BorderRenderingMode &renderMode_, const sf::Color &color_) const;
    const sf::FloatRect getBoundRect() const;

    // Functions for saving. writeOut will save pieceData to the file and readIn will load them back into
    // the helping struct.
    void writeOut(std::ostream &os_) const;
    static PieceData readIn(std::istream &is_);

private:
	sf::Sprite _sprite;
	PieceBorder _border;
    std::vector<U8> _freeSides;
	U16 _iPosInPuzzleX;
    U16 _iPosInPuzzleY;
    U16 _iLinkId;
	U8 _direction;	//0 is the original direction, it is increased by a rotation.
    bool _selected;

    // This is only for debugging purpose:
    void debugRender(sf::RenderWindow &screen) const;
};

#endif
