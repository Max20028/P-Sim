#ifndef UNICODE
#define UNICODE
#endif

#ifdef DEBUG
#define WIREFRAME
#endif


#include <stdio.h>
#include <initguid.h> //Make COM happy with mingw
#include <sstream>
#include <vector>
#include <fstream>
#include <istream>
#include <string>
#include <algorithm>
#include <iostream>

// include the basic windows header files and the Direct3D header files
#include <windows.h>
#include <windowsx.h>

#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\d3d11.h>
#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\directxmath.h>
#include <dwrite.h>
#include <d2d1_1.h>
#include <dinput.h>
// #include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\dxgi.h>

#include "headers/WICTextureLoader.h"

// #include <xnamath.h>
// #include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\d3dx11.h>
// #include <d3dx10.h>
#include <d3dcompiler.h>    //Shader compiler
// #include <C:\Users\tpbur\Documents\GitHub\ThirdParty\DirectXTK\Inc\WICTextureLoader.h>
// #include <C:\Users\tpbur\Documents\GitHub\ThirdParty\DirectXTex\WICTextureLoader\WICTextureLoader11.h>


// include the Direct3D Library file
// So, since I am using g++, i cant use these auto linkers. These libraries must be specified through the command line,
//  and thus through the Makefile. See the LIBFLAGS for the format, but essentialy -L adds a search directory and -l adds the lib
//      #pragma comment (lib, "C:\\msys64\\mingw64\\x86_64-w64-mingw32\\lib\\d3d11.a")
//      #pragma comment(linker, "C:\\msys64\\mingw64\\x86_64-w64-mingw32\\lib\\d3d11.a -verbose")
//      #pragma comment (lib, "d3dx11.lib")
//      #pragma comment (lib, "d3dx10.lib")

// define the screen resolution 800x600
#define SCREEN_WIDTH  1920
#define SCREEN_HEIGHT 1080

using namespace DirectX;

// global declarations
IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
IDXGIFactory *dxgifactory;
ID2D1Factory *d2dfactory;
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;    // global declaration
ID3D11VertexShader *pVS;    // the vertex shader
ID3D11PixelShader *pPS;     // the pixel shader
// ID3D11Buffer *pVBuffer;     // GPU input buffer
ID3D11InputLayout *pLayout; // GPU Input Layout
ID3D11Buffer* squareIndexBuffer;
ID3D11Buffer* squareVertBuffer;
ID3D11DepthStencilView* depthStencilView; //Similar to below, not 100% sure the difference
ID3D11Texture2D* depthStencilBuffer; //Stores the depth buffer, essentially all the depths across the screen I think
ID3D11Buffer* cbPerObjectBuffer; //Per object buffer sent to vs?
ID3D11RasterizerState* WireFrame; //Rasterizer state that saves how to render things
ID3D11ShaderResourceView* CubesTexture; //Stores the texture the cube
ID3D11SamplerState* CubesTexSamplerState; //Stores the sampler, I think this is how the ps reads the file???
ID3D11BlendState* Transparency;
ID3D11RasterizerState* CCWcullMode;
ID3D11RasterizerState* CWcullMode;
ID3D11RasterizerState* NoCullMode;
ID2D1RenderTarget* d2drendertarget;
IDWriteFactory* dwritefactory;
IDWriteTextFormat* dwritetextformat;
ID2D1SolidColorBrush* blackbrush;
ID3D11Buffer* cbPerFrameBuffer;

ID3D11Buffer* meshVertBuff;
ID3D11Buffer* meshIndexBuff;
XMMATRIX meshWorld;
int meshSubsets = 0;
std::vector<int> meshSubsetIndexStart;
std::vector<int> meshSubsetTexture;

std::vector<ID3D11ShaderResourceView*> meshSRV;
std::vector<std::wstring> textureNameArray;


XMMATRIX WVP;
XMMATRIX camView;
XMMATRIX camProjection;

XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;

XMMATRIX cube1World;

struct SurfaceMaterial
{
    std::wstring matName;
    XMFLOAT4 difColor;
    int texArrayIndex;
    int normMapTexArrayIndex;
    bool hasNormMap;
    bool hasTexture;
    bool transparent;
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

    XMFLOAT3 pos;
    XMFLOAT2 texcoord;
    XMFLOAT3 normal;
    XMFLOAT3 tangent;
    XMFLOAT3 biTangent;

    // Will not be sent to shader
    int StartWeight;
    int WeightCount;
};

struct Joint
{
    std::wstring name;
    int parentID;

    XMFLOAT3 pos;
    XMFLOAT4 orientation;
};

struct Weight
{
    int jointID;
    float bias;
    XMFLOAT3 pos;
    XMFLOAT3 normal;
};

struct ModelSubset
{
    int texArrayIndex;
    int numTriangles;

    std::vector<Vertex> vertices;
    std::vector<DWORD> indices;
    std::vector<Weight> weights;

    std::vector<XMFLOAT3> positions;

    ID3D11Buffer* vertBuff; 
    ID3D11Buffer* indexBuff;
};

struct BoundingBox
{
    XMFLOAT3 min;
    XMFLOAT3 max;
};

struct FrameData
{
    int frameID;
    std::vector<float> frameData;
};
struct AnimJointInfo
{
    std::wstring name;
    int parentID;

    int flags;
    int startIndex;
};

struct ModelAnimation
{
    int numFrames;
    int numJoints;
    int frameRate;
    int numAnimatedComponents;

    float frameTime;
    float totalAnimTime;
    float currAnimTime;

    std::vector<AnimJointInfo> jointInfo;
    std::vector<BoundingBox> frameBounds;
    std::vector<Joint>    baseFrameJoints;
    std::vector<FrameData>    frameData;
    std::vector<std::vector<Joint>> frameSkeleton;
};

struct Model3D
{
    int numSubsets;
    int numJoints;

    std::vector<Joint> joints;
    std::vector<ModelSubset> subsets;

    std::vector<ModelAnimation> animations;
};





//------------------------
//Time constants
double countsPerSecond = 0.0;
__int64 CounterStart = 0;

int frameCount = 0;
int fps = 0;

__int64 frameTimeOld = 0;
double frameTime;
//-------------------------
//The input stuff
IDirectInputDevice8* DIKeyboard;
IDirectInputDevice8* DIMouse;

DIMOUSESTATE mouseLastState;
LPDIRECTINPUT8 DirectInput;

//---------------------------
//Camera Stuff

XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;

XMVECTOR DefaultForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR DefaultRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);
XMVECTOR camForward = XMVectorSet(0.0f,0.0f,1.0f, 0.0f);
XMVECTOR camRight = XMVectorSet(1.0f,0.0f,0.0f, 0.0f);

XMMATRIX camRotationMatrix;
XMMATRIX groundWorld;

float moveLeftRight = 0.0f;
float moveBackForward = 0.0f;

float camYaw = 0.0f;
float camPitch = 0.0f;
//---------------------------
//Picking
std::vector<XMFLOAT3> groundVertPosArray;
std::vector<DWORD> groundVertIndexArray;

ID3D11Buffer* bottleVertBuff;
ID3D11Buffer* bottleIndexBuff;
std::vector<XMFLOAT3> bottleVertPosArray;
std::vector<DWORD> bottleVertIndexArray;
int bottleSubsets = 0;
std::vector<int> bottleSubsetIndexStart;
std::vector<int> bottleSubsetTexture;

//Global Variables

bool isShoot = false;

int ClientWidth = 0;
int ClientHeight = 0;

int score = 0;
float pickedDist = 0.0f;

//---------------------------
//Bounding Volumes
XMMATRIX bottleWorld[1000];
int* bottleHit = new int[1000];
int numBottles = 1000;

float bottleBoundingSphere = 0.0f;
std::vector<XMFLOAT3> bottleBoundingBoxVertPosArray;
std::vector<DWORD> bottleBoundingBoxVertIndexArray;
XMVECTOR bottleCenterOffset;

int pickWhat = 0;

double pickOpSpeed = 0.0f;

bool isPDown = false;
//----------------------------
//Collision Detection
XMVECTOR bottleBoundingBoxMinVertex[820];
XMVECTOR bottleBoundingBoxMaxVertex[820];

XMVECTOR thrownBottleBoundingBoxMinVertex;
XMVECTOR thrownBottleBoundingBoxMaxVertex;
XMMATRIX thrownBottleWorld;
XMVECTOR thrownBottleDir;
bool bottleFlying;

int cdMethod = 0;

double cdOpSpeed = 0.0f;

bool isCDown = false;

//----------------------------
//MD5 Modeling

XMMATRIX playerCharWorld;
Model3D NewMD5Model;

//----------------------------
//Third Person Camera
XMVECTOR currCharDirection = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR oldCharDirection = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
XMVECTOR charPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

float charCamDist = 15.0f;

//----------------------------

// function prototypes
void InitD3D(HWND hwnd);     // sets up and initializes Direct3D
void CleanD3D(HWND hwnd);         // closes Direct3D and releases memory
void RenderFrame();
void InitPipeline();
void InitGraphics();
void UpdateScene(double time);
void drawstuff(std::wstring text, int inInt);
bool InitDirectInput(HINSTANCE hInstance, HWND hwnd);
void DetectInput(double time, HWND hwnd);
void UpdateCamera();
//Define LoadObjModel function after we create surfaceMaterial structure
bool LoadObjModel(std::wstring filename,            //.obj filename
    ID3D11Buffer** vertBuff,                    //mesh vertex buffer
    ID3D11Buffer** indexBuff,                    //mesh index buffer
    std::vector<int>& subsetIndexStart,            //start index of each subset
    std::vector<int>& subsetMaterialArray,        //index value of material for each subset
    std::vector<SurfaceMaterial>& material,        //vector of material structures
    int& subsetCount,                            //Number of subsets in mesh
    bool isRHCoordSys,                            //true if model was created in right hand coord system
    bool computeNormals,                        //true to compute the normals, false to use the files normals
    std::vector<XMFLOAT3>& vertPosArray,        //Used for CPU to do calculations on the Geometry
    std::vector<DWORD>& vertIndexArray);        //Also used for CPU caculations on geometry

void StartTimer();
double GetTime();
double GetFrameTime();
//Picking
void pickRayVector(float mouseX, float mouseY, XMVECTOR& pickRayInWorldSpacePos, XMVECTOR& pickRayInWorldSpaceDir);
float pick(XMVECTOR pickRayInWorldSpacePos,
    XMVECTOR pickRayInWorldSpaceDir,
    std::vector<XMFLOAT3>& vertPosArray,
    std::vector<DWORD>& indexPosArray,
    XMMATRIX& worldSpace);
bool PointInTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point );
void CreateBoundingVolumes(std::vector<XMFLOAT3> &vertPosArray,    // The array containing our models vertex positions
    std::vector<XMFLOAT3>& boundingBoxVerts,                            // Array we want to store the bounding box's vertex positions
    std::vector<DWORD>& boundingBoxIndex,                            // This is our bounding box's index array
    float &boundingSphere,                                            // The float containing the radius of our bounding sphere
    XMVECTOR &objectCenterOffset);                                    // A vector containing the distance between the models actual center and (0, 0, 0) in model space
bool BoundingSphereCollision(float firstObjBoundingSphere, 
    XMVECTOR firstObjCenterOffset,
    XMMATRIX& firstObjWorldSpace,
    float secondObjBoundingSphere,
    XMVECTOR secondObjCenterOffset, 
    XMMATRIX& secondObjWorldSpace);

bool BoundingBoxCollision(XMVECTOR& firstObjBoundingBoxMinVertex, 
    XMVECTOR& firstObjBoundingBoxMaxVertex,
    XMVECTOR& secondObjBoundingBoxMinVertex, 
    XMVECTOR& secondObjBoundingBoxMaxVertex);
void CalculateAABB(std::vector<XMFLOAT3> boundingBoxVerts, 
    XMMATRIX& worldSpace,
    XMVECTOR& boundingBoxMin,
    XMVECTOR& boundingBoxMax);

bool LoadMD5Model(std::wstring filename,
    Model3D& MD5Model,
    std::vector<ID3D11ShaderResourceView*>& shaderResourceViewArray,
    std::vector<std::wstring> texFileNameArray);
bool LoadMD5Anim(std::wstring filename,    Model3D& MD5Model);
void UpdateMD5Model(Model3D& MD5Model, float deltaTime, int animation);
void MoveChar(double time, XMVECTOR& destinationDirection, XMMATRIX& worldMatrix);



struct cbPerObject
{
    XMMATRIX  WVP;
    XMMATRIX  World;
    XMFLOAT4 difColor;
    BOOL hasTexture;
    //Because of HLSL structure packing, we will use windows BOOL
    //instead of bool because HLSL packs things into 4 bytes, and
    //bool is only one byte, where BOOL is 4 bytes
    BOOL hasNormMap;
};

cbPerObject cbPerObj;

//Lights stuff
struct Light
{
    Light()
    {
        ZeroMemory(this, sizeof(Light));
    }
    XMFLOAT3 dir;
    float cone;
    XMFLOAT3 pos;
    float range;
    XMFLOAT3 att;
    float pad;
    XMFLOAT4 ambient;
    XMFLOAT4 diffuse;
};

Light light;

struct cbPerFrame
{
    Light  light;
};

cbPerFrame constbuffPerFrame;



std::vector<SurfaceMaterial> material;
std::vector<SurfaceMaterial> bottlematerial;


// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[]  = L"Sample Window Class";
    
    WNDCLASS wc = { };

    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;
    

    RegisterClass(&wc);

    // Create the window.
    

    HWND hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Thomas-Physics-Sim",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        300, 300, SCREEN_WIDTH, SCREEN_HEIGHT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
        );

    if (hwnd == NULL)
    {
        printf("hwnd == NULL");
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    //Init directx3d
    InitD3D(hwnd);
    //Init Direct Input
    if(!InitDirectInput(hInstance, hwnd)) {
        MessageBox(0, L"Direct Input Initialization - Failed",L"Error", MB_OK);
        CleanD3D(hwnd);
        return 0;
    }

    // Run the message loop.

    printf("Initialization Complete. Starting Message Loop\n");
    MSG msg = { };
    while (true) {
        //Message Loop
        //Note: Was a while loop, where >0, but tutorial used if. I assume this means it can work a max of 1 input per frame
        if(PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage (&msg);
            DispatchMessage (&msg);

            //Exit Condition
            if(msg.message == WM_QUIT) {
                break;
            }
        }

        //Run update code here
        //...
        frameCount++;
        if(GetTime() > 1.0f) {
            fps = frameCount;
            frameCount = 0;
            StartTimer();
        }
        frameTime = GetFrameTime();
        DetectInput(frameTime, hwnd);
        UpdateScene(frameTime);
        RenderFrame();
    }

    //Clean D3d
    CleanD3D(hwnd);

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
        ClientWidth  = LOWORD(lParam);
        ClientHeight = HIWORD(lParam);
        return 0;

    case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        } break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void drawstuff(std::wstring text, int inInt) {
        static const WCHAR sc_helloWorld[] = L"Hello, World!";

    // Display which picking method we are doing
    std::wstring cdMethodString;
    if(cdMethod == 0)
        cdMethodString = L"Bounding Sphere";
    if(cdMethod == 1)
        cdMethodString = L"Bounding Box";

        std::wostringstream printString;
        printString << text << inInt << "\n" 
        << "LightPos: " << light.pos.x << "," << light.pos.y << ", " << light.pos.z << "\n"
        << "Score: " << score << "\n"
        << L"Collission Detection Method (C): " << cdMethodString << L"\n"
        << L"CD Op. Speed: " << cdOpSpeed;
        std::wstring printText = printString.str();

        // Retrieve the size of the render target.
        D2D1_SIZE_F renderTargetSize = d2drendertarget->GetSize();

        d2drendertarget->BeginDraw();

        d2drendertarget->SetTransform(D2D1::Matrix3x2F::Identity());

        // d2drendertarget->Clear(D2D1::ColorF(D2D1::ColorF::White));

        d2drendertarget->DrawText(
            printText.c_str(),
            wcslen(printText.c_str()),
            dwritetextformat,
            D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
            blackbrush
            );

        d2drendertarget->EndDraw();
}


