#pragma once
#include "AIState.h"

class NavigateToMouse : public AIState {
    NavigateToMouse() {}

public:
    ///< Singleton
    static NavigateToMouse* Instance()
    {
        static NavigateToMouse instance;
        return &instance;
    }

    ~NavigateToMouse();

    virtual void enter(GameObject* entity);
    virtual void execute(GameObject* entity, const sf::Time& dTime);
    virtual void exit(GameObject* entity);
};
