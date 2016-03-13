#pragma once
#include <vector>
#include <memory>
#include <SFML/Graphics.hpp>

class QuadTree
{
public:
	QuadTree(const float& x, const float& y, const float& width, const float& height, const int& depth, const int& maxDepth);
	
	~QuadTree();
	
	void							AddObject(sf::ConvexShape* object);
	std::vector<sf::ConvexShape*>	GetObjectsAt(const float& x, const float& y);
	void							Clear();
	
	void							Draw(sf::RenderTarget& target);
	
private:
	float	mX;
	float	mY;
	float	mWidth;
	float	mHeight;
	int		mDepth;
	int		maxDepth;
	
	std::vector<sf::ConvexShape*> objects;
	
	QuadTree* NW;
	QuadTree* NE;
	QuadTree* SW;
	QuadTree* SE;
	
	bool Contains(const QuadTree* child, const sf::ConvexShape* object);
	
	sf::RectangleShape			shape;
	
};