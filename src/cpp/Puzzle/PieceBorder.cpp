#include "Puzzle/PieceBorder.hpp"
#include "Managers/PuzzleManager.hpp"
#include "Utils/Helper.hpp"
#include <utility>

PieceBorder::PieceBorder():
    _edges(4)
{
}


PieceBorder::PieceBorder(const std::vector<Edge*> &edges_):
    _edges(4)
{
	for(int i=0; i<4; ++i)
	{
        _edges[i]=std::unique_ptr<Edge>(edges_[i]);
    }
}


PieceBorder::PieceBorder(PieceBorder &&rhs_):
    _edges(std::move(rhs_._edges))
{
}


PieceBorder& PieceBorder::operator=(PieceBorder &&rhs_)
{
    if(this != &rhs_)
	{
		_edges.clear();
        _edges=std::move(rhs_._edges);
    }
	
	return *this;
}


void PieceBorder::move(const sf::Vector2f &offset_)
{
	for(auto &edge: _edges)
	{
        edge->move(offset_);
    }
}


void PieceBorder::rotateRight(const sf::Vector2f &around_)
{
	for(auto &edge: _edges)
	{
        edge->rotateRight(around_);
	}
}


bool PieceBorder::isInside(const sf::Vector2f &pt_) const
{
	bool inside=true;
	for(auto &edge: _edges)
    {
        if(!edge->isInside(pt_))
		{
			inside=false;
			break;
		}
	}

    return inside;
}

const Edge *PieceBorder::getEdge(const U8 index_) const
{
    if(0 <= index_ && index_ < 4)
    {
        return _edges[index_].get();
    }
    else
    {
        return nullptr;
    }
}


void PieceBorder::render(SpriteBatch &spBatch_, const sf::Color &color_, const std::vector<U8> &edgeIndex_) const
{
    for(auto i: edgeIndex_)
    {
        _edges[i]->render(spBatch_,color_);
    }
}

void PieceBorder::writeOut(std::ostream &os_) const
{
    for(auto &e: _edges)
    {
        e->writeOut(os_);
    }
}

PieceBorder PieceBorder::readIn(std::istream &is_)
{
    EdgeType type;
    std::vector<Edge*> edges(4);
    for(U8 i=0; i < 4; ++i)
    {
        read<EdgeType>(is_,&type);
        if(type == eCurvy)
        {
            edges[i] = CurvyEdge::readIn(is_);
        }
        if(type == eLine)
        {
            edges[i] = LineEdge::readIn(is_);
        }
    }

    return PieceBorder(edges);
}
