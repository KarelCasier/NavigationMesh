#include "Flee.h"

#include "Game.h"
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "FollowMouse.h"
#include "math.h"
#include "GameObject.h"

void Flee::enter(GameObject* entity)
{
    // std::cout << "Flee AI Activated" << std::endl;
}

void Flee::execute(GameObject* entitiy, const sf::Time& dTime)
{
    sf::RenderWindow* pWin = TheGame::Instance()->getWindow();
    sf::View* pView = TheGame::Instance()->getWorldView();
    sf::Vector2f mousePos = pWin->mapPixelToCoords(sf::Mouse::getPosition(*pWin), *pView);

    double angle = atan2(mousePos.y - entitiy->mPosition.y, mousePos.x - entitiy->mPosition.x);
    entitiy->mVelocity = sf::Vector2f(-cos(angle) * entitiy->mSpeed, -sin(angle) * entitiy->mSpeed);

    if (Distance(mousePos, entitiy->mPosition) > 500) {
        entitiy->changeState(FollowMouse::Instance());
    }
}

void Flee::exit(GameObject* entity)
{
    // std::cout << "Flee AI Deactivated" << std::endl;
}

Flee::~Flee() {}
