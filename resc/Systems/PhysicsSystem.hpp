#include "Core/System.hpp"
#include "Core/Scene.hpp"

#include "resc/Components/RigidBody.hpp"
#include "resc/Components/Transform.hpp"
extern Scene scene;

class PhysicsSystem : public System {
    void update(float dt) override {
        for(auto const& entity : entities) {
            auto& rigidBody = scene.GetComponent<Rigidbody>(entity);
            auto& transform = scene.GetComponent<Transform>(entity);

            transform.translation[0] += rigidBody.velocity[0] * dt;
            transform.translation[1] += rigidBody.velocity[1] * dt;
            transform.translation[2] += rigidBody.velocity[2] * dt;

            rigidBody.velocity[1] += rigidBody.gravity * dt;
        }
    }
};