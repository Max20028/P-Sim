#pragma once
#include "Core/RobotMap.h"
//Note: Despite not being needed, windows.h makes clangd happy about d3d11. This is due to header linking and nothing intrinsic I think
#include <windows.h>
#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\d3d11.h> //NOLINT
struct Camera {
    bool isMainCamera;
    float VerticalFOV;
    float HorizontalFOV;
    float clearColor[4];
    float nearPlaneDist;
    float farPlaneDist;
};
struct CubeMesh {
    float color[4];
};
struct Rigidbody {
    bool isGravity;
    bool isKinematic;
    float mass;
    float gravity;
    float velocity[3];
    float acceleration[3];
    float drag;
    float angularDrag;
};
struct Transform {
    // DirectX::XMMATRIX translation;
    // DirectX::XMMATRIX rotation;
    // DirectX::XMMATRIX scale;
    float translation[3];
    float rotation[3];
    float scale[3];
};
struct SimpleMesh {
    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* vertBuffer;
    float difcolor[4];
    int stride;
    int offset;
    int indexLength;
};
struct Light {
    //0: Directional    1: Point    2: Spotlight
    int type;
    float cone;
    float range;
    float attenuation[3];

    float ambient[4];
    float diffuse[4];
};
struct FPSPlayerController {
    Entity* camera;
    float cameraSensitivity;
    
};
