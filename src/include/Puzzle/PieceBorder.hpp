#ifndef PIECEBORDER_HPP
#define PIECEBORDER_HPP

#include "Puzzle/Edge.hpp"
#include "SFML/Graphics.hpp"
#include <vector>
#include <memory>

class PieceBorder
{
public:
	PieceBorder();
    PieceBorder(const std::vector<Edge*> &edges_);
    PieceBorder(PieceBorder &&rhs_);
    PieceBorder& operator=(PieceBorder &&rhs_);

    void move(const sf::Vector2f &offset_);
    void rotateRight(const sf::Vector2f &around_);
    bool isInside(const sf::Vector2f &pt_) const;
    const Edge* getEdge(const U8 index_) const;

    void render(SpriteBatch &spBatch_, const sf::Color &color_, const std::vector<U8> &edgeIndex_ = {0, 1, 2, 3}) const;

    void writeOut(std::ostream &os_) const;
    static PieceBorder readIn(std::istream &is_);
private:
    std::vector<std::unique_ptr<Edge>> _edges;
};

#endif
