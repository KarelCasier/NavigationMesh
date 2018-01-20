#pragma once
#include <SFML/System/Time.hpp>

class GameObject;

class AIState
{
public:
	virtual ~AIState(){};
	
	virtual void enter(GameObject* entity) = 0;
	virtual void execute(GameObject* entity, const sf::Time& dTime) = 0;
	virtual void exit(GameObject* entity) = 0;
};
