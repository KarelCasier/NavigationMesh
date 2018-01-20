#include "NavMesh.h"
#include <iostream>
#include "Utilities.h"
#include <math.h>
#include "Game.h"
#include <algorithm>
#include "CollisionHelpers.h"

int NavPoly::NextID = 0;

NavMesh::NavMesh()
{
	
}

void PrintNavPoly(NavPoly* navPoly) {
    std::cout << "Poly ID: " << navPoly->polyID << std::endl;
    for (int i = 0; i < navPoly->mPoly->getPointCount(); ++i) {
        std::cout << VectorToStr(navPoly->mPoly->getPosition() + navPoly->mPoly->getPoint(i)) << std::endl;
    }
}

void NavMesh::AddPoly(sf::ConvexShape* poly)
{
	//std::cout << VectorToStr(poly->getPosition()) << std::endl;
	//< Order the points in clounter clockwise order for consistant edges (For funnel algorithm)
	std::vector<sf::Vector2f> points;
	int pointCount = poly->getPointCount();
	for ( int i = 0; i < pointCount; ++i ) {
		points.push_back(poly->getPoint(i));
	}
	points = convex_hull(points);
	NavPoly* navPoly = new NavPoly(poly);
	for ( int i = 0 ; i < pointCount; ++i ) {
		navPoly->mPoly->setPoint(i, points.at(i));
	}
	mNavPolys.push_back(navPoly);
}

void NavMesh::CreateGraph()
{
	for (int i = 0; i < mNavPolys.size(); i++) {
		for (int j = i; j < mNavPolys.size(); j++) {
			//< Don't check with self
			if (i == j) {
				continue;
			}
			
			NavPoly* iPoly = mNavPolys.at(i);
			NavPoly* jPoly = mNavPolys.at(j);
//            PrintNavPoly(iPoly);
//            PrintNavPoly(jPoly);
			//< Quick check to see if bounding boxes don't touch
			if (!iPoly->mPoly->getGlobalBounds().intersects(jPoly->mPoly->getGlobalBounds())) {
				//continue;
			}
			//< Check how many points are shared
			unsigned int numPtsConnected = 0;
			std::vector<sf::Vector2f> connectedPts;
			for (int u = 0; u < iPoly->mPoly->getPointCount(); u++) {
				for (int v = 0; v < jPoly->mPoly->getPointCount(); v++) {
					if (iPoly->mPoly->getPosition() + iPoly->mPoly->getPoint(u) == jPoly->mPoly->getPosition() + jPoly->mPoly->getPoint(v)) {
						++numPtsConnected;
						connectedPts.push_back(iPoly->mPoly->getPosition() + iPoly->mPoly->getPoint(u));
                        
					}
				}
			}
			//< Warn NavMesh Polys are not set up right
			if (numPtsConnected > 3) {
				std::cout << "Alert: 2 Polygons touch by more then 2 points which may cause navigation problems" << std::endl;
			}
			
			if (numPtsConnected == 2) {
				Edge mEdge;
				mEdge.start = connectedPts.at(0);
				mEdge.end = connectedPts.at(1);
				mEdge.mid = Midpoint(mEdge.start, mEdge.end);
				iPoly->mNeighbours.push_back(std::make_pair(jPoly, mEdge));
				jPoly->mNeighbours.push_back(std::make_pair(iPoly, mEdge));
			}
			
		}
	}
}

float heuristic(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return fabs(a.x-b.x) + fabs(a.y-b.y);
}

float heuristic(const NavPoly* a, const NavPoly* b)
{
	return fabs(a->mPoly->getPosition().x - b->mPoly->getPosition().x) + fabs(a->mPoly->getPosition().y - b->mPoly->getPosition().y);
}