void UpdateScene(double time){
    //Reset cube1World
    groundWorld = XMMatrixIdentity();

    //Define cube1's world space matrix
    Scale = XMMatrixScaling( 500.0f, 10.0f, 500.0f );
    Translation = XMMatrixTranslation( 0.0f, 10.0f, 0.0f );

    //Set cube1's world space using the transformations
    groundWorld = Scale * Translation;

    //Update the light to follow the camera and point where the camera points, essentially making a flashlight
    light.pos.x = XMVectorGetX(camPosition);
    light.pos.y = XMVectorGetY(camPosition);
    light.pos.z = XMVectorGetZ(camPosition);

    // light.dir.x = XMVectorGetX(camTarget) - light.pos.x;
    // light.dir.y = XMVectorGetY(camTarget) - light.pos.y;
    // light.dir.z = XMVectorGetZ(camTarget) - light.pos.z;

    meshWorld = XMMatrixIdentity();

    //Define cube1's world space matrix
    Rotation = XMMatrixRotationY(3.14f);
    Scale = XMMatrixScaling( 1.0f, 1.0f, 1.0f );
    Translation = XMMatrixTranslation( 0.0f, 0.0f, 0.0f );

    meshWorld = Rotation * Scale * Translation;

        //Update our thrown bottles position
    if(bottleFlying)
    {
        XMVECTOR tempBottlePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        // tempBottlePos = XMVector3TransformCoord(tempBottlePos, thrownBottleWorld) + (thrownBottleDir * time * 10.0f);
        tempBottlePos = XMVector3TransformCoord(tempBottlePos, thrownBottleWorld) + (XMVectorScale(thrownBottleDir, time * 10.0f));
        Rotation = XMMatrixRotationY(3.14f);

        thrownBottleWorld = XMMatrixIdentity();
        Translation = XMMatrixTranslation( XMVectorGetX(tempBottlePos), XMVectorGetY(tempBottlePos), XMVectorGetZ(tempBottlePos) );

        thrownBottleWorld = Translation;

        //Update the objects AABB every time the object is transformed
        CalculateAABB(bottleBoundingBoxVertPosArray, thrownBottleWorld, thrownBottleBoundingBoxMinVertex, thrownBottleBoundingBoxMaxVertex);

        for(int i = 0; i < numBottles; i++)
        {
            if(bottleHit[i] == 0) // No need to check bottles already hit
            {    
                double cdOpStartTime = GetTime();
                if(cdMethod == 0)
                {
                    if(BoundingSphereCollision(bottleBoundingSphere, bottleCenterOffset, thrownBottleWorld, bottleBoundingSphere, bottleCenterOffset, bottleWorld[i]))
                    {
                        bottleHit[i] = 1;
                        score++;
                        bottleFlying = false;
                    }
                }

                if(cdMethod == 1)
                {
                    if(BoundingBoxCollision(thrownBottleBoundingBoxMinVertex, thrownBottleBoundingBoxMaxVertex, bottleBoundingBoxMinVertex[i], bottleBoundingBoxMaxVertex[i]))
                    {
                        bottleHit[i] = 1;
                        score++;
                        bottleFlying = false;
                    }
                }    
                // This is the time in seconds it took to complete the CD process
                cdOpSpeed = GetTime() - cdOpStartTime;    
            }
        }
    }
}

// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd)
{
    printf("InitD3D\n");



    //Describe our SwapChain Buffer
    DXGI_MODE_DESC bufferDesc;

    ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

    bufferDesc.Width = SCREEN_WIDTH;
    bufferDesc.Height = SCREEN_HEIGHT;
    bufferDesc.RefreshRate.Numerator = 60;
    bufferDesc.RefreshRate.Denominator = 1;
    bufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; //Must be bgra for d2d compatibility
    bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    // create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                    // one back buffer
    scd.BufferDesc = bufferDesc;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    scd.OutputWindow = hWnd;                                // the window to be used
    scd.SampleDesc.Count = 1;                               // how many multisamples
    scd.SampleDesc.Quality = 0;
    scd.Windowed = false;                                    // windowed/full-screen mode
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching

    // create a device, device context and swap chain using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  NULL,
                                  D3D11_CREATE_DEVICE_BGRA_SUPPORT, //Gives d2d support
                                  NULL,
                                  0,
                                  D3D11_SDK_VERSION,
                                  &scd,
                                  &swapchain,
                                  &dev,
                                  NULL,
                                  &devcon);

// -----------------
    // get the address of the back buffer
    ID3D11Texture2D *pBackBuffer;
    // swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer); //Mingw version
    swapchain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBuffer); //Windows version

    // use the back buffer address to create the render target
    dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);

    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dfactory);

    //Create the DXGI Surface Render Target.
    FLOAT dpiX;
    FLOAT dpiY;
    d2dfactory->GetDesktopDpi(&dpiX, &dpiY);


    D2D1_RENDER_TARGET_PROPERTIES props =
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_HARDWARE,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
            dpiX,
            dpiY
            );
    // printf("Here\n");
    IDXGISurface1* surface;
    HRESULT hb = swapchain->GetBuffer(0, IID_IDXGISurface1, (void**)&surface);
    if(!SUCCEEDED(hb)) {
        printf("uh oh\n");
        printf("%p\n", hb);
    }
    // printf("Here\n");

    //Create the d2d render target
    HRESULT hr = d2dfactory->CreateDxgiSurfaceRenderTarget(surface, &props, &d2drendertarget);
    if(!SUCCEEDED(hr)) {
        printf("uh oh in the dxgisurfacerendertarget\n");
        printf("%p\n", hr);
    }

    surface->Release();
    pBackBuffer->Release();

    // printf("Here\n");
    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, IID_IDWriteFactory, reinterpret_cast<IUnknown **>(&dwritefactory));
// printf("Here\n");
    dwritefactory->CreateTextFormat(L"Verdana", //Font name
            NULL,
            DWRITE_FONT_WEIGHT_NORMAL,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            24, //Font Size
            L"", //locale
            &dwritetextformat
            );
    // printf("Here\n");
    d2drendertarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black, 1.0f), &blackbrush);
// printf("Here\n");
    dwritetextformat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_JUSTIFIED);
        
    dwritetextformat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

// -----------------

    

    InitPipeline();
    InitGraphics();

    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = SCREEN_WIDTH;
    viewport.Height = SCREEN_HEIGHT;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    devcon->RSSetViewports(1, &viewport);

    // set the render target as the back buffer
    devcon->OMSetRenderTargets(1, &backbuffer, depthStencilView);
}

// this is the function that cleans up Direct3D and COM
void CleanD3D(HWND hwnd) {
    printf("Cleanup D3D\n");
    swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode
    PostMessage(hwnd, WM_DESTROY, 0, 0);

    // close and release all existing COM objects
    pLayout->Release();
    pVS->Release();
    pPS->Release();
    // pVBuffer->Release();
    swapchain->Release();
    backbuffer->Release();
    dev->Release();
    devcon->Release();
    squareIndexBuffer->Release();
    squareVertBuffer->Release();
    depthStencilView->Release();
    depthStencilBuffer->Release();
    cbPerObjectBuffer->Release();
    WireFrame->Release();
    Transparency->Release();
    CCWcullMode->Release();
    CWcullMode->Release();
    NoCullMode->Release();
    dxgifactory->Release();
    d2dfactory->Release();
    d2drendertarget->Release();
    dwritefactory->Release();
    dwritetextformat->Release();
    blackbrush->Release();
    cbPerFrameBuffer->Release();

    DIKeyboard->Unacquire();
    DIMouse->Unacquire();
    DirectInput->Release();

    meshVertBuff->Release();
    meshIndexBuff->Release();

    bottleVertBuff->Release();
    bottleIndexBuff->Release();

    for(int i = 0; i < NewMD5Model.numSubsets; i++)
    {
        NewMD5Model.subsets[i].indexBuff->Release();
        NewMD5Model.subsets[i].vertBuff->Release();
    }

}

// this is the function used to render a single frame
void RenderFrame(void) {
    // clear the back buffer to a deep blue
    // devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f)); //This is the tutorial version, requires d3dx
    // float color[4] = {0.0f, 0.2f, 0.4f, 1.0f};

    //Set the light
    constbuffPerFrame.light = light;
    devcon->UpdateSubresource( cbPerFrameBuffer, 0, NULL, &constbuffPerFrame, 0, 0 );
    devcon->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer); 

    //Reset the V and P Shaders
    devcon->VSSetShader(pVS, 0, 0);
    devcon->PSSetShader(pPS, 0, 0);

    float color[4] = {0.4f,0.4f,0.4f,1.0f};
    devcon->ClearRenderTargetView(backbuffer, color);
    devcon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);


    devcon->OMSetBlendState(0, 0, 0xffffffff);

    UINT stride = sizeof( Vertex );
    UINT offset = 0;

    //Draw our model's NON-transparent subsets

    for(int i = 0; i < meshSubsets; ++i)
    {
        //Set the grounds index buffer
        devcon->IASetIndexBuffer( meshIndexBuff, DXGI_FORMAT_R32_UINT, 0);
        //Set the grounds vertex buffer
        devcon->IASetVertexBuffers( 0, 1, &meshVertBuff, &stride, &offset );

        //Set the WVP matrix and send it to the constant buffer in effect file
        WVP = meshWorld * camView * camProjection;
        cbPerObj.WVP = XMMatrixTranspose(WVP);    
        cbPerObj.World = XMMatrixTranspose(meshWorld);    
        cbPerObj.difColor = material[meshSubsetTexture[i]].difColor;
        cbPerObj.hasTexture = material[meshSubsetTexture[i]].hasTexture;
        cbPerObj.hasNormMap = material[meshSubsetTexture[i]].hasNormMap;
        // printf("%d\n",cbPerObj.hasTexture);
        devcon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
        devcon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
        devcon->PSSetConstantBuffers( 1, 1, &cbPerObjectBuffer );
        if(material[meshSubsetTexture[i]].hasTexture)
            devcon->PSSetShaderResources( 0, 1, &meshSRV[material[meshSubsetTexture[i]].texArrayIndex] );
        if(material[meshSubsetTexture[i]].hasNormMap)
            devcon->PSSetShaderResources( 1, 1, &meshSRV[material[meshSubsetTexture[i]].normMapTexArrayIndex] );
        devcon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

        devcon->RSSetState(NoCullMode);
        int indexStart = meshSubsetIndexStart[i];
        int indexDrawAmount =  meshSubsetIndexStart[i+1] - meshSubsetIndexStart[i];
        if(!material[meshSubsetTexture[i]].transparent)
            devcon->DrawIndexed( indexDrawAmount, indexStart, 0 );
    }

    ///***Draw MD5 Model***///
    for(int i = 0; i < NewMD5Model.numSubsets; i ++)
    {
        //Set the grounds index buffer
        devcon->IASetIndexBuffer( NewMD5Model.subsets[i].indexBuff, DXGI_FORMAT_R32_UINT, 0);
        //Set the grounds vertex buffer
        devcon->IASetVertexBuffers( 0, 1, &NewMD5Model.subsets[i].vertBuff, &stride, &offset );

        //Set the WVP matrix and send it to the constant buffer in effect file
        WVP = playerCharWorld * camView * camProjection;
        cbPerObj.WVP = XMMatrixTranspose(WVP);    
        cbPerObj.World = XMMatrixTranspose(playerCharWorld);    
        cbPerObj.hasTexture = true;        // We'll assume all md5 subsets have textures
        cbPerObj.hasNormMap = false;    // We'll also assume md5 models have no normal map (easy to change later though)
        devcon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
        devcon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
        devcon->PSSetConstantBuffers( 1, 1, &cbPerObjectBuffer );
        devcon->PSSetShaderResources( 0, 1, &meshSRV[NewMD5Model.subsets[i].texArrayIndex] );
        devcon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

        devcon->RSSetState(NoCullMode);
        devcon->DrawIndexed( NewMD5Model.subsets[i].indices.size(), 0, 0 );

    }


    //Draw our model's TRANSPARENT subsets now

    //Set our blend state
    devcon->OMSetBlendState(Transparency, NULL, 0xffffffff);

    for(int i = 0; i < meshSubsets; ++i)
    {
        //Set the grounds index buffer
        devcon->IASetIndexBuffer( meshIndexBuff, DXGI_FORMAT_R32_UINT, 0);
        //Set the grounds vertex buffer
        devcon->IASetVertexBuffers( 0, 1, &meshVertBuff, &stride, &offset );

        //Set the WVP matrix and send it to the constant buffer in effect file
        WVP = meshWorld * camView * camProjection;
        cbPerObj.WVP = XMMatrixTranspose(WVP);    
        cbPerObj.World = XMMatrixTranspose(meshWorld);    
        cbPerObj.difColor = material[meshSubsetTexture[i]].difColor;
        cbPerObj.hasTexture = material[meshSubsetTexture[i]].hasTexture;
        cbPerObj.hasNormMap = material[meshSubsetTexture[i]].hasNormMap;
        devcon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
        devcon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
        devcon->PSSetConstantBuffers( 1, 1, &cbPerObjectBuffer );
        if(material[meshSubsetTexture[i]].hasTexture)
            devcon->PSSetShaderResources( 0, 1, &meshSRV[material[meshSubsetTexture[i]].texArrayIndex] );
        devcon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

        devcon->RSSetState(NoCullMode);
        int indexStart = meshSubsetIndexStart[i];
        int indexDrawAmount =  meshSubsetIndexStart[i+1] - meshSubsetIndexStart[i];
        if(material[meshSubsetTexture[i]].transparent)
            devcon->DrawIndexed( indexDrawAmount, indexStart, 0 );
    }


    // switch the back buffer and the front buffer
    drawstuff(L"FPS: ", fps);
    swapchain->Present(0, 0);
}

