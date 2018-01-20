#include "GameObject.h"
#include "Utilities.h"
#include "NavigateToMouse.h"


GameObject::GameObject(const sf::Texture& tex, float w, float h)
: Kinematic()
, refTexture(tex)
{
	mShape.setSize(sf::Vector2f(w,h));
	mShape.setTexture(&refTexture);
	mShape.setOutlineColor(sf::Color::Yellow);
	CenterOrgin(mShape);
	mpCurrentState = NavigateToMouse::Instance();
	mpCurrentState->enter(this);
}

void GameObject::update(const sf::Time dTime)
{
	Kinematic::update(dTime);
	mpCurrentState->execute(this, dTime);
}

void GameObject::setPosition(const sf::Vector2f &newPos)
{
	mPosition = newPos;
}

const sf::Vector2f GameObject::getPosition()
{
	return mPosition;
}

void GameObject::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	/*
	///< Shows Quadrants bounds
	sf::RectangleShape outline;
	outline.setPosition(getPosition());
	outline.setSize(sf::Vector2f(mShape.getSize().x * getScale().x, mShape.getSize().y * getScale().y));
	outline.setOutlineColor(sf::Color::Red);
	outline.setFillColor(sf::Color::Transparent);
	outline.setOutlineThickness(1);
	CenterOrgin(outline);
	target.draw(outline);
	sf::CircleShape point;
	point.setPosition(getPosition());
	point.setFillColor(sf::Color::Green);
	point.setRadius(2);
	CenterOrgin(point);
	target.draw(point);
	//*/
	states.transform.translate(mPosition);
	
	target.draw(mShape, states);
}

void GameObject::changeState(AIState* pNewState)
{
	assert(mpCurrentState && pNewState);
	mpCurrentState->exit(this);
	mpCurrentState = pNewState;
	mpCurrentState->enter(this);
}


GameObject::~GameObject()
{
	
}
