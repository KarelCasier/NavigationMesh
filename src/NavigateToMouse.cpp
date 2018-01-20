#include "NavigateToMouse.h"

#include "Game.h"
#include <SFML/Graphics.hpp>
#include "Utilities.h"
#include "math.h"
#include "GameObject.h"
#include <vector>

void NavigateToMouse::enter(GameObject* entity)
{
	//std::cout << "Follow Mouse AI Activated" << std::endl;
}

void NavigateToMouse::execute(GameObject* entity, const sf::Time& dTime)
{
	sf::RenderWindow* pWin = TheGame::Instance()->getWindow();
	sf::View* pView = TheGame::Instance()->getWorldView();
	sf::Vector2f mousePos = pWin->mapPixelToCoords(sf::Mouse::getPosition(*pWin), *pView);
	
	std::vector<sf::Vector2f> path = TheGame::Instance()->getNavMesh()->GetNavPath(entity->mPosition, mousePos);
	sf::Vector2f& imedTarget = path.at(1);

	double angle = atan2(imedTarget.y - entity->mPosition.y, imedTarget.x - entity->mPosition.x);
	entity->mVelocity = sf::Vector2f(cos(angle)*entity->mSpeed, sin(angle)*entity->mSpeed);
	//std::cout << angle << std::endl;
	
}

void NavigateToMouse::exit(GameObject* entity)
{
	//std::cout << "Follow Mouse AI Deactivated" << std::endl;
}

NavigateToMouse::~NavigateToMouse()
{
	
}