void InitPipeline()
{
    printf("InitPipeline\n");
    // load and compile the two shaders
    ID3DBlob *VS, *PS;
    // D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
    // D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);
    D3DCompileFromFile(L"myshader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "VShader", "vs_4_0", 0, 0,&VS, 0);
    D3DCompileFromFile(L"myshader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "PShader", "ps_4_0", 0, 0,&PS, 0);
    printf("L");

    // encapsulate both shaders into shader objects
    dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
    dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

    printf("O");
    // set the shader objects
    devcon->VSSetShader(pVS, 0, 0);
    devcon->PSSetShader(pPS, 0, 0);

    printf("B");
    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    dev->CreateInputLayout(ied, ARRAYSIZE(ied), VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
    devcon->IASetInputLayout(pLayout);
    printf("A\n");
    // select which primtive type we are using
    devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Depth Stencil Shit
    //Describe our Depth/Stencil Buffer
    D3D11_TEXTURE2D_DESC depthStencilDesc;
    depthStencilDesc.Width     = SCREEN_WIDTH;
    depthStencilDesc.Height    = SCREEN_HEIGHT;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count   = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0; 
    depthStencilDesc.MiscFlags      = 0;

    dev->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
    dev->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);

    //Create blending description for transparency

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory( &blendDesc, sizeof(blendDesc) );

    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory( &rtbd, sizeof(rtbd) );

    rtbd.BlendEnable             = true;
    rtbd.SrcBlend                 = D3D11_BLEND_INV_SRC_ALPHA;
    rtbd.DestBlend                 = D3D11_BLEND_SRC_ALPHA;
    rtbd.BlendOp                 = D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha             = D3D11_BLEND_INV_SRC_ALPHA;
    rtbd.DestBlendAlpha             = D3D11_BLEND_SRC_ALPHA;
    rtbd.BlendOpAlpha             = D3D11_BLEND_OP_ADD;
    rtbd.RenderTargetWriteMask     = D3D10_COLOR_WRITE_ENABLE_ALL;

    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.RenderTarget[0] = rtbd;

    dev->CreateBlendState(&blendDesc, &Transparency);

    D3D11_RASTERIZER_DESC cmdesc;
    ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

    cmdesc.FillMode = D3D11_FILL_SOLID;
    cmdesc.CullMode = D3D11_CULL_BACK;

    cmdesc.FrontCounterClockwise = true;
    dev->CreateRasterizerState(&cmdesc, &CCWcullMode);

    cmdesc.FrontCounterClockwise = false;
    dev->CreateRasterizerState(&cmdesc, &CWcullMode);

    cmdesc.CullMode = D3D11_CULL_NONE;
    dev->CreateRasterizerState(&cmdesc, &NoCullMode);
}

void InitGraphics() {
    printf("InitGraphics\n");

    //Configure the point light
    // light.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
    // light.range = 100.0f;
    // light.att = XMFLOAT3(0.0f, 0.2f, 0.0f);
    // light.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    // light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    //Configure the Directional Light
    light.dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
    light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    //Configure the Spotlight
    // light.pos = XMFLOAT3(0.0f, 1.0f, 0.0f);
    // light.dir = XMFLOAT3(0.0f, 0.0f, 1.0f);
    // light.range = 1000.0f;
    // light.cone = 20.0f;
    // light.att = XMFLOAT3(0.4f, 0.02f, 0.0f);
    // light.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    // light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    printf("C");

    //Square Index Buffer
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    dev->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

    devcon->IASetIndexBuffer( squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    printf("R");

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof( Vertex ) * 4;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    printf("A");

    D3D11_SUBRESOURCE_DATA vertexBufferData; 

    ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
    dev->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

    printf("L");

    //Set the vertex buffer
    UINT stride = sizeof( Vertex );
    UINT offset = 0;
    // devcon->IASetVertexBuffers( 0, 1, &squareVertBuffer, &stride, &offset );


    //Create constant buffer
    D3D11_BUFFER_DESC cbbd;    
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbPerObject);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;
    cbbd.MiscFlags = 0;

    dev->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

    printf("N");

    //Create perframe buffer
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbPerFrame);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;
    cbbd.MiscFlags = 0;

    dev->CreateBuffer(&cbbd, NULL, &cbPerFrameBuffer);

    printf("A");

    #ifdef WIREFRAME
    //Rasterizer Stage
    D3D11_RASTERIZER_DESC wfdesc;
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    wfdesc.CullMode = D3D11_CULL_NONE;
    dev->CreateRasterizerState(&wfdesc, &WireFrame);
    #endif

    devcon->RSSetState(WireFrame);

    // Scale = XMMatrixScaling( 0.25f, 0.25f, 0.25f );
    // Translation = XMMatrixTranslation( 0.0f, 0.0f, 0.0f);
    // playerCharWorld = Scale * Translation;

    //Camera information
    camPosition = XMVectorSet( 0.0f, 10.0f, 8.0f, 0.0f );
    camTarget = XMVectorSet( 0.0f, 3.0f, 0.0f, 0.0f );
    camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

    camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );

    camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, (float)SCREEN_WIDTH/SCREEN_HEIGHT, 1.0f, 1000.0f);

    //Load the texture
    // D3DX11CreateShaderResourceViewFromFile( dev, L"braynzar.jpg", NULL, NULL, &CubesTexture, NULL );
    // LoadFromWICFile(L"resc/monkey.jpg",NULL, );
    // CreateWICTextureFromFile(dev, devcon, L"resc/metalpanel.jpg", NULL, &CubesTexture);
    // CreateShaderResourceView();

    printf("L");

    if(!LoadObjModel(L"resc/ground.obj", &meshVertBuff, &meshIndexBuff, meshSubsetIndexStart, meshSubsetTexture, material, meshSubsets, true, true, groundVertPosArray, groundVertIndexArray))
        return;    
    if(!LoadObjModel(L"resc/bottle.obj", &bottleVertBuff, &bottleIndexBuff, bottleSubsetIndexStart, bottleSubsetTexture, bottlematerial, bottleSubsets, true, true, bottleVertPosArray, bottleVertIndexArray))
        return;
    if(!LoadMD5Model(L"resc/boy.md5mesh", NewMD5Model, meshSRV, textureNameArray))
        return;
    printf("A");
    if(!LoadMD5Anim(L"resc/boy.md5anim", NewMD5Model))
        printf("Failed to load animation\n");

    Scale = XMMatrixScaling( 0.04f, 0.04f, 0.04f );            // The model is a bit too large for our scene, so make it smaller
    Translation = XMMatrixTranslation( 0.0f, 3.0f, 0.0f );
    playerCharWorld = Scale * Translation;

    printf("E\n");
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    dev->CreateSamplerState( &sampDesc, &CubesTexSamplerState );



    //Do all the setup for the bottles

    CreateBoundingVolumes(bottleVertPosArray, bottleBoundingBoxVertPosArray, bottleBoundingBoxVertIndexArray, bottleBoundingSphere, bottleCenterOffset);

    float bottleXPos = -25.0f;
    float bottleZPos = 50.0f;
    int rows = 40;
    int counter = 0;

    for(int j = 0; j < rows; j++)
    {
        for(int k = 0; k < rows - j; k++)
        {
            bottleHit[counter] = 0;

            // set the loaded bottles world space
            bottleWorld[counter] = XMMatrixIdentity();

            Translation = XMMatrixTranslation( bottleXPos + k + j*0.5f, j*2.25 + 1, bottleZPos);
            Rotation = XMMatrixRotationZ(3.14f);

            bottleWorld[counter] = Rotation * Translation;

            //Update the objects AABB every time the object is transformed
            CalculateAABB(bottleBoundingBoxVertPosArray, bottleWorld[counter], bottleBoundingBoxMinVertex[counter], bottleBoundingBoxMaxVertex[counter]);
            
            counter++;
        }
    }


    
}

bool InitDirectInput(HINSTANCE hInstance, HWND hwnd) {
    DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);

    DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard, NULL);

    DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);

    DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    DIMouse->SetDataFormat(&c_dfDIMouse);

    //Setup mouse
    //This version has mouse invisible
    DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
    //This version has mouse visible. We want it visible for picking
    // DIMouse->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

    return true;
}

void DetectInput(double time, HWND hwnd)
{
    DIMOUSESTATE mouseCurrState;

    BYTE keyboardState[256];

    DIKeyboard->Acquire();
    DIMouse->Acquire();

    DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

    DIKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);

    if(keyboardState[DIK_ESCAPE] & 0x80)
        PostMessage(hwnd, WM_DESTROY, 0, 0);

    float speed = 10.0f * time;    
    bool moveChar = false;
    XMVECTOR desiredCharDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    if(keyboardState[DIK_A] & 0x80)
    {
        desiredCharDir += (camRight);
        moveChar = true;
    }
    if(keyboardState[DIK_D] & 0x80)
    {
        desiredCharDir += -(camRight);
        moveChar = true;
    }
    if(keyboardState[DIK_W] & 0x80)
    {
        desiredCharDir += (camForward);
        moveChar = true;
    }
    if(keyboardState[DIK_S] & 0x80)
    {
        desiredCharDir += -(camForward);
        moveChar = true;
    }
    if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
    {
        camYaw += mouseLastState.lX * 0.002f;

        camPitch += mouseCurrState.lY * 0.002f;
        // Check that the camera doesn't go over the top or under the player
        if(camPitch > 0.85f)
            camPitch = 0.85f;
        if(camPitch < -0.85f)
            camPitch = -0.85f;

        mouseLastState = mouseCurrState;
    }

    if(moveChar == true)
        MoveChar(time, desiredCharDir, playerCharWorld);

    UpdateCamera();

    return;
}

void StartTimer() {
    LARGE_INTEGER frequencyCount;
    QueryPerformanceFrequency(&frequencyCount);

    countsPerSecond = double(frequencyCount.QuadPart);

    QueryPerformanceCounter(&frequencyCount);
    CounterStart = frequencyCount.QuadPart;
}
double GetTime() {
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);
    return double(currentTime.QuadPart-CounterStart)/countsPerSecond;
}
double GetFrameTime() {
    LARGE_INTEGER currentTime;
    __int64 tickCount;
    QueryPerformanceCounter(&currentTime);

    tickCount = currentTime.QuadPart-frameTimeOld;
    frameTimeOld = currentTime.QuadPart;

    if(tickCount < 0.0f) tickCount = 0.0f;

    return float(tickCount)/countsPerSecond;
}

void UpdateCamera()
{
    // Rotate target around camera
    /*camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
    camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
    camTarget = XMVector3Normalize(camTarget);*/

    /*XMMATRIX RotateYTempMatrix;
    RotateYTempMatrix = XMMatrixRotationY(camYaw);

    // Walk
    camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
    camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);
    camUp = XMVector3Cross(camForward, camRight);*/

    /*// Free Cam
    camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
    camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
    camUp = XMVector3Cross(camForward, camRight);*/

    /*camPosition += moveLeftRight*camRight;
    camPosition += moveBackForward*camForward;

    moveLeftRight = 0.0f;
    moveBackForward = 0.0f;

    camTarget = camPosition + camTarget;*/

    // Third Person Camera
    // Set the cameras target to be looking at the character.
    camTarget = charPosition;

    // This line is because this lessons model was set to stand on the point (0,0,0) (my bad), and we
    // don't want to just be looking at the models feet, so we move the camera's target vector up 5 units
    camTarget = XMVectorSetY(camTarget, XMVectorGetY(camTarget)+5.0f);    

    // Unlike before, when we rotated the cameras target vector around the cameras position,
    // we are now rotating the cameras position around it's target (which is the character)
    // Rotate camera around target
    camRotationMatrix = XMMatrixRotationRollPitchYaw(-camPitch, camYaw, 0);
    camPosition = XMVector3TransformNormal(DefaultForward, camRotationMatrix );
    camPosition = XMVector3Normalize(camPosition);

    // Set our cameras position to rotate around the character. We need to add 5 to the characters
    // position's y axis because i'm stupid and modeled the character in the 3d modeling program
    // to be "standing" on (0,0,0), instead of centered around it ;) Well target her head here though
    camPosition = (camPosition * charCamDist) + camTarget;

    // We need to set our cameras forward and right vectors to lay
    // in the worlds xz plane, since they are the vectors we will
    // be using to determine the direction our character is running
    camForward = XMVector3Normalize(camTarget - camPosition);    // Get forward vector based on target
    camForward = XMVectorSetY(camForward, 0.0f);    // set forwards y component to 0 so it lays only on
    // the xz plane
    camForward = XMVector3Normalize(camForward);
    // To get our camera's Right vector, we set it's x component to the negative z component from the
    // camera's forward vector, and the z component to the camera forwards x component
    camRight = XMVectorSet(-XMVectorGetZ(camForward), 0.0f, XMVectorGetX(camForward), 0.0f);

    // Our camera does not "roll", so we can safely assume that the cameras right vector is always
    // in the xz plane, so to get the up vector, we just get the normalized vector from the camera
    // position to the cameras target, and cross it with the camera's Right vector
    camUp = XMVector3Cross(XMVector3Normalize(camPosition - camTarget), camRight);

    camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );
}


void pickRayVector(float mouseX, float mouseY, XMVECTOR& pickRayInWorldSpacePos, XMVECTOR& pickRayInWorldSpaceDir)
{
    XMVECTOR pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR pickRayInViewSpacePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    float PRVecX, PRVecY, PRVecZ;

    //Transform 2D pick position on screen space to 3D ray in View space
    //Of note is that x and y could be set to zero and that would be center of screen, ie shooting in first person game, see next comment
    //Also, because you can't directly access an XMMatrix, you have to copy the contents to a float4x4 first
    XMFLOAT4X4 camProjTemp;
    XMStoreFloat4x4(&camProjTemp, camProjection);
    PRVecX =  ((( 2.0f * mouseX) / ClientWidth ) - 1 ) / camProjTemp(0,0);
    PRVecY = -((( 2.0f * mouseY) / ClientHeight) - 1 ) / camProjTemp(1,1);
    PRVecZ =  1.0f;    //View space's Z direction ranges from 0 to 1, so we set 1 since the ray goes "into" the screen

    pickRayInViewSpaceDir = XMVectorSet(PRVecX, PRVecY, PRVecZ, 0.0f);

    //Uncomment this line if you want to use the center of the screen (client area)
    //to be the point that creates the picking ray (eg. first person shooter)
    //pickRayInViewSpaceDir = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    // Transform 3D Ray from View space to 3D ray in World space
    XMMATRIX pickRayToWorldSpaceMatrix;
    XMVECTOR matInvDeter;    //We don't use this, but the xna matrix inverse function requires the first parameter to not be null

    pickRayToWorldSpaceMatrix = XMMatrixInverse(&matInvDeter, camView);    //Inverse of View Space matrix is World space matrix

    pickRayInWorldSpacePos = XMVector3TransformCoord(pickRayInViewSpacePos, pickRayToWorldSpaceMatrix);
    pickRayInWorldSpaceDir = XMVector3TransformNormal(pickRayInViewSpaceDir, pickRayToWorldSpaceMatrix);
}

