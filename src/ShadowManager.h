#pragma once

#include <vector>
#include <array>
#include <SFML/Graphics.hpp>

class ShadowManager {
public:
    ShadowManager(sf::View* view);
    void addShape(sf::ConvexShape* newShape);
    void updateShadows(const sf::Vector2f& lightPos);
    void drawShadows(sf::RenderTarget& target);

private:
    std::vector<sf::ConvexShape*> mShadowCasters;
    std::vector<sf::ConvexShape*> mShadows;

    std::array<sf::Vector2f, 4> mScreenCorners;

    std::vector<sf::Vector2f> mDebugLines;
    std::vector<sf::Vector2f> mDebugPoints;

    sf::View* pView;

    sf::Vector2f lightPos;
};