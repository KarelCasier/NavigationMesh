#include "Player.h"
#include "Utilities.h"
#include "Game.h"
#include <cmath>

#include "GlobalConstants.hpp"

Player::Player():
box(sf::Vector2f(20,20))
{
    CenterOrgin(box);
    
}

void Player::update(const sf::Time& dTime)
{
    ///< Position Calculations
    sf::Vector2f prevPosition = getPosition();
    sf::Vector2f prevVelocity = mVelocity;
    
    mVelocity += mAcceleration * dTime.asSeconds();
    mVelocity *= gFriction;
    sf::Vector2f nextPosition = prevPosition + (prevVelocity + mVelocity) * dTime.asSeconds() / 2.0f;
    
    mVelocity = (nextPosition - prevPosition) / dTime.asSeconds();
    
    setPosition(nextPosition);
	
	///< Rotation Calculations
	sf::Vector2f playerToCamDiff = TheGame::Instance()->getWorldView()->getCenter() - getPosition();
	
	setRotation( atan2f(playerToCamDiff.y, playerToCamDiff.x) * 180 / M_PI);
   
}

void Player::handleInput(const sf::Event &e)
{
    if (e.type == sf::Event::MouseButtonPressed)
    {
        
    }
    
    if (e.type == sf::Event::KeyPressed)
    {
        
    }
	
}

void Player::handleRealtimeInput()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
    {
        mVelocity.x += 150.f;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
    {
        mVelocity.x -= 150.f;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
    {
        mVelocity.y -= 150.f;
    }
    
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
    {
        mVelocity.y += 150.f;
    }
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    
    states.transform *= getTransform();
    
    target.draw(box, states);
}