//This does the picking. The technique is actually very cool. Go look at the braynarzsoft tutorial for an explanation
//The short version is you treat every triangle in the mesh as a plane and see where on that plane the ray intersects. Then see if that intersection point is inside triangle
float pick(XMVECTOR pickRayInWorldSpacePos,
    XMVECTOR pickRayInWorldSpaceDir, 
    std::vector<XMFLOAT3>& vertPosArray,
    std::vector<DWORD>& indexPosArray, 
    XMMATRIX& worldSpace)
{         
    //Loop through each triangle in the object
    for(int i = 0; i < indexPosArray.size()/3; i++)
    {
        //Triangle's vertices V1, V2, V3
        XMVECTOR tri1V1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMVECTOR tri1V2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMVECTOR tri1V3 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

        //Temporary 3d floats for each vertex
        XMFLOAT3 tV1, tV2, tV3;

        //Get triangle 
        tV1 = vertPosArray[indexPosArray[(i*3)+0]];
        tV2 = vertPosArray[indexPosArray[(i*3)+1]];
        tV3 = vertPosArray[indexPosArray[(i*3)+2]];

        tri1V1 = XMVectorSet(tV1.x, tV1.y, tV1.z, 0.0f);
        tri1V2 = XMVectorSet(tV2.x, tV2.y, tV2.z, 0.0f);
        tri1V3 = XMVectorSet(tV3.x, tV3.y, tV3.z, 0.0f);

        //Transform the vertices to world space
        tri1V1 = XMVector3TransformCoord(tri1V1, worldSpace);
        tri1V2 = XMVector3TransformCoord(tri1V2, worldSpace);
        tri1V3 = XMVector3TransformCoord(tri1V3, worldSpace);

        //Find the normal using U, V coordinates (two edges)
        XMVECTOR U = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMVECTOR V = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMVECTOR faceNormal = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

        U = tri1V2 - tri1V1;
        V = tri1V3 - tri1V1;

        //Compute face normal by crossing U, V
        faceNormal = XMVector3Cross(U, V);

        faceNormal = XMVector3Normalize(faceNormal);

        //Calculate a point on the triangle for the plane equation
        XMVECTOR triPoint = tri1V1;

        //Get plane equation ("Ax + By + Cz + D = 0") Variables
        float tri1A = XMVectorGetX(faceNormal);
        float tri1B = XMVectorGetY(faceNormal);
        float tri1C = XMVectorGetZ(faceNormal);
        float tri1D = (-tri1A*XMVectorGetX(triPoint) - tri1B*XMVectorGetY(triPoint) - tri1C*XMVectorGetZ(triPoint));

        //Now we find where (on the ray) the ray intersects with the triangles plane
        float ep1, ep2, t = 0.0f;
        float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
        XMVECTOR pointInPlane = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

        ep1 = (XMVectorGetX(pickRayInWorldSpacePos) * tri1A) + (XMVectorGetY(pickRayInWorldSpacePos) * tri1B) + (XMVectorGetZ(pickRayInWorldSpacePos) * tri1C);
        ep2 = (XMVectorGetX(pickRayInWorldSpaceDir) * tri1A) + (XMVectorGetY(pickRayInWorldSpaceDir) * tri1B) + (XMVectorGetZ(pickRayInWorldSpaceDir) * tri1C);

        //Make sure there are no divide-by-zeros
        if(ep2 != 0.0f)
            t = -(ep1 + tri1D)/(ep2);

        if(t > 0.0f)    //Make sure you don't pick objects behind the camera
        {
            //Get the point on the plane
            planeIntersectX = XMVectorGetX(pickRayInWorldSpacePos) + XMVectorGetX(pickRayInWorldSpaceDir) * t;
            planeIntersectY = XMVectorGetY(pickRayInWorldSpacePos) + XMVectorGetY(pickRayInWorldSpaceDir) * t;
            planeIntersectZ = XMVectorGetZ(pickRayInWorldSpacePos) + XMVectorGetZ(pickRayInWorldSpaceDir) * t;

            pointInPlane = XMVectorSet(planeIntersectX, planeIntersectY, planeIntersectZ, 0.0f);

            //Call function to check if point is in the triangle
            if(PointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
            {
                //Return the distance to the hit, so you can check all the other pickable objects in your scene
                //and choose whichever object is closest to the camera
                return t/2.0f;
            }
        }
    }
    //return the max float value (near infinity) if an object was not picked
    return FLT_MAX;
}

bool PointInTriangle(XMVECTOR& triV1, XMVECTOR& triV2, XMVECTOR& triV3, XMVECTOR& point )
{
    //To find out if the point is inside the triangle, we will check to see if the point
    //is on the correct side of each of the triangles edges.

    XMVECTOR cp1 = XMVector3Cross((triV3 - triV2), (point - triV2));
    XMVECTOR cp2 = XMVector3Cross((triV3 - triV2), (triV1 - triV2));
    if(XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
    {
        cp1 = XMVector3Cross((triV3 - triV1), (point - triV1));
        cp2 = XMVector3Cross((triV3 - triV1), (triV2 - triV1));
        if(XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
        {
            cp1 = XMVector3Cross((triV2 - triV1), (point - triV1));
            cp2 = XMVector3Cross((triV2 - triV1), (triV3 - triV1));
            if(XMVectorGetX(XMVector3Dot(cp1, cp2)) >= 0)
            {
                return true;
            }
            else
                return false;
        }
        else
            return false;
    }
    return false;
}

void CreateBoundingVolumes(std::vector<XMFLOAT3> &vertPosArray,
    std::vector<XMFLOAT3>& boundingBoxVerts,
    std::vector<DWORD>& boundingBoxIndex,
    float &boundingSphere,
    XMVECTOR &objectCenterOffset)
{
    XMFLOAT3 minVertex = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
    XMFLOAT3 maxVertex = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for(UINT i = 0; i < vertPosArray.size(); i++)
    {        
        // The minVertex and maxVertex will most likely not be actual vertices in the model, but vertices
        // that use the smallest and largest x, y, and z values from the model to be sure ALL vertices are
        // covered by the bounding volume

        //Get the smallest vertex 
        minVertex.x = std::min(minVertex.x, vertPosArray[i].x);    // Find smallest x value in model
        minVertex.y = std::min(minVertex.y, vertPosArray[i].y);    // Find smallest y value in model
        minVertex.z = std::min(minVertex.z, vertPosArray[i].z);    // Find smallest z value in model

        //Get the largest vertex 
        maxVertex.x = std::max(maxVertex.x, vertPosArray[i].x);    // Find largest x value in model
        maxVertex.y = std::max(maxVertex.y, vertPosArray[i].y);    // Find largest y value in model
        maxVertex.z = std::max(maxVertex.z, vertPosArray[i].z);    // Find largest z value in model
    }

    // Compute distance between maxVertex and minVertex
    float distX = (maxVertex.x - minVertex.x) / 2.0f;
    float distY = (maxVertex.y - minVertex.y) / 2.0f;
    float distZ = (maxVertex.z - minVertex.z) / 2.0f;    

    // Now store the distance between (0, 0, 0) in model space to the models real center
    objectCenterOffset = XMVectorSet(maxVertex.x - distX, maxVertex.y - distY, maxVertex.z - distZ, 0.0f);

    // Compute bounding sphere (distance between min and max bounding box vertices)
    // boundingSphere = sqrt(distX*distX + distY*distY + distZ*distZ) / 2.0f;
    boundingSphere = XMVectorGetX(XMVector3Length(XMVectorSet(distX, distY, distZ, 0.0f)));    

    // Create bounding box    
    // Front Vertices
    boundingBoxVerts.push_back(XMFLOAT3(minVertex.x, minVertex.y, minVertex.z));
    boundingBoxVerts.push_back(XMFLOAT3(minVertex.x, maxVertex.y, minVertex.z));
    boundingBoxVerts.push_back(XMFLOAT3(maxVertex.x, maxVertex.y, minVertex.z));
    boundingBoxVerts.push_back(XMFLOAT3(maxVertex.x, minVertex.y, minVertex.z));

    // Back Vertices
    boundingBoxVerts.push_back(XMFLOAT3(minVertex.x, minVertex.y, maxVertex.z));
    boundingBoxVerts.push_back(XMFLOAT3(maxVertex.x, minVertex.y, maxVertex.z));
    boundingBoxVerts.push_back(XMFLOAT3(maxVertex.x, maxVertex.y, maxVertex.z));
    boundingBoxVerts.push_back(XMFLOAT3(minVertex.x, maxVertex.y, maxVertex.z));

    DWORD* i = new DWORD[36];

    // Front Face
    i[0] = 0; i[1] = 1; i[2] = 2;
    i[3] = 0; i[4] = 2; i[5] = 3;

    // Back Face
    i[6] = 4; i[7]  = 5; i[8]  = 6;
    i[9] = 4; i[10] = 6; i[11] = 7;

    // Top Face
    i[12] = 1; i[13] = 7; i[14] = 6;
    i[15] = 1; i[16] = 6; i[17] = 2;

    // Bottom Face
    i[18] = 0; i[19] = 4; i[20] = 5;
    i[21] = 0; i[22] = 5; i[23] = 3;

    // Left Face
    i[24] = 4; i[25] = 7; i[26] = 1;
    i[27] = 4; i[28] = 1; i[29] = 0;

    // Right Face
    i[30] = 3; i[31] = 2; i[32] = 6;
    i[33] = 3; i[34] = 6; i[35] = 5;

    for(int j = 0; j < 36; j++)
        boundingBoxIndex.push_back(i[j]);
}

bool BoundingSphereCollision(float firstObjBoundingSphere, 
    XMVECTOR firstObjCenterOffset,
    XMMATRIX& firstObjWorldSpace,
    float secondObjBoundingSphere,
    XMVECTOR secondObjCenterOffset, 
    XMMATRIX& secondObjWorldSpace)
{
    //Declare local variables
    XMVECTOR world_1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR world_2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    float objectsDistance = 0.0f;

    //Transform the objects world space to objects REAL center in world space
    world_1 = XMVector3TransformCoord(firstObjCenterOffset, firstObjWorldSpace);
    world_2 = XMVector3TransformCoord(secondObjCenterOffset, secondObjWorldSpace);

    //Get the distance between the two objects
    objectsDistance = XMVectorGetX(XMVector3Length(world_1 - world_2));

    //If the distance between the two objects is less than the sum of their bounding spheres...
    if(objectsDistance <= (firstObjBoundingSphere + secondObjBoundingSphere))
        //Return true
        return true;

    //If the bounding spheres are not colliding, return false
    return false;
}

bool BoundingBoxCollision(XMVECTOR& firstObjBoundingBoxMinVertex, 
    XMVECTOR& firstObjBoundingBoxMaxVertex, 
    XMVECTOR& secondObjBoundingBoxMinVertex, 
    XMVECTOR& secondObjBoundingBoxMaxVertex) 
{
    //Is obj1's max X greater than obj2's min X? If not, obj1 is to the LEFT of obj2
    if (XMVectorGetX(firstObjBoundingBoxMaxVertex) > XMVectorGetX(secondObjBoundingBoxMinVertex))

        //Is obj1's min X less than obj2's max X? If not, obj1 is to the RIGHT of obj2
        if (XMVectorGetX(firstObjBoundingBoxMinVertex) < XMVectorGetX(secondObjBoundingBoxMaxVertex))

            //Is obj1's max Y greater than obj2's min Y? If not, obj1 is UNDER obj2
            if (XMVectorGetY(firstObjBoundingBoxMaxVertex) > XMVectorGetY(secondObjBoundingBoxMinVertex))

                //Is obj1's min Y less than obj2's max Y? If not, obj1 is ABOVE obj2
                if (XMVectorGetY(firstObjBoundingBoxMinVertex) < XMVectorGetY(secondObjBoundingBoxMaxVertex)) 

                    //Is obj1's max Z greater than obj2's min Z? If not, obj1 is IN FRONT OF obj2
                    if (XMVectorGetZ(firstObjBoundingBoxMaxVertex) > XMVectorGetZ(secondObjBoundingBoxMinVertex))

                        //Is obj1's min Z less than obj2's max Z? If not, obj1 is BEHIND obj2
                        if (XMVectorGetZ(firstObjBoundingBoxMinVertex) < XMVectorGetZ(secondObjBoundingBoxMaxVertex))

                            //If we've made it this far, then the two bounding boxes are colliding
                            return true;

    //If the two bounding boxes are not colliding, then return false
    return false;
}

void CalculateAABB(std::vector<XMFLOAT3> boundingBoxVerts, 
    XMMATRIX& worldSpace,
    XMVECTOR& boundingBoxMin,
    XMVECTOR& boundingBoxMax)
{
    XMFLOAT3 minVertex = XMFLOAT3(FLT_MAX, FLT_MAX, FLT_MAX);
    XMFLOAT3 maxVertex = XMFLOAT3(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    //Loop through the 8 vertices describing the bounding box
    for(UINT i = 0; i < 8; i++)
    {        
        //Transform the bounding boxes vertices to the objects world space
        XMVECTOR Vert = XMVectorSet(boundingBoxVerts[i].x, boundingBoxVerts[i].y, boundingBoxVerts[i].z, 0.0f);
        Vert = XMVector3TransformCoord(Vert, worldSpace);

        //Get the smallest vertex 
        minVertex.x = std::min(minVertex.x, XMVectorGetX(Vert));    // Find smallest x value in model
        minVertex.y = std::min(minVertex.y, XMVectorGetY(Vert));    // Find smallest y value in model
        minVertex.z = std::min(minVertex.z, XMVectorGetZ(Vert));    // Find smallest z value in model

        //Get the largest vertex 
        maxVertex.x = std::max(maxVertex.x, XMVectorGetX(Vert));    // Find largest x value in model
        maxVertex.y = std::max(maxVertex.y, XMVectorGetY(Vert));    // Find largest y value in model
        maxVertex.z = std::max(maxVertex.z, XMVectorGetZ(Vert));    // Find largest z value in model
    }

    //Store Bounding Box's min and max vertices
    boundingBoxMin = XMVectorSet(minVertex.x, minVertex.y, minVertex.z, 0.0f);
    boundingBoxMax = XMVectorSet(maxVertex.x, maxVertex.y, maxVertex.z, 0.0f);
}

void MoveChar(double time, XMVECTOR& destinationDirection, XMMATRIX& worldMatrix)
{
    // Normalize our destinated direction vector
    destinationDirection = XMVector3Normalize(destinationDirection);

    // If character is currently facing the complete opposite direction as the desired direction
    // they will turn around VERY slowly, so we want to make sure they turn around at a normal speed
    // by making the old character direction not the exact opposite direction as the current character
    // position. Try commenting out the next two lines to see what i'm talking about
    if(XMVectorGetX(XMVector3Dot(destinationDirection, oldCharDirection)) == -1)
        oldCharDirection += XMVectorSet(0.02f, 0.0f, -0.02f, 0.0f);

    // Get our current characters position in the world, from it's world matrix
    charPosition = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    charPosition = XMVector3TransformCoord(charPosition, worldMatrix);

    // Rotate our character smoothly when changing direction (from the GPG series)
    float destDirLength = 10.0f * frameTime;        // Change to the speed you want your character to rotate. This uses the game timer from an earlier lesson
                                                // The larget this value, the faster the character rotates
    currCharDirection = oldCharDirection + (destinationDirection * destDirLength);    // Get the characters direction (based off time, old position, and desired
                                            // direction), by adding together the current direction and the old direction
                                        // to get vector that smoothly turns from oldCharDir to denstinationDirection

currCharDirection = XMVector3Normalize(currCharDirection);        // Normalize the characters current direction vector

// Here we find the angle of our character (angle between current direction and world's normal vector), used so that we can actually rotate
// our characters world matrix. The three lines below, together, find the angle between 0 PI and 2 PI (360 degrees, and technically, it returns
// the degrees in radians from -1 PI to 1 PI, but that has the same effect as 0 PI to 2 PI) between two vectors.
// XMVector3AngleBetweenNormals returns an angle between two vectors, but always a positive result between
// 0 and 1 PI. Which means, it doesn't tell us which half of the 2 PI degrees that are possible. So, we have the next if statement below,
// which crosses the current characters direction and the worlds forward (0,0,1), which should give us the y axis vector (assuming that our character
// rotates on the xz plane). We check to see if the y vector is positive ( > 0.0f), and if it is, we set the characters direction angle to be
// the opposite of what it currently is, giving us the result in -1 PI to 1 PI.
float charDirAngle = XMVectorGetX(XMVector3AngleBetweenNormals( XMVector3Normalize(currCharDirection), XMVector3Normalize(DefaultForward)));
if(XMVectorGetY(XMVector3Cross(currCharDirection, DefaultForward)) > 0.0f)
    charDirAngle = -charDirAngle;

// Now we update our characters position based off the frame time, his old position, and the direction he is facing
float speed = 15.0f * frameTime;
charPosition = charPosition + (destinationDirection * speed);

// Update characters world matrix
XMMATRIX rotationMatrix;
Scale = XMMatrixScaling( 0.04f, 0.04f, 0.04f );
Translation = XMMatrixTranslation(XMVectorGetX(charPosition), 0.0f, XMVectorGetZ(charPosition) );
rotationMatrix = XMMatrixRotationY(charDirAngle - 3.14159265f);        // Subtract PI from angle so the character doesn't run backwards

worldMatrix = Scale * rotationMatrix * Translation;

// Set the characters old direction
oldCharDirection = currCharDirection;                                                                    

// Update our animation
float timeFactor = 1.0f;    // You can speed up or slow down time by changing this
UpdateMD5Model(NewMD5Model, time*timeFactor, 0);
}

//This function is copied from the braynar tutorial. Since I wrote my own crappy one in old commit I decided to skip implementing and just copied
bool LoadObjModel(std::wstring filename, 
    ID3D11Buffer** vertBuff, 
    ID3D11Buffer** indexBuff,
    std::vector<int>& subsetIndexStart,
    std::vector<int>& subsetMaterialArray,
    std::vector<SurfaceMaterial>& material, 
    int& subsetCount,
    bool isRHCoordSys,
    bool computeNormals,
    std::vector<XMFLOAT3>& vertPosArray,
    std::vector<DWORD>& vertIndexArray)
{
    HRESULT hr = 0;

    std::wifstream fileIn (filename.c_str());    //Open file
    std::wstring meshMatLib;                    //String to hold our obj material library filename

    //Arrays to store our model's information
    std::vector<DWORD> indices;
    std::vector<XMFLOAT3> vertPos;
    std::vector<XMFLOAT3> vertNorm;
    std::vector<XMFLOAT2> vertTexCoord;
    std::vector<std::wstring> meshMaterials;

    //Vertex definition indices
    std::vector<int> vertPosIndex;
    std::vector<int> vertNormIndex;
    std::vector<int> vertTCIndex;

    //Make sure we have a default if no tex coords or normals are defined
    bool hasTexCoord = false;
    bool hasNorm = false;

    //Temp variables to store into vectors
    std::wstring meshMaterialsTemp;
    int vertPosIndexTemp;
    int vertNormIndexTemp;
    int vertTCIndexTemp;

    wchar_t checkChar;        //The variable we will use to store one char from file at a time
    std::wstring face;        //Holds the string containing our face vertices
    int vIndex = 0;            //Keep track of our vertex index count
    int triangleCount = 0;    //Total Triangles
    int totalVerts = 0;
    int meshTriangles = 0;

    //Check to see if the file was opened
    if (fileIn)
    {
        while(fileIn)
        {            
            checkChar = fileIn.get();    //Get next char

            switch (checkChar)
            {        
            case '#':
                checkChar = fileIn.get();
                while(checkChar != '\n')
                    checkChar = fileIn.get();
                break;
            case 'v':    //Get Vertex Descriptions
                checkChar = fileIn.get();
                if(checkChar == ' ')    //v - vert position
                {
                    float vz, vy, vx;
                    fileIn >> vx >> vy >> vz;    //Store the next three types

                    if(isRHCoordSys)    //If model is from an RH Coord System
                        vertPos.push_back(XMFLOAT3( vx, vy, vz * -1.0f));    //Invert the Z axis
                    else
                        vertPos.push_back(XMFLOAT3( vx, vy, vz));
                }
                if(checkChar == 't')    //vt - vert tex coords
                {            
                    float vtcu, vtcv;
                    fileIn >> vtcu >> vtcv;        //Store next two types

                    if(isRHCoordSys)    //If model is from an RH Coord System
                        vertTexCoord.push_back(XMFLOAT2(vtcu, 1.0f-vtcv));    //Reverse the "v" axis
                    else
                        vertTexCoord.push_back(XMFLOAT2(vtcu, vtcv));    

                    hasTexCoord = true;    //We know the model uses texture coords
                }
                //Since we compute the normals later, we don't need to check for normals
                //In the file, but i'll do it here anyway
                if(checkChar == 'n')    //vn - vert normal
                {
                    float vnx, vny, vnz;
                    fileIn >> vnx >> vny >> vnz;    //Store next three types

                    if(isRHCoordSys)    //If model is from an RH Coord System
                        vertNorm.push_back(XMFLOAT3( vnx, vny, vnz * -1.0f ));    //Invert the Z axis
                    else
                        vertNorm.push_back(XMFLOAT3( vnx, vny, vnz ));    

                    hasNorm = true;    //We know the model defines normals
                }
                break;

                //New group (Subset)
            case 'g':    //g - defines a group
                checkChar = fileIn.get();
                if(checkChar == ' ')
                {
                    subsetIndexStart.push_back(vIndex);        //Start index for this subset
                    subsetCount++;
                }
                break;

                //Get Face Index
            case 'f':    //f - defines the faces
                checkChar = fileIn.get();
                if(checkChar == ' ')
                {
                    face = L"";
                    std::wstring VertDef;    //Holds one vertex definition at a time
                    triangleCount = 0;

                    checkChar = fileIn.get();
                    while(checkChar != '\n')
                    {
                        face += checkChar;            //Add the char to our face string
                        checkChar = fileIn.get();    //Get the next Character
                        if(checkChar == ' ')        //If its a space...
                            triangleCount++;        //Increase our triangle count
                    }

                    //Check for space at the end of our face string
                    if(face[face.length()-1] == ' ')
                        triangleCount--;    //Each space adds to our triangle count

                    triangleCount -= 1;        //Ever vertex in the face AFTER the first two are new faces

                    std::wstringstream ss(face);

                    if(face.length() > 0)
                    {
                        int firstVIndex, lastVIndex;    //Holds the first and last vertice's index

                        for(int i = 0; i < 3; ++i)        //First three vertices (first triangle)
                        {
                            ss >> VertDef;    //Get vertex definition (vPos/vTexCoord/vNorm)

                            std::wstring vertPart;
                            int whichPart = 0;        //(vPos, vTexCoord, or vNorm)

                            //Parse this string
                            for(int j = 0; j < VertDef.length(); ++j)
                            {
                                if(VertDef[j] != '/')    //If there is no divider "/", add a char to our vertPart
                                    vertPart += VertDef[j];

                                //If the current char is a divider "/", or its the last character in the string
                                if(VertDef[j] == '/' || j ==  VertDef.length()-1)
                                {
                                    std::wistringstream wstringToInt(vertPart);    //Used to convert wstring to int

                                    if(whichPart == 0)    //If vPos
                                    {
                                        wstringToInt >> vertPosIndexTemp;
                                        vertPosIndexTemp -= 1;        //subtract one since c++ arrays start with 0, and obj start with 1

                                        //Check to see if the vert pos was the only thing specified
                                        if(j == VertDef.length()-1)
                                        {
                                            vertNormIndexTemp = 0;
                                            vertTCIndexTemp = 0;
                                        }
                                    }

                                    else if(whichPart == 1)    //If vTexCoord
                                    {
                                        if(vertPart != L"")    //Check to see if there even is a tex coord
                                        {
                                            wstringToInt >> vertTCIndexTemp;
                                            vertTCIndexTemp -= 1;    //subtract one since c++ arrays start with 0, and obj start with 1
                                        }
                                        else    //If there is no tex coord, make a default
                                            vertTCIndexTemp = 0;

                                        //If the cur. char is the second to last in the string, then
                                        //there must be no normal, so set a default normal
                                        if(j == VertDef.length()-1)
                                            vertNormIndexTemp = 0;

                                    }                                
                                    else if(whichPart == 2)    //If vNorm
                                    {
                                        std::wistringstream wstringToInt(vertPart);

                                        wstringToInt >> vertNormIndexTemp;
                                        vertNormIndexTemp -= 1;        //subtract one since c++ arrays start with 0, and obj start with 1
                                    }

                                    vertPart = L"";    //Get ready for next vertex part
                                    whichPart++;    //Move on to next vertex part                    
                                }
                            }

                            //Check to make sure there is at least one subset
                            if(subsetCount == 0)
                            {
                                subsetIndexStart.push_back(vIndex);        //Start index for this subset
                                subsetCount++;
                            }

                            //Avoid duplicate vertices
                            bool vertAlreadyExists = false;
                            if(totalVerts >= 3)    //Make sure we at least have one triangle to check
                            {
                                //Loop through all the vertices
                                for(int iCheck = 0; iCheck < totalVerts; ++iCheck)
                                {
                                    //If the vertex position and texture coordinate in memory are the same
                                    //As the vertex position and texture coordinate we just now got out
                                    //of the obj file, we will set this faces vertex index to the vertex's
                                    //index value in memory. This makes sure we don't create duplicate vertices
                                    if(vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
                                    {
                                        if(vertTCIndexTemp == vertTCIndex[iCheck])
                                        {
                                            indices.push_back(iCheck);        //Set index for this vertex
                                            vertAlreadyExists = true;        //If we've made it here, the vertex already exists
                                        }
                                    }
                                }
                            }

                            //If this vertex is not already in our vertex arrays, put it there
                            if(!vertAlreadyExists)
                            {
                                vertPosIndex.push_back(vertPosIndexTemp);
                                vertTCIndex.push_back(vertTCIndexTemp);
                                vertNormIndex.push_back(vertNormIndexTemp);
                                totalVerts++;    //We created a new vertex
                                indices.push_back(totalVerts-1);    //Set index for this vertex
                            }                            

                            //If this is the very first vertex in the face, we need to
                            //make sure the rest of the triangles use this vertex
                            if(i == 0)
                            {
                                firstVIndex = indices[vIndex];    //The first vertex index of this FACE

                            }

                            //If this was the last vertex in the first triangle, we will make sure
                            //the next triangle uses this one (eg. tri1(1,2,3) tri2(1,3,4) tri3(1,4,5))
                            if(i == 2)
                            {                                
                                lastVIndex = indices[vIndex];    //The last vertex index of this TRIANGLE
                            }
                            vIndex++;    //Increment index count
                        }

                        meshTriangles++;    //One triangle down

                        //If there are more than three vertices in the face definition, we need to make sure
                        //we convert the face to triangles. We created our first triangle above, now we will
                        //create a new triangle for every new vertex in the face, using the very first vertex
                        //of the face, and the last vertex from the triangle before the current triangle
                        for(int l = 0; l < triangleCount-1; ++l)    //Loop through the next vertices to create new triangles
                        {
                            //First vertex of this triangle (the very first vertex of the face too)
                            indices.push_back(firstVIndex);            //Set index for this vertex
                            vIndex++;

                            //Second Vertex of this triangle (the last vertex used in the tri before this one)
                            indices.push_back(lastVIndex);            //Set index for this vertex
                            vIndex++;

                            //Get the third vertex for this triangle
                            ss >> VertDef;

                            std::wstring vertPart;
                            int whichPart = 0;

                            //Parse this string (same as above)
                            for(int j = 0; j < VertDef.length(); ++j)
                            {
                                if(VertDef[j] != '/')
                                    vertPart += VertDef[j];
                                if(VertDef[j] == '/' || j ==  VertDef.length()-1)
                                {
                                    std::wistringstream wstringToInt(vertPart);

                                    if(whichPart == 0)
                                    {
                                        wstringToInt >> vertPosIndexTemp;
                                        vertPosIndexTemp -= 1;

                                        //Check to see if the vert pos was the only thing specified
                                        if(j == VertDef.length()-1)
                                        {
                                            vertTCIndexTemp = 0;
                                            vertNormIndexTemp = 0;
                                        }
                                    }
                                    else if(whichPart == 1)
                                    {
                                        if(vertPart != L"")
                                        {
                                            wstringToInt >> vertTCIndexTemp;
                                            vertTCIndexTemp -= 1;
                                        }
                                        else
                                            vertTCIndexTemp = 0;
                                        if(j == VertDef.length()-1)
                                            vertNormIndexTemp = 0;

                                    }                                
                                    else if(whichPart == 2)
                                    {
                                        std::wistringstream wstringToInt(vertPart);

                                        wstringToInt >> vertNormIndexTemp;
                                        vertNormIndexTemp -= 1;
                                    }

                                    vertPart = L"";
                                    whichPart++;                            
                                }
                            }                    

                            //Check for duplicate vertices
                            bool vertAlreadyExists = false;
                            if(totalVerts >= 3)    //Make sure we at least have one triangle to check
                            {
                                for(int iCheck = 0; iCheck < totalVerts; ++iCheck)
                                {
                                    if(vertPosIndexTemp == vertPosIndex[iCheck] && !vertAlreadyExists)
                                    {
                                        if(vertTCIndexTemp == vertTCIndex[iCheck])
                                        {
                                            indices.push_back(iCheck);            //Set index for this vertex
                                            vertAlreadyExists = true;        //If we've made it here, the vertex already exists
                                        }
                                    }
                                }
                            }

                            if(!vertAlreadyExists)
                            {
                                vertPosIndex.push_back(vertPosIndexTemp);
                                vertTCIndex.push_back(vertTCIndexTemp);
                                vertNormIndex.push_back(vertNormIndexTemp);
                                totalVerts++;                    //New vertex created, add to total verts
                                indices.push_back(totalVerts-1);        //Set index for this vertex
                            }

                            //Set the second vertex for the next triangle to the last vertex we got        
                            lastVIndex = indices[vIndex];    //The last vertex index of this TRIANGLE

                            meshTriangles++;    //New triangle defined
                            vIndex++;        
                        }
                    }
                }
                break;

            case 'm':    //mtllib - material library filename
                checkChar = fileIn.get();
                if(checkChar == 't')
                {
                    checkChar = fileIn.get();
                    if(checkChar == 'l')
                    {
                        checkChar = fileIn.get();
                        if(checkChar == 'l')
                        {
                            checkChar = fileIn.get();
                            if(checkChar == 'i')
                            {
                                checkChar = fileIn.get();
                                if(checkChar == 'b')
                                {
                                    checkChar = fileIn.get();
                                    if(checkChar == ' ')
                                    {
                                        //Store the material libraries file name
                                        fileIn >> meshMatLib;
                                    }
                                }
                            }
                        }
                    }
                }

                break;

            case 'u':    //usemtl - which material to use
                checkChar = fileIn.get();
                if(checkChar == 's')
                {
                    checkChar = fileIn.get();
                    if(checkChar == 'e')
                    {
                        checkChar = fileIn.get();
                        if(checkChar == 'm')
                        {
                            checkChar = fileIn.get();
                            if(checkChar == 't')
                            {
                                checkChar = fileIn.get();
                                if(checkChar == 'l')
                                {
                                    checkChar = fileIn.get();
                                    if(checkChar == ' ')
                                    {
                                        meshMaterialsTemp = L"";    //Make sure this is cleared

                                        fileIn >> meshMaterialsTemp; //Get next type (string)

                                        meshMaterials.push_back(meshMaterialsTemp);
                                    }
                                }
                            }
                        }
                    }
                }
                break;

            default:                
                break;
            }
        }
    }
    else    //If we could not open the file
    {
        swapchain->SetFullscreenState(false, NULL);    //Make sure we are out of fullscreen

        //create message
        std::wstring message = L"Could not open: ";
        message += filename;

        MessageBox(0, message.c_str(),    //display message
            L"Error", MB_OK);

        return false;
    }

    subsetIndexStart.push_back(vIndex); //There won't be another index start after our last subset, so set it here

    //sometimes "g" is defined at the very top of the file, then again before the first group of faces.
    //This makes sure the first subset does not conatain "0" indices.
    if(subsetIndexStart[1] == 0)
    {
        subsetIndexStart.erase(subsetIndexStart.begin()+1);
        meshSubsets--;
    }

    //Make sure we have a default for the tex coord and normal
    //if one or both are not specified
    if(!hasNorm)
        vertNorm.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));
    if(!hasTexCoord)
        vertTexCoord.push_back(XMFLOAT2(0.0f, 0.0f));

    //Close the obj file, and open the mtl file
    fileIn.close();
    fileIn.open(meshMatLib.c_str());

    std::wstring lastStringRead;
    int matCount = 0;    //total materials

    //kdset - If our diffuse color was not set, we can use the ambient color (which is usually the same)
    //If the diffuse color WAS set, then we don't need to set our diffuse color to ambient
    bool kdset = false;

    if (fileIn)
    {
        while(fileIn)
        {
            checkChar = fileIn.get();    //Get next char

            switch (checkChar)
            {
                //Check for comment
            case '#':
                checkChar = fileIn.get();
                while(checkChar != '\n')
                    checkChar = fileIn.get();
                break;

                //Set diffuse color
            case 'K':
                checkChar = fileIn.get();
                if(checkChar == 'd')    //Diffuse Color
                {
                    checkChar = fileIn.get();    //remove space

                    fileIn >> material[matCount-1].difColor.x;
                    fileIn >> material[matCount-1].difColor.y;
                    fileIn >> material[matCount-1].difColor.z;

                    kdset = true;
                }

                //Ambient Color (We'll store it in diffuse if there isn't a diffuse already)
                if(checkChar == 'a')    
                {                    
                    checkChar = fileIn.get();    //remove space
                    if(!kdset)
                    {
                        fileIn >> material[matCount-1].difColor.x;
                        fileIn >> material[matCount-1].difColor.y;
                        fileIn >> material[matCount-1].difColor.z;
                    }
                }
                break;

                //Check for transparency
            case 'T':
                checkChar = fileIn.get();
                if(checkChar == 'r')
                {
                    checkChar = fileIn.get();    //remove space
                    float Transparency;
                    fileIn >> Transparency;

                    material[matCount-1].difColor.w = Transparency;

                    if(Transparency > 0.0f)
                        material[matCount-1].transparent = true;
                }
                break;

                //Some obj files specify d for transparency
            case 'd':
                checkChar = fileIn.get();
                if(checkChar == ' ')
                {
                    float Transparency;
                    fileIn >> Transparency;

                    //'d' - 0 being most transparent, and 1 being opaque, opposite of Tr
                    Transparency = 1.0f - Transparency;

                    material[matCount-1].difColor.w = Transparency;

                    if(Transparency > 0.0f)
                        material[matCount-1].transparent = true;                    
                }
                break;

                //Get the diffuse map (texture)
            case 'm':
                checkChar = fileIn.get();
                if(checkChar == 'a')
                {
                    checkChar = fileIn.get();
                    if(checkChar == 'p')
                    {
                        checkChar = fileIn.get();
                        if(checkChar == '_')
                        {
                            //map_Kd - Diffuse map
                            checkChar = fileIn.get();
                            if(checkChar == 'K')
                            {
                                checkChar = fileIn.get();
                                if(checkChar == 'd')
                                {
                                    std::wstring fileNamePath;

                                    fileIn.get();    //Remove whitespace between map_Kd and file

                                    //Get the file path - We read the pathname char by char since
                                    //pathnames can sometimes contain spaces, so we will read until
                                    //we find the file extension
                                    bool texFilePathEnd = false;
                                    while(!texFilePathEnd)
                                    {
                                        checkChar = fileIn.get();

                                        fileNamePath += checkChar;

                                        if(checkChar == '.')
                                        {
                                            for(int i = 0; i < 3; ++i)
                                                fileNamePath += fileIn.get();

                                            texFilePathEnd = true;
                                        }                            
                                    }

                                    //check if this texture has already been loaded
                                    bool alreadyLoaded = false;
                                    for(int i = 0; i < textureNameArray.size(); ++i)
                                    {
                                        if(fileNamePath == textureNameArray[i])
                                        {
                                            alreadyLoaded = true;
                                            material[matCount-1].texArrayIndex = i;
                                            material[matCount-1].hasTexture = true;
                                        }
                                    }

                                    //if the texture is not already loaded, load it now
                                    if(!alreadyLoaded)
                                    {
                                        ID3D11ShaderResourceView* tempMeshSRV;
                                        hr = CreateWICTextureFromFile(dev, devcon, fileNamePath.c_str(), NULL, &tempMeshSRV);
                                        if(SUCCEEDED(hr))
                                        {
                                            textureNameArray.push_back(fileNamePath.c_str());
                                            material[matCount-1].texArrayIndex = meshSRV.size();
                                            meshSRV.push_back(tempMeshSRV);
                                            material[matCount-1].hasTexture = true;
                                        }
                                    }    
                                }
                            }
                            //map_d - alpha map
                            else if(checkChar == 'd')
                            {
                                //Alpha maps are usually the same as the diffuse map
                                //So we will assume that for now by only enabling
                                //transparency for this material, as we will already
                                //be using the alpha channel in the diffuse map
                                material[matCount-1].transparent = true;
                            }

                            
                            //map_bump - bump map (we're usinga normal map though)
                            else if(checkChar == 'b')
                            {
                                checkChar = fileIn.get();
                                if(checkChar == 'u')
                                {
                                    checkChar = fileIn.get();
                                    if(checkChar == 'm')
                                    {
                                        checkChar = fileIn.get();
                                        if(checkChar == 'p')
                                        {
                                            std::wstring fileNamePath;

                                            fileIn.get();    //Remove whitespace between map_bump and file

                                            //Get the file path - We read the pathname char by char since
                                            //pathnames can sometimes contain spaces, so we will read until
                                            //we find the file extension
                                            bool texFilePathEnd = false;
                                            while(!texFilePathEnd)
                                            {
                                                checkChar = fileIn.get();

                                                fileNamePath += checkChar;

                                                if(checkChar == '.')
                                                {
                                                    for(int i = 0; i < 3; ++i)
                                                        fileNamePath += fileIn.get();

                                                    texFilePathEnd = true;
                                                }                            
                                            }

                                            //check if this texture has already been loaded
                                            bool alreadyLoaded = false;
                                            for(int i = 0; i < textureNameArray.size(); ++i)
                                            {
                                                if(fileNamePath == textureNameArray[i])
                                                {
                                                    alreadyLoaded = true;
                                                    material[matCount-1].normMapTexArrayIndex = i;
                                                    material[matCount-1].hasNormMap = true;
                                                }
                                            }

                                            //if the texture is not already loaded, load it now
                                            if(!alreadyLoaded)
                                            {
                                                ID3D11ShaderResourceView* tempMeshSRV;
                                                hr = CreateWICTextureFromFile(dev, devcon, fileNamePath.c_str(), NULL, &tempMeshSRV);
                                                if(SUCCEEDED(hr))
                                                {
                                                    textureNameArray.push_back(fileNamePath.c_str());
                                                    material[matCount-1].normMapTexArrayIndex = meshSRV.size();
                                                    meshSRV.push_back(tempMeshSRV);
                                                    material[matCount-1].hasNormMap = true;
                                                }
                                            }    
                                        }
                                    }
                                }
                            }
                            
                        }
                    }
                }
                break;

            case 'n':    //newmtl - Declare new material
                checkChar = fileIn.get();
                if(checkChar == 'e')
                {
                    checkChar = fileIn.get();
                    if(checkChar == 'w')
                    {
                        checkChar = fileIn.get();
                        if(checkChar == 'm')
                        {
                            checkChar = fileIn.get();
                            if(checkChar == 't')
                            {
                                checkChar = fileIn.get();
                                if(checkChar == 'l')
                                {
                                    checkChar = fileIn.get();
                                    if(checkChar == ' ')
                                    {
                                        //New material, set its defaults
                                        SurfaceMaterial tempMat;
                                        material.push_back(tempMat);
                                        fileIn >> material[matCount].matName;
                                        material[matCount].transparent = false;
                                        material[matCount].hasTexture = false;
                                        
                                        material[matCount].hasNormMap = false;
                                        material[matCount].normMapTexArrayIndex = 0;
                                        
                                        material[matCount].texArrayIndex = 0;
                                        matCount++;
                                        kdset = false;
                                    }
                                }
                            }
                        }
                    }
                }
                break;

            default:
                break;
            }
        }
    }    
    else
    {
        swapchain->SetFullscreenState(false, NULL);    //Make sure we are out of fullscreen

        std::wstring message = L"Could not open: ";
        message += meshMatLib;

        MessageBox(0, message.c_str(),
            L"Error", MB_OK);

        return false;
    }

    //Set the subsets material to the index value
    //of the its material in our material array
    for(int i = 0; i < meshSubsets; ++i)
    {
        bool hasMat = false;
        for(int j = 0; j < material.size(); ++j)
        {
            if(meshMaterials[i] == material[j].matName)
            {
                subsetMaterialArray.push_back(j);
                hasMat = true;
            }
        }
        if(!hasMat)
            subsetMaterialArray.push_back(0); //Use first material in array
    }

    std::vector<Vertex> vertices;
    Vertex tempVert;

    //Create our vertices using the information we got 
    //from the file and store them in a vector
    for(int j = 0 ; j < totalVerts; ++j)
    {
        tempVert.pos = vertPos[vertPosIndex[j]];
        tempVert.normal = vertNorm[vertNormIndex[j]];
        tempVert.texcoord = vertTexCoord[vertTCIndex[j]];

        vertices.push_back(tempVert);

        //Copy just the vertex positions to the vector
        vertPosArray.push_back(tempVert.pos);
    }

    //Copy the index list to the array
    vertIndexArray = indices;

    //////////////////////Compute Normals///////////////////////////
    //If computeNormals was set to true then we will create our own
    //normals, if it was set to false we will use the obj files normals
    if(computeNormals)
    {
        std::vector<XMFLOAT3> tempNormal;

        //normalized and unnormalized normals
        XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

        
        //tangent stuff
        std::vector<XMFLOAT3> tempTangent;
        XMFLOAT3 tangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
        float tcU1, tcV1, tcU2, tcV2;
        

        //Used to get vectors (sides) from the position of the verts
        float vecX, vecY, vecZ;

        //Two edges of our triangle
        XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

        //Compute face normals
        //And Tangents
        for(int i = 0; i < meshTriangles; ++i)
        {
            //Get the vector describing one edge of our triangle (edge 0,2)
            vecX = vertices[indices[(i*3)]].pos.x - vertices[indices[(i*3)+2]].pos.x;
            vecY = vertices[indices[(i*3)]].pos.y - vertices[indices[(i*3)+2]].pos.y;
            vecZ = vertices[indices[(i*3)]].pos.z - vertices[indices[(i*3)+2]].pos.z;        
            edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our first edge

            //Get the vector describing another edge of our triangle (edge 2,1)
            vecX = vertices[indices[(i*3)+2]].pos.x - vertices[indices[(i*3)+1]].pos.x;
            vecY = vertices[indices[(i*3)+2]].pos.y - vertices[indices[(i*3)+1]].pos.y;
            vecZ = vertices[indices[(i*3)+2]].pos.z - vertices[indices[(i*3)+1]].pos.z;        
            edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our second edge

            //Cross multiply the two edge vectors to get the un-normalized face normal
            XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));

            tempNormal.push_back(unnormalized);

            
            //Find first texture coordinate edge 2d vector
            tcU1 = vertices[indices[(i*3)]].texcoord.x - vertices[indices[(i*3)+2]].texcoord.x;
            tcV1 = vertices[indices[(i*3)]].texcoord.y - vertices[indices[(i*3)+2]].texcoord.y;

            //Find second texture coordinate edge 2d vector
            tcU2 = vertices[indices[(i*3)+2]].texcoord.x - vertices[indices[(i*3)+1]].texcoord.x;
            tcV2 = vertices[indices[(i*3)+2]].texcoord.y - vertices[indices[(i*3)+1]].texcoord.y;

            //Find tangent using both tex coord edges and position edges
            tangent.x = (tcV1 * XMVectorGetX(edge1) - tcV2 * XMVectorGetX(edge2)) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));
            tangent.y = (tcV1 * XMVectorGetY(edge1) - tcV2 * XMVectorGetY(edge2)) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));
            tangent.z = (tcV1 * XMVectorGetZ(edge1) - tcV2 * XMVectorGetZ(edge2)) * (1.0f / (tcU1 * tcV2 - tcU2 * tcV1));

            tempTangent.push_back(tangent);
            
        }

        //Compute vertex normals (normal Averaging)
        XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMVECTOR tangentSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        int facesUsing = 0;
        float tX, tY, tZ;    //temp axis variables

        //Go through each vertex
        for(int i = 0; i < totalVerts; ++i)
        {
            //Check which triangles use this vertex
            for(int j = 0; j < meshTriangles; ++j)
            {
                if(indices[j*3] == i ||
                    indices[(j*3)+1] == i ||
                    indices[(j*3)+2] == i)
                {
                    tX = XMVectorGetX(normalSum) + tempNormal[j].x;
                    tY = XMVectorGetY(normalSum) + tempNormal[j].y;
                    tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

                    normalSum = XMVectorSet(tX, tY, tZ, 0.0f);    //If a face is using the vertex, add the unormalized face normal to the normalSum

                            
                    //We can reuse tX, tY, tZ to sum up tangents
                    tX = XMVectorGetX(tangentSum) + tempTangent[j].x;
                    tY = XMVectorGetY(tangentSum) + tempTangent[j].y;
                    tZ = XMVectorGetZ(tangentSum) + tempTangent[j].z;

                    tangentSum = XMVectorSet(tX, tY, tZ, 0.0f); //sum up face tangents using this vertex
                    

                    facesUsing++;
                }
            }

            //Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
            // printf("%f\n", 1/float(facesUsing));
            normalSum = XMVectorScale(normalSum, 1/float(facesUsing));
            tangentSum = XMVectorScale(tangentSum, 1/float(facesUsing));


            //Normalize the normalSum vector and tangent
            normalSum = XMVector3Normalize(normalSum);
            
            tangentSum =  XMVector3Normalize(tangentSum);
            

            //Store the normal and tangent in our current vertex
            vertices[i].normal.x = XMVectorGetX(normalSum);
            vertices[i].normal.y = XMVectorGetY(normalSum);
            vertices[i].normal.z = XMVectorGetZ(normalSum);

            
            vertices[i].tangent.x = XMVectorGetX(tangentSum);
            vertices[i].tangent.y = XMVectorGetY(tangentSum);
            vertices[i].tangent.z = XMVectorGetZ(tangentSum);

            // printf("%f, %f, %f\n", vertices[i].normal.x,vertices[i].normal.y,vertices[i].normal.z);
            // printf("%f, %f, %f\n", vertices[i].tangent.x,vertices[i].tangent.y,vertices[i].tangent.z);
            

            //Clear normalSum, tangentSum and facesUsing for next vertex
            normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
            
            tangentSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
            
            facesUsing = 0;

        }
    }

    //Create index buffer
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * meshTriangles*3;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData;

    iinitData.pSysMem = &indices[0];
    dev->CreateBuffer(&indexBufferDesc, &iinitData, indexBuff);

    //Create Vertex Buffer
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof( Vertex ) * totalVerts;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData; 

    ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
    vertexBufferData.pSysMem = &vertices[0];
    hr = dev->CreateBuffer( &vertexBufferDesc, &vertexBufferData, vertBuff);

    return true;
}

