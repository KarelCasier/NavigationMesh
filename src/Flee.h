#pragma once
#include "AIState.h"

class Flee : public AIState {
private:
    Flee() {}

public:
    ///< Singleton
    static Flee* Instance()
    {
        static Flee instance;
        return &instance;
    }

    virtual ~Flee();

    virtual void enter(GameObject* entity);
    virtual void execute(GameObject* entity, const sf::Time& dTime);
    virtual void exit(GameObject* entity);
};