#pragma once
#include <SFML/System.hpp>
#include "Utilities.h"

class Kinematic
{
public:
	struct Steering
	{
		sf::Vector2f	linear;
		double			angular;
	};
	
	Kinematic()
	: mSpeed(500)
	{}
	
	virtual ~Kinematic(){}
	
	void update(const sf::Time& dTime)
	{
		mPosition += mVelocity * dTime.asSeconds();
		mOrientation += mRotation * dTime.asSeconds();
		
		mVelocity += mSteering.linear * dTime.asSeconds();
		mOrientation += mSteering.angular * dTime.asSeconds();
		
		mHeading = Normalize(mVelocity);
	}
	
	sf::Vector2f	mPosition;
	sf::Vector2f	mVelocity;
	double			mOrientation;
	double			mRotation; //< Angular Velocity
	Steering		mSteering;
	double			mSpeed;
	sf::Vector2f	mHeading;
};