bool LoadMD5Model(std::wstring filename,
    Model3D& MD5Model,
    std::vector<ID3D11ShaderResourceView*>& shaderResourceViewArray,
    std::vector<std::wstring> texFileNameArray)
{
    std::wifstream fileIn (filename.c_str());        // Open file

    std::wstring checkString;                        // Stores the next string from our file
    HRESULT hr;
    if(fileIn)                                        // Check if the file was opened
    {
        while(fileIn)                                // Loop until the end of the file is reached
        {    
            fileIn >> checkString;                    // Get next string from file

            if(checkString == L"MD5Version")        // Get MD5 version (this function supports version 10)
            {
                /*fileIn >> checkString;
                MessageBox(0, checkString.c_str(),    //display message
                L"MD5Version", MB_OK);*/
            }
            else if ( checkString == L"commandline" )
            {
                std::getline(fileIn, checkString);    // Ignore the rest of this line
            }
            else if ( checkString == L"numJoints" )
            {
                fileIn >> MD5Model.numJoints;        // Store number of joints
            }
            else if ( checkString == L"numMeshes" )
            {
                fileIn >> MD5Model.numSubsets;        // Store number of meshes or subsets which we will call them
            }
            else if ( checkString == L"joints" )
            {
                Joint tempJoint;

                fileIn >> checkString;                // Skip the "{"

                for(int i = 0; i < MD5Model.numJoints; i++)
                {
                    fileIn >> tempJoint.name;        // Store joints name
                    // Sometimes the names might contain spaces. If that is the case, we need to continue
                    // to read the name until we get to the closing " (quotation marks)
                    if(tempJoint.name[tempJoint.name.size()-1] != '"')
                    {
                        wchar_t checkChar;
                        bool jointNameFound = false;
                        while(!jointNameFound)
                        {
                            checkChar = fileIn.get();

                            if(checkChar == '"')
                                jointNameFound = true;        

                            tempJoint.name += checkChar;                                                            
                        }
                    }

                    fileIn >> tempJoint.parentID;    // Store Parent joint's ID

                    fileIn >> checkString;            // Skip the "("

                    // Store position of this joint (swap y and z axis if model was made in RH Coord Sys)
                    fileIn >> tempJoint.pos.x >> tempJoint.pos.z >> tempJoint.pos.y;

                    fileIn >> checkString >> checkString;    // Skip the ")" and "("

                    // Store orientation of this joint
                    fileIn >> tempJoint.orientation.x >> tempJoint.orientation.z >> tempJoint.orientation.y;

                    // Remove the quotation marks from joints name
                    tempJoint.name.erase(0, 1);
                    tempJoint.name.erase(tempJoint.name.size()-1, 1);

                    // Compute the w axis of the quaternion (The MD5 model uses a 3D vector to describe the
                    // direction the bone is facing. However, we need to turn this into a quaternion, and the way
                    // quaternions work, is the xyz values describe the axis of rotation, while the w is a value
                    // between 0 and 1 which describes the angle of rotation)
                    float t = 1.0f - ( tempJoint.orientation.x * tempJoint.orientation.x )
                        - ( tempJoint.orientation.y * tempJoint.orientation.y )
                        - ( tempJoint.orientation.z * tempJoint.orientation.z );
                    if ( t < 0.0f )
                    {
                        tempJoint.orientation.w = 0.0f;
                    }
                    else
                    {
                        tempJoint.orientation.w = -sqrtf(t);
                    }

                    std::getline(fileIn, checkString);        // Skip rest of this line

                    MD5Model.joints.push_back(tempJoint);    // Store the joint into this models joint vector
                }

                fileIn >> checkString;                    // Skip the "}"
            }
            else if ( checkString == L"mesh")
            {
                ModelSubset subset;
                int numVerts, numTris, numWeights;

                fileIn >> checkString;                    // Skip the "{"

                fileIn >> checkString;
                while ( checkString != L"}" )            // Read until '}'
                {
                    // In this lesson, for the sake of simplicity, we will assume a textures filename is givin here.
                    // Usually though, the name of a material (stored in a material library. Think back to the lesson on
                    // loading .obj files, where the material library was contained in the file .mtl) is givin. Let this
                    // be an exercise to load the material from a material library such as obj's .mtl file, instead of
                    // just the texture like we will do here.
                    if(checkString == L"shader")        // Load the texture or material
                    {                        
                        std::wstring fileNamePath;
                        fileIn >> fileNamePath;            // Get texture's filename

                        // Take spaces into account if filename or material name has a space in it
                        if(fileNamePath[fileNamePath.size()-1] != '"')
                        {
                            wchar_t checkChar;
                            bool fileNameFound = false;
                            while(!fileNameFound)
                            {
                                checkChar = fileIn.get();

                                if(checkChar == '"')
                                    fileNameFound = true;

                                fileNamePath += checkChar;                                                                    
                            }
                        }

                        // Remove the quotation marks from texture path
                        fileNamePath.erase(0, 1);
                        fileNamePath.erase(fileNamePath.size()-1, 1);

                        //check if this texture has already been loaded
                        bool alreadyLoaded = false;
                        for(int i = 0; i < texFileNameArray.size(); ++i)
                        {
                            if(fileNamePath == texFileNameArray[i])
                            {
                                alreadyLoaded = true;
                                subset.texArrayIndex = i;
                            }
                        }

                        //if the texture is not already loaded, load it now
                        if(!alreadyLoaded)
                        {
                            ID3D11ShaderResourceView* tempMeshSRV;
                            hr = CreateWICTextureFromFile(dev, devcon, fileNamePath.c_str(), NULL, &tempMeshSRV);
                            if(SUCCEEDED(hr))
                            {
                                texFileNameArray.push_back(fileNamePath.c_str());
                                subset.texArrayIndex = shaderResourceViewArray.size();
                                shaderResourceViewArray.push_back(tempMeshSRV);
                            }
                            else
                            {
                                MessageBox(0, fileNamePath.c_str(),        //display message
                                    L"Could Not Open:", MB_OK);
                                return false;
                            }
                        }    

                        std::getline(fileIn, checkString);                // Skip rest of this line
                    }
                    else if ( checkString == L"numverts")
                    {
                        fileIn >> numVerts;                                // Store number of vertices

                        std::getline(fileIn, checkString);                // Skip rest of this line

                        for(int i = 0; i < numVerts; i++)
                        {
                            Vertex tempVert;

                            fileIn >> checkString                        // Skip "vert # ("
                                >> checkString
                                >> checkString;

                            fileIn >> tempVert.texcoord.x                // Store tex coords
                                >> tempVert.texcoord.y;    

                            fileIn >> checkString;                        // Skip ")"

                            fileIn >> tempVert.StartWeight;                // Index of first weight this vert will be weighted to

                            fileIn >> tempVert.WeightCount;                // Number of weights for this vertex

                            std::getline(fileIn, checkString);            // Skip rest of this line

                            subset.vertices.push_back(tempVert);        // Push back this vertex into subsets vertex vector
                        }
                    }
                    else if ( checkString == L"numtris")
                    {
                        fileIn >> numTris;
                        subset.numTriangles = numTris;

                        std::getline(fileIn, checkString);                // Skip rest of this line

                        for(int i = 0; i < numTris; i++)                // Loop through each triangle
                        {
                            DWORD tempIndex;
                            fileIn >> checkString;                        // Skip "tri"
                            fileIn >> checkString;                        // Skip tri counter

                            for(int k = 0; k < 3; k++)                    // Store the 3 indices
                            {
                                fileIn >> tempIndex;
                                subset.indices.push_back(tempIndex);
                            }

                            std::getline(fileIn, checkString);            // Skip rest of this line
                        }
                    }
                    else if ( checkString == L"numweights")
                    {
                        fileIn >> numWeights;

                        std::getline(fileIn, checkString);                // Skip rest of this line

                        for(int i = 0; i < numWeights; i++)
                        {
                            Weight tempWeight;
                            fileIn >> checkString >> checkString;        // Skip "weight #"

                            fileIn >> tempWeight.jointID;                // Store weight's joint ID

                            fileIn >> tempWeight.bias;                    // Store weight's influence over a vertex

                            fileIn >> checkString;                        // Skip "("

                            fileIn >> tempWeight.pos.x                    // Store weight's pos in joint's local space
                                >> tempWeight.pos.z
                                >> tempWeight.pos.y;

                            std::getline(fileIn, checkString);            // Skip rest of this line

                            subset.weights.push_back(tempWeight);        // Push back tempWeight into subsets Weight array
                        }

                    }
                    else
                        std::getline(fileIn, checkString);                // Skip anything else

                    fileIn >> checkString;                                // Skip "}"
                }

                //*** find each vertex's position using the joints and weights ***//
                for ( int i = 0; i < subset.vertices.size(); ++i )
                {
                    Vertex tempVert = subset.vertices[i];
                    tempVert.pos = XMFLOAT3(0, 0, 0);    // Make sure the vertex's pos is cleared first

                    // Sum up the joints and weights information to get vertex's position
                    for ( int j = 0; j < tempVert.WeightCount; ++j )
                    {
                        Weight tempWeight = subset.weights[tempVert.StartWeight + j];
                        Joint tempJoint = MD5Model.joints[tempWeight.jointID];

                        // Convert joint orientation and weight pos to vectors for easier computation
                        // When converting a 3d vector to a quaternion, you should put 0 for "w", and
                        // When converting a quaternion to a 3d vector, you can just ignore the "w"
                        XMVECTOR tempJointOrientation = XMVectorSet(tempJoint.orientation.x, tempJoint.orientation.y, tempJoint.orientation.z, tempJoint.orientation.w);
                        XMVECTOR tempWeightPos = XMVectorSet(tempWeight.pos.x, tempWeight.pos.y, tempWeight.pos.z, 0.0f);

                        // We will need to use the conjugate of the joint orientation quaternion
                        // To get the conjugate of a quaternion, all you have to do is inverse the x, y, and z
                        XMVECTOR tempJointOrientationConjugate = XMVectorSet(-tempJoint.orientation.x, -tempJoint.orientation.y, -tempJoint.orientation.z, tempJoint.orientation.w);

                        // Calculate vertex position (in joint space, eg. rotate the point around (0,0,0)) for this weight using the joint orientation quaternion and its conjugate
                        // We can rotate a point using a quaternion with the equation "rotatedPoint = quaternion * point * quaternionConjugate"
                        XMFLOAT3 rotatedPoint;
                        XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightPos), tempJointOrientationConjugate));

                        // Now move the verices position from joint space (0,0,0) to the joints position in world space, taking the weights bias into account
                        // The weight bias is used because multiple weights might have an effect on the vertices final position. Each weight is attached to one joint.
                        tempVert.pos.x += ( tempJoint.pos.x + rotatedPoint.x ) * tempWeight.bias;
                        tempVert.pos.y += ( tempJoint.pos.y + rotatedPoint.y ) * tempWeight.bias;
                        tempVert.pos.z += ( tempJoint.pos.z + rotatedPoint.z ) * tempWeight.bias;

                        // Basically what has happened above, is we have taken the weights position relative to the joints position
                        // we then rotate the weights position (so that the weight is actually being rotated around (0, 0, 0) in world space) using
                        // the quaternion describing the joints rotation. We have stored this rotated point in rotatedPoint, which we then add to
                        // the joints position (because we rotated the weight's position around (0,0,0) in world space, and now need to translate it
                        // so that it appears to have been rotated around the joints position). Finally we multiply the answer with the weights bias,
                        // or how much control the weight has over the final vertices position. All weight's bias effecting a single vertex's position
                        // must add up to 1.
                    }

                    subset.positions.push_back(tempVert.pos);            // Store the vertices position in the position vector instead of straight into the vertex vector
                    // since we can use the positions vector for certain things like collision detection or picking
                    // without having to work with the entire vertex structure.
                }

                // Put the positions into the vertices for this subset
                for(int i = 0; i < subset.vertices.size(); i++)
                {
                    subset.vertices[i].pos = subset.positions[i];
                }

                //*** Calculate vertex normals using normal averaging ***///
                std::vector<XMFLOAT3> tempNormal;

                //normalized and unnormalized normals
                XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

                //Used to get vectors (sides) from the position of the verts
                float vecX, vecY, vecZ;

                //Two edges of our triangle
                XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
                XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

                //Compute face normals
                for(int i = 0; i < subset.numTriangles; ++i)
                {
                    //Get the vector describing one edge of our triangle (edge 0,2)
                    vecX = subset.vertices[subset.indices[(i*3)]].pos.x - subset.vertices[subset.indices[(i*3)+2]].pos.x;
                    vecY = subset.vertices[subset.indices[(i*3)]].pos.y - subset.vertices[subset.indices[(i*3)+2]].pos.y;
                    vecZ = subset.vertices[subset.indices[(i*3)]].pos.z - subset.vertices[subset.indices[(i*3)+2]].pos.z;        
                    edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our first edge

                    //Get the vector describing another edge of our triangle (edge 2,1)
                    vecX = subset.vertices[subset.indices[(i*3)+2]].pos.x - subset.vertices[subset.indices[(i*3)+1]].pos.x;
                    vecY = subset.vertices[subset.indices[(i*3)+2]].pos.y - subset.vertices[subset.indices[(i*3)+1]].pos.y;
                    vecZ = subset.vertices[subset.indices[(i*3)+2]].pos.z - subset.vertices[subset.indices[(i*3)+1]].pos.z;        
                    edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our second edge

                    //Cross multiply the two edge vectors to get the un-normalized face normal
                    XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));

                    tempNormal.push_back(unnormalized);
                }

                //Compute vertex normals (normal Averaging)
                XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
                int facesUsing = 0;
                float tX, tY, tZ;    //temp axis variables

                //Go through each vertex
                for(int i = 0; i < subset.vertices.size(); ++i)
                {
                    //Check which triangles use this vertex
                    for(int j = 0; j < subset.numTriangles; ++j)
                    {
                        if(subset.indices[j*3] == i ||
                            subset.indices[(j*3)+1] == i ||
                            subset.indices[(j*3)+2] == i)
                        {
                            tX = XMVectorGetX(normalSum) + tempNormal[j].x;
                            tY = XMVectorGetY(normalSum) + tempNormal[j].y;
                            tZ = XMVectorGetZ(normalSum) + tempNormal[j].z;

                            normalSum = XMVectorSet(tX, tY, tZ, 0.0f);    //If a face is using the vertex, add the unormalized face normal to the normalSum

                            facesUsing++;
                        }
                    }

                    //Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
                    // normalSum = normalSum / facesUsing;
                    normalSum = XMVectorScale(normalSum, 1/float(facesUsing));

                    //Normalize the normalSum vector
                    normalSum = XMVector3Normalize(normalSum);

                    //Store the normal and tangent in our current vertex
                    subset.vertices[i].normal.x = -XMVectorGetX(normalSum);
                    subset.vertices[i].normal.y = -XMVectorGetY(normalSum);
                    subset.vertices[i].normal.z = -XMVectorGetZ(normalSum);

                    //Clear normalSum, facesUsing for next vertex
                    normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
                    facesUsing = 0;
                }

                // Create index buffer
                D3D11_BUFFER_DESC indexBufferDesc;
                ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

                indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
                indexBufferDesc.ByteWidth = sizeof(DWORD) * subset.numTriangles * 3;
                indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
                indexBufferDesc.CPUAccessFlags = 0;
                indexBufferDesc.MiscFlags = 0;

                D3D11_SUBRESOURCE_DATA iinitData;

                iinitData.pSysMem = &subset.indices[0];
                dev->CreateBuffer(&indexBufferDesc, &iinitData, &subset.indexBuff);

                //Create Vertex Buffer
                D3D11_BUFFER_DESC vertexBufferDesc;
                ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

                vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                            // We will be updating this buffer, so we must set as dynamic
                vertexBufferDesc.ByteWidth = sizeof( Vertex ) * subset.vertices.size();
                vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                // Give CPU power to write to buffer
                vertexBufferDesc.MiscFlags = 0;

                D3D11_SUBRESOURCE_DATA vertexBufferData; 

                ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
                vertexBufferData.pSysMem = &subset.vertices[0];
                dev->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &subset.vertBuff);

                // Push back the temp subset into the models subset vector
                MD5Model.subsets.push_back(subset);
            }
        }
    }
    else
    {
        swapchain->SetFullscreenState(false, NULL);    // Make sure we are out of fullscreen

        // create message
        std::wstring message = L"Could not open: ";
        message += filename;

        MessageBox(0, message.c_str(),    // display message
            L"Error", MB_OK);

        return false;
    }

    return true;
}

