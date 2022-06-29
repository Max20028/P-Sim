#include "Graphics/Renderer.h"
#include "Core/RobotMap.h"
#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\directxmath.h>
#include <iostream>
#include <fstream>
#include <memory>

#include "Core/Scene.hpp"
#include "resc/Components/CoreComponents.h"
#include "resc/Components/MovementComponents.h"
#include "resc/Systems/PhysicsSystem.hpp"
#include "resc/Systems/CameraSystem.hpp"
#include "resc/Systems/SimpleMeshSystem.hpp"
#include "resc/Systems/SpinSystem.hpp"
#include "resc/prefabs.hpp"

HWND hwnd;

Renderer* renderer;

Scene scene;
std::shared_ptr<PhysicsSystem> physicsSystem;
std::shared_ptr<CameraSystem> cameraSystem;
std::shared_ptr<SimpleMeshSystem> simpleMeshSystem;
std::shared_ptr<SpinSystem> spinSystem;
std::vector<Entity> entities;

// Renderer::CameraDetails camdets;
// Renderable cubeRender;


void ImportObj(std::string filepath, Vertex** vert, DWORD** ind, int* nvert, int* nind);

void updateGame(float dt, int tps) {
    //Update Input
    renderer->DetectInput(dt, hwnd);
    //Run update code here
    physicsSystem->update(dt);
    spinSystem->update(dt);


    //Create camera details from the main camera
    Camera mainCamera = scene.GetComponent<Camera>(cameraSystem->mainCamera);
    Transform cameraTransform = scene.GetComponent<Transform>(cameraSystem->mainCamera);
    Renderer::CameraDetails camdets;
    camdets.backColor[0] = mainCamera.clearColor[0];
    camdets.backColor[1] = mainCamera.clearColor[1];
    camdets.backColor[2] = mainCamera.clearColor[2];
    camdets.backColor[3] = mainCamera.clearColor[3];
    camdets.position[0] = cameraTransform.translation[0];
    camdets.position[1] = cameraTransform.translation[1];
    camdets.position[2] = cameraTransform.translation[2];
    camdets.rotation[0] = cameraTransform.rotation[0];
    camdets.rotation[1] = cameraTransform.rotation[1];
    camdets.rotation[2] = cameraTransform.rotation[2];
    camdets.nearPlaneDist = mainCamera.nearPlaneDist;
    camdets.farPlaneDist = mainCamera.farPlaneDist;
    camdets.verticalFOV = mainCamera.VerticalFOV;

    //Render stuff
    renderer->startRenderFrame(camdets);
    float rot;
    //Render the simple meshes
    for(auto entity : simpleMeshSystem->entities) {
        SimpleMesh mesh = scene.GetComponent<SimpleMesh>(entity);
        Transform transform = scene.GetComponent<Transform>(entity);
        Renderable render;
        render.difColor = DirectX::XMFLOAT4(mesh.difcolor);
        render.IndexBuffer = mesh.indexBuffer;
        render.indexLength = mesh.indexLength;
        rot = transform.rotation[1];
        render.offset = mesh.offset;
        render.stride = mesh.stride;
        render.VertBuffer = mesh.vertBuffer;
        render.World = DirectX::XMMatrixScaling(transform.scale[0], transform.scale[1], transform.scale[2])
                        * DirectX::XMMatrixRotationX(transform.rotation[0]) * DirectX::XMMatrixRotationY(transform.rotation[1]) * DirectX::XMMatrixRotationZ(transform.rotation[2])
                        * DirectX::XMMatrixTranslation(transform.translation[0], transform.translation[1], transform.translation[2]);

        renderer->renderObject(render);
    }
    std::wostringstream debugString;
    debugString << L"TPS: " << tps << L"SPIN: " << rot;
    renderer->finishRenderFrame(debugString.str());
}

void startGame(HWND hwn, HINSTANCE hInstance) {
    renderer = new Renderer();
    hwnd = hwn;
    renderer->InitRenderer(hwnd, hInstance);

    //Init all the stuff for the scene
    scene.Init();
    //Init Components
    scene.RegisterComponent<Camera>();
    scene.RegisterComponent<Transform>();
    scene.RegisterComponent<CubeMesh>();
    scene.RegisterComponent<Rigidbody>();
    scene.RegisterComponent<SimpleMesh>();
    scene.RegisterComponent<Spin>();
    //Init Systems

    //Physics System
        physicsSystem = scene.RegisterSystem<PhysicsSystem>();
        Signature physicssignature;
        physicssignature.set(scene.GetComponentType<Transform>());
        physicssignature.set(scene.GetComponentType<Rigidbody>());
        scene.SetSystemSignature<PhysicsSystem>(physicssignature);
    //Camera System
        cameraSystem = scene.RegisterSystem<CameraSystem>();
        Signature camerasignature;
        camerasignature.set(scene.GetComponentType<Camera>());
        camerasignature.set(scene.GetComponentType<Transform>());
        scene.SetSystemSignature<CameraSystem>(camerasignature);
    //SimpleMesh System
        simpleMeshSystem = scene.RegisterSystem<SimpleMeshSystem>();
        Signature simplemeshsignature;
        simplemeshsignature.set(scene.GetComponentType<Transform>());
        simplemeshsignature.set(scene.GetComponentType<SimpleMesh>());
        scene.SetSystemSignature<SimpleMeshSystem>(simplemeshsignature);
    //Spin System
        spinSystem = scene.RegisterSystem<SpinSystem>();
        Signature spinsignature;
        spinsignature.set(scene.GetComponentType<Transform>());
        spinsignature.set(scene.GetComponentType<Spin>());
        scene.SetSystemSignature<SpinSystem>(spinsignature);

    //Create Monkey
    entities.push_back(createModelPrefab("resc/misc/monkey.obj"));

    //Create Camera
    entities.push_back(createCameraPrefab());
    cameraSystem->mainCamera = entities.back();

    // SimpleModel monkey;

    // DWORD* indices;
    // Vertex* verts;

    // int numberInd;
    // int numberVert;

    // ImportObj("resc/misc/monkey.obj", &verts, &indices, &numberVert, &numberInd);

    // renderer->createBuffers(numberInd, numberVert, indices, verts, &monkey.indexBuffer, &monkey.vertBuffer);


    // camdets.backColor[0] = 0.2f;
    // camdets.backColor[1] = 0.2f;
    // camdets.backColor[2] = 0.2f;
    // camdets.backColor[3] = 1.0f;

    // camdets.camTarget[0] = 0.0f;
    // camdets.camTarget[1] = 0.0f;
    // camdets.camTarget[2] = 0.0f;

    // camdets.camUp[0] = 0.0f;
    // camdets.camUp[1] = 1.0f;
    // camdets.camUp[2] = 0.0f;

    // camdets.farPlaneDist = 1000;
    // camdets.nearPlaneDist = 1;

    // camdets.position[0] = 0;
    // camdets.position[1] = 3;
    // camdets.position[2] = -8;

    // camdets.verticalFOV = 0.4*3.14;


    // cubeRender.difColor = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

    // cubeRender.IndexBuffer = monkey.indexBuffer;
    // cubeRender.VertBuffer = monkey.vertBuffer;

    // cubeRender.offset = 0;
    // cubeRender.stride = sizeof(Vertex);

    // cubeRender.indexLength = numberInd;
    // DirectX::XMVECTOR rotaxis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    // DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(rotaxis, rot);
    // cubeRender.World = DirectX::XMMatrixTranslation( 0.0f, 0.0f, 4.0f) * Rotation;
}

void endGame(HWND hwnd) {
    renderer->EndRenderer(hwnd);
}

