#pragma once
#include <cstdint>
#include <bitset>

#ifndef ROBOTMAP
#define ROBOTMAP
using Entity = std::uint64_t;
//uint8 is 0 to 255
using ComponentType = std::uint8_t;

using EntitySize_t = Entity;
using ComponentSize_t = ComponentType;

const Entity MAX_ENTITIES = 10000;
const ComponentType MAX_COMPONENTS = 64;

using Signature = std::bitset<MAX_COMPONENTS>;

#define INITIAL_SCREEN_WIDTH  1920
#define INITIAL_SCREEN_HEIGHT 1080

#define INITIALLY_WINDOWED true
#define MAX_LIGHTS 16
#define MAX_CONTROLLERS 4
#define MAX_MOUSE_BUTTONS 24

#define FPS_CAP 60

extern int ClientWidth;
extern int ClientHeight;
#endif
