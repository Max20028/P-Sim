#pragma once
#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\d3d11.h>
struct Camera {
    bool isMainCamera;
    float VerticalFOV;
    float HorizontalFOV;
    float clearColor[4];
    float nearPlaneDist;
    float farPlaneDist;
};
struct CubeMesh {
    // float width; //x
    // float height; //y
    // float depth; //z
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