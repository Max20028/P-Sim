#pragma once
#include "RobotMap.h"
#include "System.hpp"

#include <memory>
#include <unordered_map>

class SystemManager {
    public: 
        template<typename T>
        std::shared_ptr<T> RegisterSystem() {
            const char* typeName = typeid(T).name();
            
            assert(systems.find(typeName) == systems.end() && "Resgistering system that already exists");

            auto system = std::make_shared<T>();
            systems.insert({typeName, system});
            return system;
        }

        template<typename T>
        void SetSignature(Signature signature) {
            const char* typeName = typeid(T).name();
            
            assert(systems.find(typeName) != systems.end() && "Set signature used before system registered");

            signatures.insert({typeName, signature});
        }

        void EntityDestroyed(Entity entity) {
            //Erase a destroyed entity from all system lists
            //entites is a set so no check needed
            for(auto const& pair : systems) {
                auto const& system = pair.second;
                system->entities.erase(entity);
            }
        }

        void EntitySignatureChanged(Entity entity, Signature entitySignature) {
            //Notify each system that an entity's signautre changed
            for(auto const& pair : systems) {
                auto const& type = pair.first;
                auto const& system = pair.second;
                auto const& systemSignature = signatures[type];

                //Entity signature matches system signature then insert into set
                if((entitySignature & systemSignature) == systemSignature) {
                    system->entities.insert(entity);
                } else {
                    //Entity signature does not match system, erase from set
                    system->entities.erase(entity);
                }
            }
        }

    private: 
        //Map from system type string to signature
        std::unordered_map<const char*, Signature> signatures{};

        //Map from system type string to a system pointer
        std::unordered_map<const char*, std::shared_ptr<System>> systems{};
};