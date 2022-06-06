#include "RobotMap.h"
#include <array>
#include <unordered_map>
#include <memory>

// The one instance of virtual inheritance in the entire implementation.
// An interface is needed so that the ComponentManager
// can tell a generic ComponentArray that an entity has been destroyed
// and that it needs to update its array mappings.
// Go back to Austin Morlan's Implementation for more details on why this is BAD!
// And the alternatives he presents
class IComponentArray
{
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity entity) = 0;
};

template <typename T>
class ComponentArray : IComponentArray {
    public: 
        void InsertData(Entity ent, T component) {
            assert(mapEntityToIndex.find(ent) == mapIndextoEntity.end() && "Component added to same entity more than once.");

            //Insert the component data at the end and update the maps
            EntitySize_t newIndex = size;
            mapEntityToIndex[ent] = newIndex;
            mapIndextoEntity[newIndex] = entity;
            componentArray[newIndex] = component;

            size++;
        }

        void RemoveData(Entity ent) {
            assert(mapEntityToIndex.find(entity) != mapEntityToIndex.end() && "Removing component that does not exist");

            //Copy last element into deleted element's place. Maintains Density
            EntitySize_t removedEntIndex = mapEntityToIndex[entity];
            EntitySize_t lastElementIndex = size-1;
            componentArray[removedEntIndex] = componentArray[lastElementIndex];

            //Update maps
            Entity lastEntity = mapIndextoEntity[lastElementIndex];
            mapEntityToIndex[lastElementIndex] = removedEntIndex;
            mapIndextoEntity[removedEntIndex] = lastEntity;

            mapEntityToIndex.erase(entity);
            mapIndextoEntity.erase(lastElementIndex);

            size--;
        }

        T& GetData(Entity ent) {
            assert(mapEntityToIndex.find(entity) != mapEntityToIndex.end() && "Retrieving component that does not exist");

            return componentArray[mapEntityToIndex[entity]];
        }

        void EntityDestroyed(Entity ent) override{
            if(mapEntityToIndex.find(entity) != mapEntityToIndex.end()) 
                RemoveData(entity);
        }

    private: 
    //Array of component T
    std::array<T, MAX_ENTITIES> componentArray;

    //Map from entity id to array index
    std::unordered_map<Entity, EntitySize_t> mapEntityToIndex;

    //Map from array index to entity id
    std::unordered_map<EntitySize_t, Entity> mapIndextoEntity;

    //NOTE: In this function there are instances of EntitySize_t, this is instead of Austin's size_t that he does not explain. I think this is what it means though
    //Number of component T in array
    EntitySize_t size;
};

class ComponentManager {
    public: 
        template<typename T>
        void RegisterComponent() {
            const char* typeName = typeid(T).name();

            assert(componentTypes.find(typename) == componentTypes.end() && "Registering component type more than once");

            //Add component type to map
            componentTypes.insert({typeName, nextComponentType});

            //Create componentarray pointer and add it to the component arrays map
            componentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});

            nextComponentType++;
        }

        template<typename T>
        ComponentType GetComponentType() {
            const char* typeName = typeid(T).name();

            assert(componentTypes.find(typeName) != componentTypes.end() && "Component not registered before use");

            return componentTypes[typeName];
        }

        template<typename T>
        void AddComponent(Entity entity, T component) {
            GetComponentArray<T>()->InsertData(entity, component);
        }

        template<typename T>
        void RemoveComponent(Entity entity) {
            GetComponentArray<T>()->RemoveData(entity);
        }

        template<typename T>
        T& GetComponent(Entity entity) {
            return GetComponentArray<T>()->GetData(entity);
        }

        void EntityDestroyed(Entity entity) {
            //Notify each component array that an entity has been destroyed
            //If it has that component it will remove it
            for(auto const& pair : componentArrays) {
                auto const& component = pair.second;
                component->EntityDestroyed(entity);
            }
        }

    private: 
        //map from string to a component type
        std::unordered_map<const char*, ComponentType> componentTypes{};

        //Map from string to a component array
        std::unordered_map<const char*, std::shared_ptr<IComponentArray>> componentArrays{};

        //The next component type to hand out
        ComponentType nextComponentType{};

        template<typename T>
        std::shared_ptr<ComponentArray<T>> GetComponentArray() {
            const char* typeName = typeid(T).name();

            assert (mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use");

            return std::static_pointer_cast<ComponentArray<T>>(componentArrays[typeName]);
        }

};