#pragma once
#include "Utilities.h"
#include <SFML/System/Vector2.hpp>
#include "math.h"

typedef double coord2_t;  // must be big enough to hold 2*max(|coordinate|)^2
// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
// Returns a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the points are collinear.
coord2_t cross(const sf::Vector2f &O, const sf::Vector2f &A, const sf::Vector2f &B);

bool vectorCompare(sf::Vector2f& a, sf::Vector2f& b);

// Returns a list of points on the convex hull in counter-clockwise order.
// Note: the last point in the returned list is the same as the first one.
std::vector<sf::Vector2f> convex_hull(std::vector<sf::Vector2f>& P);

//----------------------------------------------------------------------------
typedef const sf::Vector2f& sfPoint;

///< Code enclosed here is not mine
bool onSegment(sfPoint p, sfPoint q, sfPoint r);

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(sfPoint p, sfPoint q, sfPoint r);

// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool doIntersect(sfPoint p1, sfPoint q1, sfPoint p2, sfPoint q2);

//--------------------------------------------------------------------

bool isPointOnLineSegment(const sf::Vector2f& p, const sf::Vector2f& a1, const sf::Vector2f& a2);

sf::Vector2f getMinimumDistfromPoint(const sf::Vector2f& p, const std::vector<sf::Vector2f>& points);
//----------------------------------------------------------------------------

sf::Vector2f GetIntersectionPoint(const sf::Vector2f& x1, const sf::Vector2f& x2, const sf::Vector2f& y1, const sf::Vector2f& y2);