std::vector<sf::Vector2f> NavMesh::RecreatePathPoints(std::unordered_map<NavPoly*, NavPoly*> CameFrom, NavPoly* current, const sf::Vector2f& startPos, const sf::Vector2f& targetPos)
{
    std::vector<sf::Vector2f> path;
    path.push_back(targetPos);
    path.push_back(GetCenter(*current->mPoly));
    while (CameFrom[current]) {
        current = CameFrom[current];
        path.push_back(GetCenter(*current->mPoly));
    }
    path.push_back(startPos);
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<NavPoly*> NavMesh::RecreatePathNavPolys(std::unordered_map<NavPoly*, NavPoly*> CameFrom, NavPoly* current, const sf::Vector2f& startPos, const sf::Vector2f& targetPos)
{
    std::vector<NavPoly*> path;
    path.push_back(current);
    while (CameFrom[current]) {
        current = CameFrom[current];
        path.push_back(current);
    }
    std::reverse(path.begin(), path.end());
    return path;

}

bool doIntersect(const sf::Vector2f& a, const sf::Vector2f& b, const NavPoly* navPoly) {
	sf::ConvexShape* poly = navPoly->mPoly;
	sf::Vector2f polyPos = poly->getPosition();
	for (int i = 0; i < poly->getPointCount(); ++i) {
		if (doIntersect(a, b, polyPos + poly->getPoint(i), polyPos + poly->getPoint(i+1))) {
			return true;
		}	
	}
	if (doIntersect(a, b, polyPos + poly->getPoint(0), polyPos + poly->getPoint(poly->getPointCount()-1))) {
		return true;
	}
	return false;
}

bool NavMesh::HasLineOfSight(const sf::Vector2f& a, const sf::Vector2f& b)
{
	NavPoly* aNavPoly = nullptr;
	NavPoly* bNavPoly = nullptr;
	for (NavPoly* poly : mNavPolys) {
		if (ptContatinedInPoly(a, *poly->mPoly)) {
			aNavPoly = poly;
		}
		if (ptContatinedInPoly(b, *poly->mPoly)) {
			bNavPoly = poly;
		}
	}
	NavPoly* iterPoly = aNavPoly;
    std::vector<NavPoly*> prevIterPolys;
	while (iterPoly != bNavPoly) {
        bool iterationFound = false;
		for ( auto neighbourPoly : iterPoly->mNeighbours ) {
            if (std::find(prevIterPolys.begin(), prevIterPolys.end(), neighbourPoly.first) == prevIterPolys.end()) {
                if (doIntersect(a, b, neighbourPoly.first)) {
                    prevIterPolys.push_back(iterPoly);
                    iterPoly = neighbourPoly.first;
                    iterationFound = true;
                }
            }
		}
		if (!iterationFound) {
			return false;
		}
	}
	return true;
	/*/
	for ( auto neighbourPair : aNavPoly->mNeighbours ) {
		if (neighbourPair.first == bNavPoly && doIntersect(a, b, neighbourPair.second.start, neighbourPair.second.end)) {
			result = true;
		}
	}
	return result;
	//*/
}

std::vector<sf::Vector2f>& NavMesh::OptimizePathPoints(std::vector<sf::Vector2f>& path)
{
	bool madeChange = true;
	while ( madeChange ) {
		madeChange = false;
		for (int i = 0; i < path.size()-2; ++i) {
			if ( HasLineOfSight( path[i], path[i+2] ) ) {
				path.erase(path.begin() + i + 1);
				madeChange = true;
                break;
			}
		}
	}
	return path;
}

std::vector<sf::Vector2f> NavMesh::OptimizePathFunnel(const std::vector<NavPoly*>& polyPath, const sf::Vector2f& start, const sf::Vector2f& target)
{	
	std::vector<sf::Vector2f> optimPath;
	//< Start with apex point
	sf::Vector2f apex = start;
	optimPath.push_back(start);
	//< Get left and right vertices of first edge
	sf::Vector2f left, right;
	for ( auto NeighEdgePair : polyPath.front()->mNeighbours ) {
		if ( NeighEdgePair.first == polyPath.at(1) ){
			left = NeighEdgePair.second.start;
			right = NeighEdgePair.second.end;
		}
	}
	//< Step through
	for ( int i = 2; i < polyPath.size(); ++i ) {
		//float funnelAngle = acosf( DotProduct(left - apex, right - apex) / ( Norm(left, apex) * Norm(right, apex) ) );
		//< Advance to next edge
		//< Get neighbour and edge
		NavPoly* neighNavPoly = polyPath.at(i);
		Edge neighEdge;
		for ( auto NeighEdgePair : polyPath.front()->mNeighbours ) {
			if ( NeighEdgePair.first == neighNavPoly ){
				neighEdge = NeighEdgePair.second;
			}
		}
		
		//< Try to move left to next vertex
		if ( left != neighEdge.start ) {
			sf::Vector2f potenLeft = neighEdge.start;
			//float potenAngle = acosf( DotProduct(potenLeft - apex, right - apex) / ( Norm(potenLeft, apex) * Norm(right, apex) ) );
			if ( isLeft(apex, right, potenLeft) ) {

			}
		}	

		//< Try to move right to next vertex
	}	
	
}

std::vector<sf::Vector2f> NavMesh::GetNavPath(const sf::Vector2f& startPos, const sf::Vector2f& targetPos)
{
    std::vector<NavPoly*> ClosedSet; 	//< Set of already checked nodes
    PriorityQueue<NavPoly*> OpenSet; 	//< Set of potential nodes to check
    std::vector<NavPoly*> OpenSetVec;	//< Duplicate set for open
    std::unordered_map<NavPoly*, NavPoly*> CameFrom;//< Set of navigated nodes
    std::unordered_map<NavPoly*, float> g_score; 	//< Cost from start along best known path
    std::unordered_map<NavPoly*, float> f_score;	//< Total cost from start to goal through y

    //< Find start and target node
    NavPoly* targetNavPoly = nullptr;
    NavPoly* startNavPoly = nullptr;
    for (NavPoly* poly : mNavPolys) {
	    if (ptContatinedInPoly(startPos, *poly->mPoly)) {
		    startNavPoly = poly;
	    }
	    if (ptContatinedInPoly(targetPos, *poly->mPoly)) {
		    targetNavPoly = poly;
	    }
    }
    if ( !startNavPoly || !targetNavPoly ) {
        //std::cerr << "Failed to find current or target navpoly" << std::endl;
        std::vector<sf::Vector2f> path;
        path.push_back(startPos);
        path.push_back(targetPos);
        return path;
    }

    OpenSet.add(startNavPoly, 0);
    OpenSetVec.push_back(startNavPoly);

    g_score[startNavPoly] = 0;
    f_score[startNavPoly] = heuristic(startNavPoly, targetNavPoly);	

    while ( !OpenSet.empty() )
    {
	NavPoly *current = OpenSet.get(); //< Get and remove node with lowest f_score
	OpenSetVec.erase(std::remove(OpenSetVec.begin(), OpenSetVec.end(), current), OpenSetVec.end());

	if ( current == targetNavPoly ) {
	std::vector<sf::Vector2f> path = RecreatePathPoints(CameFrom, current, startPos, targetPos);
	return OptimizePathPoints(path);
	    }
	    
	    ClosedSet.push_back(current);

	    for ( auto neighbour : current->mNeighbours )
	    {
		    NavPoly *neighbourNavPoly = neighbour.first;
		    
		    if(std::find(ClosedSet.begin(), ClosedSet.end(), neighbourNavPoly) != ClosedSet.end()) {
			    continue; //< Skip the neighbour if it's already been checked
		    }

		    auto got = g_score.find(current
				);
		    if ( got == g_score.end() ) {
			    g_score[current] = 99999;
		    }
		    int tentative_g_score = g_score[current] + heuristic(current, targetNavPoly);
		    
		    if ( std::find(OpenSetVec.begin(), OpenSetVec.end(), neighbourNavPoly) == OpenSetVec.end() ) {
			    //< Discover new node
			    g_score[neighbourNavPoly] = tentative_g_score;
			    f_score[neighbourNavPoly] = g_score[neighbourNavPoly] + heuristic(neighbourNavPoly, targetNavPoly);
			    OpenSet.add(neighbourNavPoly, f_score[neighbourNavPoly]);
		    } else if ( tentative_g_score >= g_score[neighbourNavPoly] ) {
			    // Not a better path
			    continue;
		    }

		    //< This path is the best so far, save it
		    CameFrom[neighbourNavPoly] = current;
	    }
		    
    }
    assert(true); //< FAILED
}
/*/
std::vector<sf::Vector2f> NavMesh::GetNavPath(const sf::Vector2f& startPos, const sf::Vector2f& targetPos)
{
	PriorityQueue<NavPoly*> open;
	std::vector<NavPoly*> closed;
	std::vector<NavPoly*> path;
	std::unordered_map<NavPoly*, float> cost_so_far;
	
	NavPoly* targetNavPoly = nullptr;
	NavPoly* startNavPoly = nullptr;
	for (NavPoly* poly : mNavPolys) {
		if (ptContatinedInPoly(startPos, *poly->mPoly)) {
			startNavPoly = poly;
			open.add(poly , 0);
		} else if (ptContatinedInPoly(targetPos, *poly->mPoly)) {
			targetNavPoly = poly;
		}
	}
	assert(startNavPoly != nullptr);  // Cannot find pt in any navpoly
	assert(targetNavPoly != nullptr); // Cannot find pt in any navpoly
	
	cost_so_far[startNavPoly] = 0;
	
	
	while (!open.empty()) {
		//< Get next ploy with smallest F
		NavPoly* currentNavPoly = open.get();
		
		closed.push_back(currentNavPoly);
		
		if (currentNavPoly == targetNavPoly) {
			std::vector<sf::Vector2f> vecPath;
			while (currentNavPoly->parentPath) {
				path.push_back(currentNavPoly);
				vecPath.push_back(GetCenter(*currentNavPoly->mPoly));
				currentNavPoly = currentNavPoly->parentPath;
			}
			return vecPath;
		}
		
		for (auto next : currentNavPoly->mNeighbours) {
			// nextCost = cost to get here from current path + distance to next
			float nextCost = cost_so_far[currentNavPoly] + 1.0/heuristic(GetCenter(*currentNavPoly->mPoly), GetCenter(*next.first->mPoly));
			//////////// CHECK THAT NOT IN CLOSED LIST FIRST /////////////////////
			bool neighbourInClosedList = false;
			for (auto c : closed){
				if (next.first == c){
					neighbourInClosedList = true;
				}
			}
			if (!neighbourInClosedList) {
				open.add(next.first, nextCost);
			}
			
			next.first->parentPath = currentNavPoly;
		}
		
		/////////////// ADD CURENT NAV POLY TO CLOSED LIST //////////////////
	}
	//vec.erase(std::remove(vec.begin(), vec.end(), int_to_remove), vec.end());
    return optimizePolyPath(path, startPos, targetPos);
}
//*/

void NavMesh::ClearNavMeshColor(const sf::Color& c) const{
    for (auto navPoly : mNavPolys){
        navPoly->mPoly->setFillColor(c);
    }
}

void NavMesh::Draw(sf::RenderTarget &target) const
{
    ClearNavMeshColor(sf::Color(87,235,255,200));
    
    for (auto navPoly : mNavPolys) {
        //< Set player nav poly to a different colour
        if (ptContatinedInPoly(TheGame::Instance()->getPlayerPosition(), *navPoly->mPoly))
        {
            navPoly->mPoly->setFillColor(sf::Color(255,235,87,200));
            for ( auto neighbour : navPoly->mNeighbours) {
                neighbour.first->mPoly->setFillColor(sf::Color(230,200,50,200));
            }
        }
    }
    
	for (auto navPoly : mNavPolys) {
		target.draw(*(navPoly->mPoly));
		DrawDebugCircle(GetCenter(*navPoly->mPoly), 10.f, target, sf::Color::Green);

		
		//< Draw connected Edges
		/*/
		for ( auto neighbour : navPoly->mNeighbours) {
			DrawDebugCircle(neighbour.second.mid, 10.f, target, sf::Color::Cyan);
		}
		//*/
		
	}
	
}
NavMesh::~NavMesh()
{
	for (int i = 0; i < mNavPolys.size(); i++) {
		delete mNavPolys.at(i)->mPoly;
		delete mNavPolys.at(i);
	}
}
