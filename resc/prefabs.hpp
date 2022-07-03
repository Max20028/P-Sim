#include "Core/Scene.hpp"
#include "Core/RobotMap.h"
#include "resc/Components/CoreComponents.h"
#include "resc/Components/MovementComponents.h"
#include "Graphics/Renderer.h"
#include <fstream>
#include <string>
#include "resc/OBJImport.hpp"

extern Scene scene;
extern Renderer* renderer;

Entity createModelPrefab(std::string path) {
    Entity entity = scene.CreateEntity();
    //Add transform component
    scene.AddComponent(entity, Transform{
                                .translation={0.0f, 0.0f, 0.0f},
                                .rotation = {0.0f, 0.0f, 0.0f},
                                .scale = {2.0f, 2.0f, 2.0f}
                                });
    //Add Mesh Component
    SimpleMesh mesh;

    DWORD* indices;
    Vertex* verts;

    int numberInd;
    int numberVert;

    ImportObj(path, &verts, &indices, &numberVert, &numberInd);

    renderer->createBuffers(numberInd, numberVert, indices, verts, &mesh.indexBuffer, &mesh.vertBuffer);

    mesh.offset = 0;
    mesh.stride = sizeof(Vertex);
    mesh.indexLength = numberInd;
    mesh.difcolor[0] = 0.0f;
    mesh.difcolor[1] = 1.0f;
    mesh.difcolor[2] = 1.0f;
    mesh.difcolor[3] = 1.0f;

    scene.AddComponent(entity, mesh);

    //Add Rigidbody component
    scene.AddComponent(entity, Rigidbody{
                                    .isGravity = false,
                                    .isKinematic = true,
                                    .mass = 1.0f,
                                    .gravity = -1.0f
                                    });
    //Add SPIN
    scene.AddComponent(entity, Spin{.spinspeed=0.5});
    return entity;
}

Entity createCameraPrefab() {
    Entity entity = scene.CreateEntity();
    //Add transform component
    scene.AddComponent(entity, Transform{
                                .translation={0.0f, /*3.0f*/0.0f, -8.0f},
                                .rotation = {0.0f, 0.0f, 0.0f},
                                .scale = {1.0f, 1.0f, 0.0f}
                                });
    //Add camera component
    //TODO: HORIZONTAL FOV MEANS NOTHING
    scene.AddComponent(entity, Camera{
                                .isMainCamera = true,
                                .VerticalFOV = 0.4*3.14,
                                .HorizontalFOV = 1,
                                .clearColor = {0.2f, 0.2f, 0.2f, 2.0f},
                                .nearPlaneDist = 0.1f,
                                .farPlaneDist = 1000.0f
                                });
    return entity;
}

Entity createPointLight(float x, float y, float z, float ambient, float diffuse, float range, float attenuation[3]) {
    Entity entity = scene.CreateEntity();
    scene.AddComponent(entity, Transform{
                                .translation={x,y,z},
                                .rotation{0.0f, 0.0f, 0.0f},
                                .scale={1.0f, 1.0f, 1.0f}
                                });
    Light light;
    light.ambient[0] = ambient;
    light.ambient[1] = ambient;
    light.ambient[2] = ambient;
    light.ambient[3] = 1.0f;

    light.range = range;

    light.attenuation[0] = attenuation[0];
    light.attenuation[1] = attenuation[1];
    light.attenuation[2] = attenuation[2];

    light.diffuse[0] = diffuse;
    light.diffuse[1] = diffuse;
    light.diffuse[2] = diffuse;

    light.type = 1;

    scene.AddComponent(entity, light);

    return entity;
}
