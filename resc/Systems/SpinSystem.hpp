#pragma once
#include "Core/System.hpp"
#include "Core/Scene.hpp"

#include "resc/Components/CoreComponents.h"
#include "resc/Components/MovementComponents.h"
extern Scene scene;

class SpinSystem : public System {
    public:
        void update(float dt) {
            for(auto const& entity : entities) {
                auto& spin = scene.GetComponent<Spin>(entity);
                auto& transform = scene.GetComponent<Transform>(entity);
                transform.rotation[1] += spin.spinspeed * dt;
            }
        }
};