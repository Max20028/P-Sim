#include "Core/System.hpp"
#include "Core/Scene.hpp"

#include "resc/Components/CoreComponents.h"
extern Scene scene;

class CameraSystem : public System {
    public:
        Entity mainCamera;
    void update(float dt) {

    }
    void initSystem() {

    }
    void initEntity(Entity) {
        
    }
};