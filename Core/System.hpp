#include <set>
#include "RobotMap.h"

class System {
    public:
        std::set<Entity> entities;

        virtual void initSystem();
        virtual void initEntity(Entity);
        virtual void update(float dt);
};