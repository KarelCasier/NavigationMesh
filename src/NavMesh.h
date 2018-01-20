#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics.hpp>
#include <utility>
#include <queue>
#include <unordered_map>

struct Edge {
    sf::Vector2f start;
    sf::Vector2f end;
    sf::Vector2f mid;
};

class NavPoly {
public:
    NavPoly(sf::ConvexShape* poly)
    {
        //< Copy polygon data over
        poly->setOutlineThickness(2.f);
        poly->setOutlineColor(sf::Color(87, 235, 255));
        poly->setFillColor(sf::Color(87, 235, 255, 200));
        mPoly = poly;
        polyID = NextID++;
    }
    sf::ConvexShape* mPoly;
    //< Vector of the neighbors each paired with the edge shared
    std::vector<std::pair<NavPoly*, Edge>> mNeighbours;
    NavPoly* parentPath;
    int polyID;

private:
    static int NextID;
};

class NavMesh {
public:
    NavMesh();
    void AddPoly(sf::ConvexShape* poly);
    void CreateGraph();

    std::vector<sf::Vector2f> GetNavPath(const sf::Vector2f& startPos, const sf::Vector2f& targetPos);

    void Draw(sf::RenderTarget& target) const;

    ~NavMesh();

private:
    void ClearNavMeshColor(const sf::Color& c) const;
    std::vector<sf::Vector2f>& OptimizePathPoints(std::vector<sf::Vector2f>& path);
    bool HasLineOfSight(const sf::Vector2f& a, const sf::Vector2f& b);
    std::vector<sf::Vector2f> RecreatePathPoints(std::unordered_map<NavPoly*, NavPoly*> CameFrom,
                                                 NavPoly* current,
                                                 const sf::Vector2f& startPos,
                                                 const sf::Vector2f& targetPos);
    std::vector<NavPoly*> RecreatePathNavPolys(std::unordered_map<NavPoly*, NavPoly*> CameFrom,
                                               NavPoly* current,
                                               const sf::Vector2f& startPos,
                                               const sf::Vector2f& targetPos);
    std::vector<sf::Vector2f> OptimizePathFunnel(const std::vector<NavPoly*>& polyPath,
                                                 const sf::Vector2f& start,
                                                 const sf::Vector2f& target);

private:
    std::vector<NavPoly*> mNavPolys;
};

template <typename T, typename Number = int>
struct PriorityQueue {
    typedef std::pair<Number, T> PQElement;
    std::priority_queue<PQElement, std::vector<PQElement>, std::greater<PQElement>> elements;

    inline bool empty() { return elements.empty(); }

    inline void add(T item, Number priority) { elements.emplace(priority, item); }

    inline T get()
    {
        T best_item = elements.top().second;
        elements.pop();
        return best_item;
    }
};
