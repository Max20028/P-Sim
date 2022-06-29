#include "Core/System.hpp"
#include "Core/Scene.hpp"

#include "resc/Components/CoreComponents.h"
extern Scene scene;

class PhysicsSystem : public System {
    public:
        void update(float dt) {
            for(auto const& entity : entities) {
                auto& rigidBody = scene.GetComponent<Rigidbody>(entity);
                auto& transform = scene.GetComponent<Transform>(entity);
                if(rigidBody.isKinematic) {
                    transform.translation[0] += rigidBody.velocity[0] * dt;
                    transform.translation[1] += rigidBody.velocity[1] * dt;
                    transform.translation[2] += rigidBody.velocity[2] * dt;

                    if(rigidBody.isGravity)
                        rigidBody.velocity[1] += rigidBody.mass * rigidBody.gravity * dt;
                }
            }
        }
    void initSystem() {

    }
    void initEntity(Entity) {
        
    }
};