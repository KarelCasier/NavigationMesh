#include "FollowMouse.h"

#include "Game.h"
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "Flee.h"
#include "math.h"
#include "GameObject.h"

void FollowMouse::enter(GameObject* entity)
{
    // std::cout << "Follow Mouse AI Activated" << std::endl;
}

void FollowMouse::execute(GameObject* entitiy, const sf::Time& dTime)
{
    sf::RenderWindow* pWin = TheGame::Instance()->getWindow();
    sf::View* pView = TheGame::Instance()->getWorldView();
    sf::Vector2f mousePos = pWin->mapPixelToCoords(sf::Mouse::getPosition(*pWin), *pView);

    double angle = atan2(mousePos.y - entitiy->mPosition.y, mousePos.x - entitiy->mPosition.x);
    entitiy->mVelocity = sf::Vector2f(cos(angle) * entitiy->mSpeed, sin(angle) * entitiy->mSpeed);
    // std::cout << angle << std::endl;

    if (Distance(mousePos, entitiy->mPosition) < 50) {
        entitiy->changeState(Flee::Instance());
    }
}

void FollowMouse::exit(GameObject* entity)
{
    // std::cout << "Follow Mouse AI Deactivated" << std::endl;
}

FollowMouse::~FollowMouse() {}
