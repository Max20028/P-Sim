#ifndef ROBOTMAP
#define ROBOTMAP
#include <cstdint>
#include <bitset>

using Entity = std::uint64_t;
//uint8 is 0 to 255
using ComponentType = std::uint8_t;

using EntitySize_t = Entity;
using ComponentSize_t = ComponentType;

const Entity MAX_ENTITIES = 10000;
const ComponentType MAX_COMPONENTS = 64;

using Signature = std::bitset<MAX_COMPONENTS>;
#endif