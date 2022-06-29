#pragma once
#include "RobotMap.h"
#include <queue>
#include <array>
#include <cassert>

class EntityManager {
public:
    EntityManager() {
        //Fill the available entity queue with every possible entity id
        for(Entity i = 0; i < MAX_ENTITIES; i++) {
            availableEntityIDs.push(i);
        }
    }

    Entity CreateEntity() {
        assert(activeEntityCount < MAX_ENTITIES && "Tried to Create Entity When Too Many Active Entities");

        Entity ent = availableEntityIDs.front();
        availableEntityIDs.pop();
        activeEntityCount++;

        return ent;
    }

    void DestroyEntity(Entity ent) {
        assert(ent < MAX_ENTITIES && "Tried to destroy Entity outside of max entities");

        signatures[ent].reset();

        availableEntityIDs.push(ent);
        activeEntityCount--;

        return;
    }

    void SetSignature(Entity ent, Signature sig) {
        assert(ent < MAX_ENTITIES && "Tried to set signature of entity outside of max entities");

        signatures[ent] = sig;
    }

    Signature GetSignature(Entity ent) {
        assert(ent < MAX_ENTITIES && "Tried to get signature of entity outside of max entities");
        
        return signatures[ent];
    }

private:
    //List of all available entity ids
    std::queue<Entity> availableEntityIDs;

    //Stores the signatures for every entity
    std::array<Signature, MAX_ENTITIES> signatures;

    //Number of Entity IDs that are in use
    EntitySize_t activeEntityCount = 0;
};