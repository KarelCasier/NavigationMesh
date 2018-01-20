#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Player : public sf::Drawable, public sf::Transformable {
public:
    Player();

    void update(const sf::Time& dTime);

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    void handleInput(const sf::Event& e);
    void handleRealtimeInput();

    void setVelocity(const sf::Vector2f& vel) { mVelocity = vel; }
    sf::Vector2f getVelocity() { return mVelocity; }

    void setAcceleration(const sf::Vector2f& accel) { mAcceleration = accel; }
    sf::Vector2f getAcceleration() { return mAcceleration; }

private:
    sf::Vector2f mVelocity;
    sf::Vector2f mAcceleration;

    sf::RectangleShape box;
};