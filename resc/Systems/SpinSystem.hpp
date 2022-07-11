#pragma once
#include "Core/System.hpp"
#include "Core/Scene.hpp"
#include "Core/EventManager.hpp"
#include <iostream>

#include "resc/Components/CoreComponents.h"
#include "resc/Components/MovementComponents.h"
extern Scene scene;
extern EventManager* eventManager;

class SpinSystem : public System {
    public:
        void start() {
            //eventManager->subscribe([this](ToggleSpinEvent tse){toggleSpin(tse);});
            //eventManager->subscribe(std::bind(&SpinSystem::toggleSpin, this, std::placeholders::_1));
            eventManager->subscribe(this, &SpinSystem::toggleSpin);
        }
        void update(float dt) {
            for(auto const& entity : entities) {
                auto& spin = scene.GetComponent<Spin>(entity);
                auto& transform = scene.GetComponent<Transform>(entity);
                if(spin.doSpin)
                    transform.rotation[1] += spin.spinspeed * dt;
            }
        }
        void toggleSpin(ToggleSpinEvent* tse) {
            for(auto entity : entities) {
                auto& spin = scene.GetComponent<Spin>(entity);
                spin.doSpin = !spin.doSpin;
            }
        }
};