bool LoadMD5Anim(std::wstring filename,    Model3D& MD5Model)
{    
    ModelAnimation tempAnim;                        // Temp animation to later store in our model's animation array

    std::wifstream fileIn (filename.c_str());        // Open file

    std::wstring checkString;                        // Stores the next string from our file

    if(fileIn)                                        // Check if the file was opened
    {
        while(fileIn)                                // Loop until the end of the file is reached
        {    
            fileIn >> checkString;                    // Get next string from file

            if ( checkString == L"MD5Version" )        // Get MD5 version (this function supports version 10)
            {
                fileIn >> checkString;
                /*MessageBox(0, checkString.c_str(),    //display message
                L"MD5Version", MB_OK);*/
            }
            else if ( checkString == L"commandline" )
            {
                std::getline(fileIn, checkString);    // Ignore the rest of this line
            }
            else if ( checkString == L"numFrames" )
            {
                fileIn >> tempAnim.numFrames;                // Store number of frames in this animation
            }
            else if ( checkString == L"numJoints" )
            {
                fileIn >> tempAnim.numJoints;                // Store number of joints (must match .md5mesh)
            }
            else if ( checkString == L"frameRate" )
            {
                fileIn >> tempAnim.frameRate;                // Store animation's frame rate (frames per second)
            }
            else if ( checkString == L"numAnimatedComponents" )
            {
                fileIn >> tempAnim.numAnimatedComponents;    // Number of components in each frame section
            }
            else if ( checkString == L"hierarchy" )
            {
                fileIn >> checkString;                // Skip opening bracket "{"

                for(int i = 0; i < tempAnim.numJoints; i++)    // Load in each joint
                {
                    AnimJointInfo tempJoint;

                    fileIn >> tempJoint.name;        // Get joints name
                    // Sometimes the names might contain spaces. If that is the case, we need to continue
                    // to read the name until we get to the closing " (quotation marks)
                    if(tempJoint.name[tempJoint.name.size()-1] != '"')
                    {
                        wchar_t checkChar;
                        bool jointNameFound = false;
                        while(!jointNameFound)
                        {
                            checkChar = fileIn.get();

                            if(checkChar == '"')
                                jointNameFound = true;        

                            tempJoint.name += checkChar;                                                            
                        }
                    }
                    // Remove the quotation marks from joints name
                    tempJoint.name.erase(0, 1);
                    tempJoint.name.erase(tempJoint.name.size()-1, 1);

                    fileIn >> tempJoint.parentID;            // Get joints parent ID
                    fileIn >> tempJoint.flags;                // Get flags
                    fileIn >> tempJoint.startIndex;            // Get joints start index

                    // Make sure the joint exists in the model, and the parent ID's match up
                    // because the bind pose (md5mesh) joint hierarchy and the animations (md5anim)
                    // joint hierarchy must match up
                    bool jointMatchFound = false;
                    for(int k = 0; k < MD5Model.numJoints; k++)
                    {
                        if(MD5Model.joints[k].name == tempJoint.name)
                        {
                            if(MD5Model.joints[k].parentID == tempJoint.parentID)
                            {
                                jointMatchFound = true;
                                tempAnim.jointInfo.push_back(tempJoint);
                            }
                        }
                    }
                    if(!jointMatchFound){                  // If the skeleton system does not match up, return false
                        printf("\nMismatch animation and model\n");
                        std::wcout << "Could not find " << tempJoint.name << std::endl;
                        return false;                        // You might want to add an error message here
                    }
                    std::getline(fileIn, checkString);        // Skip rest of this line
                }
            }
            else if ( checkString == L"bounds" )            // Load in the AABB for each animation
            {
                fileIn >> checkString;                        // Skip opening bracket "{"

                for(int i = 0; i < tempAnim.numFrames; i++)
                {
                    BoundingBox tempBB;

                    fileIn >> checkString;                    // Skip "("
                    fileIn >> tempBB.min.x >> tempBB.min.z >> tempBB.min.y;
                    fileIn >> checkString >> checkString;    // Skip ") ("
                    fileIn >> tempBB.max.x >> tempBB.max.z >> tempBB.max.y;
                    fileIn >> checkString;                    // Skip ")"

                    tempAnim.frameBounds.push_back(tempBB);
                }
            }            
            else if ( checkString == L"baseframe" )            // This is the default position for the animation
            {                                                // All frames will build their skeletons off this
                fileIn >> checkString;                        // Skip opening bracket "{"

                for(int i = 0; i < tempAnim.numJoints; i++)
                {
                    Joint tempBFJ;

                    fileIn >> checkString;                        // Skip "("
                    fileIn >> tempBFJ.pos.x >> tempBFJ.pos.z >> tempBFJ.pos.y;
                    fileIn >> checkString >> checkString;        // Skip ") ("
                    fileIn >> tempBFJ.orientation.x >> tempBFJ.orientation.z >> tempBFJ.orientation.y;
                    fileIn >> checkString;                        // Skip ")"

                    tempAnim.baseFrameJoints.push_back(tempBFJ);
                }
            }
            else if ( checkString == L"frame" )        // Load in each frames skeleton (the parts of each joint that changed from the base frame)
            {
                FrameData tempFrame;

                fileIn >> tempFrame.frameID;        // Get the frame ID

                fileIn >> checkString;                // Skip opening bracket "{"

                for(int i = 0; i < tempAnim.numAnimatedComponents; i++)
                {
                    float tempData;
                    fileIn >> tempData;                // Get the data

                    tempFrame.frameData.push_back(tempData);
                }

                tempAnim.frameData.push_back(tempFrame);

                ///*** build the frame skeleton ***///
                std::vector<Joint> tempSkeleton;

                for(int i = 0; i < tempAnim.jointInfo.size(); i++)
                {
                    int k = 0;                        // Keep track of position in frameData array

                    // Start the frames joint with the base frame's joint
                    Joint tempFrameJoint = tempAnim.baseFrameJoints[i];

                    tempFrameJoint.parentID = tempAnim.jointInfo[i].parentID;

                    // Notice how I have been flipping y and z. this is because some modeling programs such as
                    // 3ds max (which is what I use) use a right handed coordinate system. Because of this, we
                    // need to flip the y and z axes. If your having problems loading some models, it's possible
                    // the model was created in a left hand coordinate system. in that case, just reflip all the
                    // y and z axes in our md5 mesh and anim loader.
                    if(tempAnim.jointInfo[i].flags & 1)        // pos.x    ( 000001 )
                        tempFrameJoint.pos.x = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

                    if(tempAnim.jointInfo[i].flags & 2)        // pos.y    ( 000010 )
                        tempFrameJoint.pos.z = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

                    if(tempAnim.jointInfo[i].flags & 4)        // pos.z    ( 000100 )
                        tempFrameJoint.pos.y = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

                    if(tempAnim.jointInfo[i].flags & 8)        // orientation.x    ( 001000 )
                        tempFrameJoint.orientation.x = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

                    if(tempAnim.jointInfo[i].flags & 16)    // orientation.y    ( 010000 )
                        tempFrameJoint.orientation.z = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];

                    if(tempAnim.jointInfo[i].flags & 32)    // orientation.z    ( 100000 )
                        tempFrameJoint.orientation.y = tempFrame.frameData[tempAnim.jointInfo[i].startIndex + k++];


                    // Compute the quaternions w
                    float t = 1.0f - ( tempFrameJoint.orientation.x * tempFrameJoint.orientation.x )
                        - ( tempFrameJoint.orientation.y * tempFrameJoint.orientation.y )
                        - ( tempFrameJoint.orientation.z * tempFrameJoint.orientation.z );
                    if ( t < 0.0f )
                    {
                        tempFrameJoint.orientation.w = 0.0f;
                    }
                    else
                    {
                        tempFrameJoint.orientation.w = -sqrtf(t);
                    }

                    // Now, if the upper arm of your skeleton moves, you need to also move the lower part of your arm, and then the hands, and then finally the fingers (possibly weapon or tool too)
                    // This is where joint hierarchy comes in. We start at the top of the hierarchy, and move down to each joints child, rotating and translating them based on their parents rotation
                    // and translation. We can assume that by the time we get to the child, the parent has already been rotated and transformed based of it's parent. We can assume this because
                    // the child should never come before the parent in the files we loaded in.
                    if(tempFrameJoint.parentID >= 0)
                    {
                        Joint parentJoint = tempSkeleton[tempFrameJoint.parentID];

                        // Turn the XMFLOAT3 and 4's into vectors for easier computation
                        XMVECTOR parentJointOrientation = XMVectorSet(parentJoint.orientation.x, parentJoint.orientation.y, parentJoint.orientation.z, parentJoint.orientation.w);
                        XMVECTOR tempJointPos = XMVectorSet(tempFrameJoint.pos.x, tempFrameJoint.pos.y, tempFrameJoint.pos.z, 0.0f);
                        XMVECTOR parentOrientationConjugate = XMVectorSet(-parentJoint.orientation.x, -parentJoint.orientation.y, -parentJoint.orientation.z, parentJoint.orientation.w);

                        // Calculate current joints position relative to its parents position
                        XMFLOAT3 rotatedPos;
                        XMStoreFloat3(&rotatedPos, XMQuaternionMultiply(XMQuaternionMultiply(parentJointOrientation, tempJointPos), parentOrientationConjugate));

                        // Translate the joint to model space by adding the parent joint's pos to it
                        tempFrameJoint.pos.x = rotatedPos.x + parentJoint.pos.x;
                        tempFrameJoint.pos.y = rotatedPos.y + parentJoint.pos.y;
                        tempFrameJoint.pos.z = rotatedPos.z + parentJoint.pos.z;

                        // Currently the joint is oriented in its parent joints space, we now need to orient it in
                        // model space by multiplying the two orientations together (parentOrientation * childOrientation) <- In that order
                        XMVECTOR tempJointOrient = XMVectorSet(tempFrameJoint.orientation.x, tempFrameJoint.orientation.y, tempFrameJoint.orientation.z, tempFrameJoint.orientation.w);
                        tempJointOrient = XMQuaternionMultiply(parentJointOrientation, tempJointOrient);

                        // Normalize the orienation quaternion
                        tempJointOrient = XMQuaternionNormalize(tempJointOrient);

                        XMStoreFloat4(&tempFrameJoint.orientation, tempJointOrient);
                    }

                    // Store the joint into our temporary frame skeleton
                    tempSkeleton.push_back(tempFrameJoint);
                }

                // Push back our newly created frame skeleton into the animation's frameSkeleton array
                tempAnim.frameSkeleton.push_back(tempSkeleton);

                fileIn >> checkString;                // Skip closing bracket "}"
            }
        }

        // Calculate and store some usefull animation data
        tempAnim.frameTime = 1.0f / tempAnim.frameRate;                        // Set the time per frame
        tempAnim.totalAnimTime = tempAnim.numFrames * tempAnim.frameTime;    // Set the total time the animation takes
        tempAnim.currAnimTime = 0.0f;                                        // Set the current time to zero

        MD5Model.animations.push_back(tempAnim);                            // Push back the animation into our model object
    }
    else    // If the file was not loaded
    {
        swapchain->SetFullscreenState(false, NULL);    // Make sure we are out of fullscreen
        printf("HEHEHEHE\n");
        // create message
        std::wstring message = L"Could not open: ";
        message += filename;

        MessageBox(0, message.c_str(),                // display message
            L"Error", MB_OK);

        return false;
    }
    return true;
}

