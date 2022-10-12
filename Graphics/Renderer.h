#pragma once
//Basic Windows Headers
#include <windows.h>
#include <windowsx.h>

//DirectX Headers
#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\d3d11.h>
#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\directxmath.h>
#include <dwrite.h>
#include <d2d1_1.h>
#include <dinput.h>
#include <d3dcompiler.h>    //Shader compiler

//Misc Not Me Imports
#include <sstream>          // Used for the 2d drawn string
#include <vector>           //Vector is so easy to use

//Engine Config
#include "Core/Robotmap.h"
#include "resc/Components/CoreComponents.h"

struct Renderable {
    ID3D11Buffer* IndexBuffer;
    // std::vector<DWORD> indexList;
    ID3D11Buffer* VertBuffer;
    // std::vector<DWORD> vertList;
    UINT stride;
    UINT offset;
    DirectX::XMMATRIX World;
    DirectX::XMFLOAT4 difColor;

    int indexLength;
};



struct Vertex    //Overloaded Vertex Structure
{
    Vertex(){}
    Vertex(float x, float y, float z,
        float u, float v,
        float nx, float ny, float nz,
        float tx, float ty, float tz)
        : pos(x,y,z), texcoord(u, v), normal(nx, ny, nz),
        tangent(tx, ty, tz){}
    Vertex(float x, float y, float z) : pos(x,y,z), texcoord(0.0f,0.0f), normal(0.0f,0.0f,0.0f), tangent(0.0f,0.0f,0.0f){}

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 texcoord;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT3 tangent;
    DirectX::XMFLOAT3 biTangent;

    // Will not be sent to shader
    int StartWeight;
    int WeightCount;
};

class Renderer {
public:
    //TODO: Figure this out
struct CameraDetails {
    float backColor[4];
    float position[3];
    float verticalFOV;
    float nearPlaneDist;
    float farPlaneDist;
    // float camTarget[3];
    // float camUp[3];
    float rotation[3];
};
//Lights stuff
struct RenderLight
{
    RenderLight()
    {
        ZeroMemory(this, sizeof(RenderLight));
    }
    DirectX::XMFLOAT3 dir;
    float cone;
    DirectX::XMFLOAT3 pos;
    float range;
    DirectX::XMFLOAT3 att;
    int type;
    DirectX::XMFLOAT4 ambient;
    DirectX::XMFLOAT4 diffuse;
};
    //This is the function that starts all the 3d stuff
    void InitRenderer(HWND, HINSTANCE);

    void updateLights(RenderLight light[MAX_LIGHTS], int numLights);

    void EndRenderer(HWND hwnd);

    void startRenderFrame(CameraDetails camDets);

    void renderObject(Renderable renderable);

    void finishRenderFrame(std::wstring debug_string);

    void createBuffers(int numIndices, int numVerts, std::vector<DWORD> indices, std::vector<Vertex> vertices, ID3D11Buffer** indexBuffer, ID3D11Buffer** vertBuffer);
    void createBuffers(int numIndices, int numVerts, DWORD* indices, Vertex* vertices, ID3D11Buffer** indexBuffer, ID3D11Buffer** vertBuffer);

private:

    void InitD3D(HWND hwnd, HINSTANCE hInstance);
    void CleanD3D(HWND hwnd);
    void InitPipeline();
    void drawstuff(std::wstring instr);
//Key Variables
//TODO: Organize these into a struct or something
//These are the windows handles to the window and process. Important for binding d3d to the window
//Only storing this here as in the future may want multiple windows or some shit
// HWND hwnd;
// HINSTANCE hInstance;



struct cbPerObject
{
    DirectX::XMMATRIX  WVP;
    DirectX::XMMATRIX  World;
    DirectX::XMFLOAT4 difColor;
    BOOL hasTexture;
    //Because of HLSL structure packing, we will use windows BOOL
    //instead of bool because HLSL packs things into 4 bytes, and
    //bool is only one byte, where BOOL is 4 bytes
    BOOL hasNormMap;
};
struct cbPerFrame
{
    int numberLights;
    DirectX::XMINT3 pad;
    RenderLight light[MAX_LIGHTS];
};


    IDXGISwapChain *swapchain;                // the pointer to the swap chain interface
    ID3D11Device *dev;                        // the pointer to our Direct3D device interface
    ID3D11DeviceContext *devcon;              // the pointer to our Direct3D device context
    ID3D11RenderTargetView *backbuffer;       // the pointer to our back buffer
    ID3D11DepthStencilView* depthStencilView; // Similar to below, not 100% sure the difference
    ID3D11Texture2D* depthStencilBuffer;      // Stores the depth buffer, essentially all the depths across the screen I think
    ID3D11VertexShader *pVS;                  // the pointer to our vertex shader
    ID3D11PixelShader *pPS;                   // the pointer to our pixel shader
    ID3D11InputLayout *pLayout;               // the pointer to the input layout of the vertex
    ID3D11BlendState* Transparency;           // the pointer to our transparency rendering state
    ID3D11Buffer* cbPerObjectBuffer;          // Data sent to shader updated per object
    cbPerObject cbPerObj;                     // CPU side storage for PerObjectBuffer
    ID3D11Buffer* cbPerFrameBuffer;           // Data sent to shader updated once per frame
    cbPerFrame constbuffPerFrame;             // CPU side storage for PerFrameBuffer
    ID3D11RasterizerState* WireFrame;         //Rasterizer state that saves how to render things
    ID3D11SamplerState* TexSamplerState;      //Tells the GPU how to read a texture

    ID3D11RasterizerState* CCWcullMode;
    ID3D11RasterizerState* CWcullMode;
    ID3D11RasterizerState* NoCullMode;

    ID2D1Factory *d2dfactory;                 // the pointer to our 2d rendering factory
    ID2D1RenderTarget* d2drendertarget;       // the pointer to our 2d rendering target

    IDWriteFactory* dwritefactory;            // the pointer to our 2d text factory (This is window's way of drawing text on d2d surface)
    IDWriteTextFormat* dwritetextformat;      // the format specifier for dwrite drawing
    ID2D1SolidColorBrush* blackbrush;         // the solid black brush for drawing black text

    //Camera Stuff
    DirectX::XMMATRIX camView;
    DirectX::XMMATRIX camProjection;

};
