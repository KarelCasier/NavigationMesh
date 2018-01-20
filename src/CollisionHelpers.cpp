#include "CollisionHelpers.h"
#include "Utilities.h"
#include "math.h"

typedef double coord2_t;  // must be big enough to hold 2*max(|coordinate|)^2
// 2D cross product of OA and OB vectors, i.e. z-component of their 3D cross product.
// Returns a positive value, if OAB makes a counter-clockwise turn,
// negative for clockwise turn, and zero if the points are collinear.
coord2_t cross(const sf::Vector2f &O, const sf::Vector2f &A, const sf::Vector2f &B)
{
	return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}

bool vectorCompare(sf::Vector2f& a, sf::Vector2f& b)
{
	return a.x < b.x || (a.x == b.x && a.y < b.y);
}

// Returns a list of points on the convex hull in counter-clockwise order.
// Note: the last point in the returned list is the same as the first one.
std::vector<sf::Vector2f> convex_hull(std::vector<sf::Vector2f>& P)
{
	int n = P.size(), k = 0;
	std::vector<sf::Vector2f> H(2*n);
 
	// Sort points lexicographically
	sort(P.begin(), P.end(), vectorCompare);
 
	// Build lower hull
	for (int i = 0; i < n; ++i) {
		while (k >= 2 && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
		H[k++] = P[i];
	}
 
	// Build upper hull
	for (int i = n-2, t = k+1; i >= 0; i--) {
		while (k >= t && cross(H[k-2], H[k-1], P[i]) <= 0) k--;
		H[k++] = P[i];
	}
 
	H.resize(k);
	return H;
}

//----------------------------------------------------------------------------
typedef const sf::Vector2f& sfPoint;

///< Code enclosed here is not mine
bool onSegment(sfPoint p, sfPoint q, sfPoint r)
{
	if (q.x <= std::max(p.x, r.x) && q.x >= std::min(p.x, r.x) &&
		q.y <= std::max(p.y, r.y) && q.y >= std::min(p.y, r.y))
		return true;
 
	return false;
}

// To find orientation of ordered triplet (p, q, r).
// The function returns following values
// 0 --> p, q and r are colinear
// 1 --> Clockwise
// 2 --> Counterclockwise
int orientation(sfPoint p, sfPoint q, sfPoint r)
{
	// See 10th slides from following link for derivation of the formula
	// http://www.dcs.gla.ac.uk/~pat/52233/slides/Geometry1x1.pdf
	int val = (q.y - p.y) * (r.x - q.x) -
	(q.x - p.x) * (r.y - q.y);
 
	if (val == 0) return 0;  // colinear
 
	return (val > 0)? 1: 2; // clock or counterclock wise
}

// The main function that returns true if line segment 'p1q1'
// and 'p2q2' intersect.
bool doIntersect(sfPoint p1, sfPoint q1, sfPoint p2, sfPoint q2)
{
	// Find the four orientations needed for general and
	// special cases
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);
 
	// General case
	if (o1 != o2 && o3 != o4)
		return true;
 
	// Special Cases
	// p1, q1 and p2 are colinear and p2 lies on segment p1q1
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;
 
	// p1, q1 and p2 are colinear and q2 lies on segment p1q1
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;
 
	// p2, q2 and p1 are colinear and p1 lies on segment p2q2
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;
 
	// p2, q2 and q1 are colinear and q1 lies on segment p2q2
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;
 
	return false; // Doesn't fall in any of the above cases
}

//--------------------------------------------------------------------

bool isPointOnLineSegment(const sf::Vector2f& p, const sf::Vector2f& a1, const sf::Vector2f& a2)
{
	// a1---a2
	//  \   /
	//   \ /
	//    p
	
	return fabsf((Norm(a1-p) + Norm(a2-p)) - Norm(a1-a2)) < 0.01;
}

sf::Vector2f getMinimumDistfromPoint(const sf::Vector2f& p, const std::vector<sf::Vector2f>& points)
{
	if (points.empty())
	{
		return sf::Vector2f(0,0);
	}
	sf::Vector2f min = points.front();
	float minDistSquared = NormSquared(p-min);
	
	for (const sf::Vector2f& point : points)
	{
		float thisDistSquared = NormSquared(p-point);
		if (thisDistSquared < minDistSquared)
		{
			minDistSquared = thisDistSquared;
			min = point;
		}
	}
	return min;
}
//----------------------------------------------------------------------------

sf::Vector2f GetIntersectionPoint(const sf::Vector2f& x1, const sf::Vector2f& x2, const sf::Vector2f& y1, const sf::Vector2f& y2)
{
	///< Get line in form: y = mx + b
	float m1;
	if (x1.x - x2.x == 0)
	{
		m1 = 999999;
	}
	else
	{
		m1 = (x1.y - x2.y) / (x1.x - x2.x);
	}
	float b1 = x2.y - m1 * x2.x;
	
	///< Get line in form: y = mx + b
	float m2;
	
	if (y1.x - y2.x == 0)
	{
		m2 = 999999;
	}
	else
	{
		m2 = (y1.y - y2.y) / (y1.x - y2.x);
	}
	
	float b2 = y2.y - m2 * y2.x;
	
	if (m1 - m2 == 0)
	{
		//no intersection
		//return y1;
	}
	float xIntercect = (b2-b1) / (m1-m2);
	float yIntercect = m1 * xIntercect + b1;
	
	return sf::Vector2f(xIntercect, yIntercect);
	
}
