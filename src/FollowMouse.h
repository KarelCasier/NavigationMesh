#pragma once
#include "AIState.h"

class FollowMouse : public AIState {
private:
    FollowMouse() {}

public:
    ///< Singleton
    static FollowMouse* Instance()
    {
        static FollowMouse instance;
        return &instance;
    }

    virtual ~FollowMouse();

    virtual void enter(GameObject* entity);
    virtual void execute(GameObject* entity, const sf::Time& dTime);
    virtual void exit(GameObject* entity);
};
