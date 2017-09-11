#ifndef LINK_HPP
#define LINK_HPP

#include "Utils\Config.hpp"
#include "Puzzle\Piece.hpp"
#include <unordered_set>
#include <memory>

class Link
{
public:
	Link(const U16 id);
	
	void addPiece(std::unique_ptr<Piece> &&piece);
	//void move(const sf::Vector2f &offset);
	//void rotateRight(const sf::Vector2f &around);
	//void render(sf::RenderWindow &screen) const;
	//bool isInside(const sf::Vector2f &pt) const;
	std::unordered_set<Piece *> getPieces() const;
	void merge(Link &link);
	U16 getSize() const;
	U16 getId() const;

private:
	std::vector<std::unique_ptr<Piece>> _pieces;
	U16 _id;
	U16 _size;
};



#endif