void UpdateMD5Model(Model3D& MD5Model, float deltaTime, int animation)
{
    MD5Model.animations[animation].currAnimTime += deltaTime;            // Update the current animation time

    if(MD5Model.animations[animation].currAnimTime > MD5Model.animations[animation].totalAnimTime)
        MD5Model.animations[animation].currAnimTime = 0.0f;

    // Which frame are we on
    float currentFrame = MD5Model.animations[animation].currAnimTime * MD5Model.animations[animation].frameRate;    
    int frame0 = floorf( currentFrame );
    int frame1 = frame0 + 1;

    // Make sure we don't go over the number of frames    
    if(frame0 == MD5Model.animations[animation].numFrames-1)
        frame1 = 0;

    float interpolation = currentFrame - frame0;    // Get the remainder (in time) between frame0 and frame1 to use as interpolation factor

    std::vector<Joint> interpolatedSkeleton;        // Create a frame skeleton to store the interpolated skeletons in

    // Compute the interpolated skeleton
    for( int i = 0; i < MD5Model.animations[animation].numJoints; i++)
    {
        Joint tempJoint;
        Joint joint0 = MD5Model.animations[animation].frameSkeleton[frame0][i];        // Get the i'th joint of frame0's skeleton
        Joint joint1 = MD5Model.animations[animation].frameSkeleton[frame1][i];        // Get the i'th joint of frame1's skeleton

        tempJoint.parentID = joint0.parentID;                                            // Set the tempJoints parent id

        // Turn the two quaternions into XMVECTORs for easy computations
        XMVECTOR joint0Orient = XMVectorSet(joint0.orientation.x, joint0.orientation.y, joint0.orientation.z, joint0.orientation.w);
        XMVECTOR joint1Orient = XMVectorSet(joint1.orientation.x, joint1.orientation.y, joint1.orientation.z, joint1.orientation.w);

        // Interpolate positions
        tempJoint.pos.x = joint0.pos.x + (interpolation * (joint1.pos.x - joint0.pos.x));
        tempJoint.pos.y = joint0.pos.y + (interpolation * (joint1.pos.y - joint0.pos.y));
        tempJoint.pos.z = joint0.pos.z + (interpolation * (joint1.pos.z - joint0.pos.z));

        // Interpolate orientations using spherical interpolation (Slerp)
        XMStoreFloat4(&tempJoint.orientation, XMQuaternionSlerp(joint0Orient, joint1Orient, interpolation));

        interpolatedSkeleton.push_back(tempJoint);        // Push the joint back into our interpolated skeleton
    }

    for ( int k = 0; k < MD5Model.numSubsets; k++)
    {
        for ( int i = 0; i < MD5Model.subsets[k].vertices.size(); ++i )
        {
            Vertex tempVert = MD5Model.subsets[k].vertices[i];
            tempVert.pos = XMFLOAT3(0, 0, 0);    // Make sure the vertex's pos is cleared first
            tempVert.normal = XMFLOAT3(0,0,0);    // Clear vertices normal

            // Sum up the joints and weights information to get vertex's position and normal
            for ( int j = 0; j < tempVert.WeightCount; ++j )
            {
                Weight tempWeight = MD5Model.subsets[k].weights[tempVert.StartWeight + j];
                Joint tempJoint = interpolatedSkeleton[tempWeight.jointID];

                // Convert joint orientation and weight pos to vectors for easier computation
                XMVECTOR tempJointOrientation = XMVectorSet(tempJoint.orientation.x, tempJoint.orientation.y, tempJoint.orientation.z, tempJoint.orientation.w);
                XMVECTOR tempWeightPos = XMVectorSet(tempWeight.pos.x, tempWeight.pos.y, tempWeight.pos.z, 0.0f);

                // We will need to use the conjugate of the joint orientation quaternion
                XMVECTOR tempJointOrientationConjugate = XMQuaternionInverse(tempJointOrientation);

                // Calculate vertex position (in joint space, eg. rotate the point around (0,0,0)) for this weight using the joint orientation quaternion and its conjugate
                // We can rotate a point using a quaternion with the equation "rotatedPoint = quaternion * point * quaternionConjugate"
                XMFLOAT3 rotatedPoint;
                XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightPos), tempJointOrientationConjugate));

                // Now move the verices position from joint space (0,0,0) to the joints position in world space, taking the weights bias into account
                tempVert.pos.x += ( tempJoint.pos.x + rotatedPoint.x ) * tempWeight.bias;
                tempVert.pos.y += ( tempJoint.pos.y + rotatedPoint.y ) * tempWeight.bias;
                tempVert.pos.z += ( tempJoint.pos.z + rotatedPoint.z ) * tempWeight.bias;

                // Compute the normals for this frames skeleton using the weight normals from before
                // We can comput the normals the same way we compute the vertices position, only we don't have to translate them (just rotate)
                XMVECTOR tempWeightNormal = XMVectorSet(tempWeight.normal.x, tempWeight.normal.y, tempWeight.normal.z, 0.0f);

                // Rotate the normal
                XMStoreFloat3(&rotatedPoint, XMQuaternionMultiply(XMQuaternionMultiply(tempJointOrientation, tempWeightNormal), tempJointOrientationConjugate));

                // Add to vertices normal and ake weight bias into account
                tempVert.normal.x -= rotatedPoint.x * tempWeight.bias;
                tempVert.normal.y -= rotatedPoint.y * tempWeight.bias;
                tempVert.normal.z -= rotatedPoint.z * tempWeight.bias;
            }

            MD5Model.subsets[k].positions[i] = tempVert.pos;                // Store the vertices position in the position vector instead of straight into the vertex vector
            MD5Model.subsets[k].vertices[i].normal = tempVert.normal;        // Store the vertices normal
            XMStoreFloat3(&MD5Model.subsets[k].vertices[i].normal, XMVector3Normalize(XMLoadFloat3(&MD5Model.subsets[k].vertices[i].normal)));
        }

        // Put the positions into the vertices for this subset
        for(int i = 0; i < MD5Model.subsets[k].vertices.size(); i++)
        {
            MD5Model.subsets[k].vertices[i].pos = MD5Model.subsets[k].positions[i];
        }

        // Update the subsets vertex buffer
        // First lock the buffer
        D3D11_MAPPED_SUBRESOURCE mappedVertBuff;
        devcon->Map(MD5Model.subsets[k].vertBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertBuff);

        // Copy the data into the vertex buffer.
        memcpy(mappedVertBuff.pData, &MD5Model.subsets[k].vertices[0], (sizeof(Vertex) * MD5Model.subsets[k].vertices.size()));

        devcon->Unmap(MD5Model.subsets[k].vertBuff, 0);

        // The line below is another way to update a buffer. You will use this when you want to update a buffer less
        // than once per frame, since the GPU reads will be faster (the buffer was created as a DEFAULT buffer instead
        // of a DYNAMIC buffer), and the CPU writes will be slower. You can try both methods to find out which one is faster
        // for you. if you want to use the line below, you will have to create the buffer with D3D11_USAGE_DEFAULT instead
        // of D3D11_USAGE_DYNAMIC
        //d3d11DevCon->UpdateSubresource( MD5Model.subsets[k].vertBuff, 0, NULL, &MD5Model.subsets[k].vertices[0], 0, 0 );
    }
}
