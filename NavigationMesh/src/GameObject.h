#pragma once
#include <SFML/Graphics.hpp>
#include "Kinematic.h"
#include "AIState.h"
#include <assert.h>

class GameObject : public sf::Drawable, public Kinematic
{
public:
	GameObject(const sf::Texture& tex, float w, float h);
	virtual ~GameObject();
	
public:
	virtual void update(const sf::Time dTime);
	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	
	void setPosition(const sf::Vector2f& newPos);
	
	void changeState(AIState* pNewState);
	
private:
	const sf::Texture&	refTexture;
	sf::RectangleShape	mShape;
	AIState*			mpCurrentState;
	
};