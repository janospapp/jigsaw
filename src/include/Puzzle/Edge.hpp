#ifndef EDGE_HPP
#define EDGE_HPP

#include "Puzzle/Curve.hpp"
#include "Utils/Config.hpp"
#include "Utils/SpriteBatch.hpp"
#include "Utils/VertexSprite.hpp"
#include <vector>

enum DirEnum
{
    eLeft,
    eRight,
    eUp,
    eDown
};


enum EdgeType
{
    eCurvy,
    eLine
};


class Edge
{
public:
    //Edge();
    Edge(const bool isHorizontal_, const DirEnum inDirection_, const sf::Vector2f &scale_);
    Edge(const Edge &rhs_);
	virtual sf::Vector2f getStartPoint() const=0;
	virtual sf::Vector2f getEndPoint() const=0;

    virtual void move(const sf::Vector2f &offset_);
    virtual void rotateRight(const sf::Vector2f &around_ = sf::Vector2f(0,0));
    virtual void rotateLeft(const sf::Vector2f &around_ = sf::Vector2f(0,0));
	virtual void flipInside();
    virtual bool isInside(const sf::Vector2f &pt_) const=0;

    virtual void writeOut(std::ostream &os_) const=0;

    virtual void render(SpriteBatch &spBatch_, const sf::Color &color_) const;
    void setPosition(const sf::Vector2f &pos_);

    static void setTextureCoordOffset(const U16 coord_);
protected:
    virtual void changeSprite() const=0;

    mutable VertexSprite _sprite;
    mutable sf::Color _currentColor;
    DirEnum _inDirection;
    static U16 _textureStartCoord;
    bool _isHorizontal;
};


class CurvyEdge: public Edge
{
public:
    CurvyEdge(const bool isHorizontal, const DirEnum insideDir_, const DirEnum faceDir_, std::vector<sf::Vector2f> &ctrPts_,
              std::vector<I8> &dirs_, const sf::Vector2f &scale_);
    CurvyEdge(const CurvyEdge &rhs_);
	sf::Vector2f getStartPoint() const;
	sf::Vector2f getEndPoint() const;

    void move(const sf::Vector2f &offset_);
    void rotateRight(const sf::Vector2f &around_ = sf::Vector2f(0,0));
    void rotateLeft(const sf::Vector2f &around_ = sf::Vector2f(0,0));
	void flipInside();
    bool isInside(const sf::Vector2f &pt_) const;

    void rawRender(sf::RenderTarget &screen_, const sf::Color &color_, const float thickness_ = 1.f) const;

    void writeOut(std::ostream &os_) const;
    static Edge* readIn(std::istream &is_);
private:
    void reverseEdge();	//Only reverse the curves' order. This is need to stay consistent to the inside check after a rotation.
    void changeSprite() const;

    std::vector<Curve> _curves;	//The 9 curves that form an edge.
    DirEnum _faceDir;
};


class LineEdge: public Edge
{
public:
    LineEdge(const bool isHorizontal_, const DirEnum inDirection_, const sf::Vector2f &startPoint_, const sf::Vector2f &endPoint_,
             const sf::Vector2f &scale_);
    LineEdge(const LineEdge &rhs_);
	sf::Vector2f getStartPoint() const;
	sf::Vector2f getEndPoint() const;

    void move(const sf::Vector2f &offset_);
    void rotateRight(const sf::Vector2f &around_ = sf::Vector2f(0,0));
    void rotateLeft(const sf::Vector2f &around_ = sf::Vector2f(0,0));
    bool isInside(const sf::Vector2f &pt_) const;

    void rawRender(sf::RenderTarget &screen_, const sf::Color &color_, const float thickness_ = 1.f) const;

    void writeOut(std::ostream &os_) const;
    static Edge* readIn(std::istream &is_);
private:
    void changeSprite() const;

	sf::Vector2f _startPt;
	sf::Vector2f _endPt;
};

#endif
