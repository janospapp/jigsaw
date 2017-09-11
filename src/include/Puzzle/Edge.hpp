#ifndef EDGE_HPP
#define EDGE_HPP

#include "Puzzle/Curve.hpp"
#include "SFML/Graphics.hpp"
#include "Utils/Config.hpp"
#include <vector>

class Edge
{
public:
	Edge();
	Edge(const bool isHorizontal, const I8 direction);
	Edge(const Edge &rhs);
	virtual sf::Vector2f getStartPoint() const=0;
	virtual sf::Vector2f getEndPoint() const=0;

	virtual void move(const sf::Vector2f &offset)=0;
	virtual void rotateRight(const sf::Vector2f &around = sf::Vector2f(0,0)); 
	virtual void rotateLeft(const sf::Vector2f &around = sf::Vector2f(0,0)); 
	virtual void flipInside();
	virtual bool isInside(const sf::Vector2f &pt) const=0;

    // Render code is the same for every edge. I just get the points, and draw
    // a line between each adjecent points with the given style.
    void render(sf::RenderTarget &screen_, const sf::Color &color_, const float thickNess_ = 1.f) const;
    void addLinesToRender(std::vector<sf::Vertex> &vertexVec_, const sf::Color &color_, const float thickNess_ = 1.f) const;

	void setPosition(const sf::Vector2f &pos);
protected:
    // Get the points for rendering.
    virtual std::vector<sf::Vector2f> getPoints() const = 0;

	bool _isHorizontal;
	I8 _inDirection; //horizontal: 1->above, -1->below
					 //vertical: 1->right, -1->left
};


class CurvyEdge: public Edge
{
public:
	//Needs an ifstream or a filename to load the curve control points. Or the Puzzle builder will load the points, transforms if
	//it has to and tis constructor only gets the points.
    CurvyEdge(const bool isHorizontal, const I8 direction, const I8 faceDir, std::vector<sf::Vector2f> &ctrPts, std::vector<I16> &dirs);
	CurvyEdge(const CurvyEdge &rhs);
	sf::Vector2f getStartPoint() const;
	sf::Vector2f getEndPoint() const;

	void move(const sf::Vector2f &offset);
    void rotateRight(const sf::Vector2f &around = sf::Vector2f(0,0));
    void rotateLeft(const sf::Vector2f &around = sf::Vector2f(0,0));
	void flipInside();
    bool isInside(const sf::Vector2f &pt) const;
private:
    std::vector<sf::Vector2f> getPoints() const;

	std::vector<Curve> _curves;	//The 9 curves that form an edge.
	I8 _faceDir;	//horizontal: 1 up, -1 down vertical: 1 right, -1 left
	void reverseEdge();	//Only reverse the curves' order. This is need to stay consistent to the inside check after a rotation.
};


class LineEdge: public Edge
{
public:
	LineEdge(const bool isHorizontal, const I8 direction, const sf::Vector2f &startPoint, const sf::Vector2f &endPoint);
	LineEdge(const LineEdge &rhs);
	sf::Vector2f getStartPoint() const;
	sf::Vector2f getEndPoint() const;

	void move(const sf::Vector2f &offset);
	void rotateRight(const sf::Vector2f &around = sf::Vector2f(0,0));
	void rotateLeft(const sf::Vector2f &around = sf::Vector2f(0,0));
	bool isInside(const sf::Vector2f &pt) const;
private:
    std::vector<sf::Vector2f> getPoints() const;

	sf::Vector2f _startPt;
	sf::Vector2f _endPt;
};

#endif
