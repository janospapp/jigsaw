#include "Puzzle\Link.hpp"

Link::Link(const U16 id):
    _id(id),
    _size(0)
{
}


void Link::addPiece(std::unique_ptr<Piece> &&piece)
{
	if(piece != nullptr)
	{
		_pieces.push_back(std::move(piece));
        ++_size;
	}
}


std::unordered_set<Piece *> Link::getPieces() const
{
	std::unordered_set<Piece *> ret;
	for(auto &piece: _pieces)
	{
		ret.insert(piece.get());
	}

	return ret;
}


void Link::merge(Link &link)
{
	_size+=link._size;
	_pieces.reserve(_size);

	for(auto &piece: link._pieces)
	{
		piece->setLinkId(_id);
		_pieces.push_back(std::move(piece));
	}
}


U16 Link::getSize() const
{
	return _size;
}


U16 Link::getId() const
{
	return _id;
}
