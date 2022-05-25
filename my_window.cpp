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

struct HeightMapInfo{        // Heightmap structure
    int terrainWidth;        // Width of heightmap
    int terrainHeight;        // Height (Length) of heightmap
    XMFLOAT3 *heightMap;    // Array to store terrain's vertex positions
};

struct CollisionPacket{
    // Information about ellipsoid (in world space)
    XMVECTOR ellipsoidSpace;
    XMVECTOR w_Position;
    XMVECTOR w_Velocity;

    // Information about ellipsoid (in ellipsoid space)
    XMVECTOR e_Position;
    XMVECTOR e_Velocity;
    XMVECTOR e_normalizedVelocity;

    // Collision Information
    bool foundCollision;
    float nearestDistance;
    XMVECTOR intersectionPoint;
    int collisionRecursionDepth;
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

XMMATRIX smilesWorld;
Model3D NewMD5Model;

//----------------------------
//Heightmap
int NumFaces = 0;
int NumVertices = 0;
//----------------------------
//Sliding Camera Collision
const float unitsPerMeter = 100.0f;

XMVECTOR gravity = XMVectorSet(0.0f, -0.2f, 0.0f, 0.0f);

std::vector<XMFLOAT3> collidableGeometryPositions;
std::vector<DWORD> collidableGeometryIndices;
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
bool HeightMapLoad(const char* filename, HeightMapInfo &hminfo);
//This is my function that just contains all the random stuff it wants to do
void setupHeightMap(HeightMapInfo hmInfo, std::vector<DWORD>& ind, std::vector<Vertex>& vert);
// Collision Detection and Response Function Prototypes
XMVECTOR CollisionSlide(CollisionPacket& cP,            // Pointer to a CollisionPacket object (expects ellipsoidSpace, w_Position and w_Velocity to be filled)
    std::vector<XMFLOAT3>& vertPos,                        // An array holding the polygon soup vertex positions
    std::vector<DWORD>& indices);                        // An array holding the polygon soup indices (triangles)

XMVECTOR CollideWithWorld(CollisionPacket& cP,            // Same arguments as the above function
    std::vector<XMFLOAT3>& vertPos,
    std::vector<DWORD>& indices);

bool SphereCollidingWithTriangle(CollisionPacket& cP,    // Pointer to a CollisionPacket object    
    XMVECTOR &p0,                                        // First vertex position of triangle
    XMVECTOR &p1,                                        // Second vertex position of triangle
    XMVECTOR &p2,                                        // Third vertex position of triangle 
    XMVECTOR &triNormal);                                // Triangle's Normal

// Checks if a point (inside the triangle's plane) is inside the triangle
bool checkPointInTriangle(const XMVECTOR& point, const XMVECTOR& triV1,const XMVECTOR& triV2, const XMVECTOR& triV3);

// Solves the quadratic eqation, and returns the lowest root if equation is solvable, returns false if not solvable
bool getLowestRoot(float a, float b, float c, float maxR, float* root);



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

    //Update the light to follow the camera and point where the camera points, essentially making a flashlight
    light.pos.x = XMVectorGetX(camPosition);
    light.pos.y = XMVectorGetY(camPosition);
    light.pos.z = XMVectorGetZ(camPosition);

    light.dir.x = XMVectorGetX(camTarget) - light.pos.x;
    light.dir.y = XMVectorGetY(camTarget) - light.pos.y;
    light.dir.z = XMVectorGetZ(camTarget) - light.pos.z;

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
    scd.Windowed = true;                                    // windowed/full-screen mode
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

    //Set the cubes index buffer
    devcon->IASetIndexBuffer( squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    devcon->IASetVertexBuffers( 0, 1, &squareVertBuffer, &stride, &offset );

    //Set the WVP matrix and send it to the constant buffer in effect file
    WVP = groundWorld * camView * camProjection;
    cbPerObj.WVP = XMMatrixTranspose(WVP);    
    cbPerObj.World = XMMatrixTranspose(groundWorld);    
    devcon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
    devcon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
    devcon->PSSetShaderResources( 0, 1, &CubesTexture );
    devcon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

    devcon->RSSetState(CCWcullMode);
    devcon->DrawIndexed( NumFaces * 3, 0, 0 );

    //draw bottle's nontransparent subsets
    for(int j = 0; j < numBottles; j++)
    {
        if(bottleHit[j] == 0)
        {
            for(int i = 0; i < bottleSubsets; ++i)
            {
                //Set the grounds index buffer
                devcon->IASetIndexBuffer( bottleIndexBuff, DXGI_FORMAT_R32_UINT, 0);
                //Set the grounds vertex buffer
                devcon->IASetVertexBuffers( 0, 1, &bottleVertBuff, &stride, &offset );

                //Set the WVP matrix and send it to the constant buffer in effect file
                WVP = bottleWorld[j] * camView * camProjection;
                cbPerObj.WVP = XMMatrixTranspose(WVP);    
                cbPerObj.World = XMMatrixTranspose(bottleWorld[j]);    
                cbPerObj.difColor = bottlematerial[bottleSubsetTexture[i]].difColor;
                cbPerObj.hasTexture = bottlematerial[bottleSubsetTexture[i]].hasTexture;
                cbPerObj.hasNormMap = bottlematerial[bottleSubsetTexture[i]].hasNormMap;
                devcon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
                devcon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
                devcon->PSSetConstantBuffers( 1, 1, &cbPerObjectBuffer );
                if(bottlematerial[bottleSubsetTexture[i]].hasTexture)
                    devcon->PSSetShaderResources( 0, 1, &meshSRV[bottlematerial[bottleSubsetTexture[i]].texArrayIndex] );
                if(bottlematerial[bottleSubsetTexture[i]].hasNormMap)
                    devcon->PSSetShaderResources( 1, 1, &meshSRV[bottlematerial[bottleSubsetTexture[i]].normMapTexArrayIndex] );
                devcon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

                devcon->RSSetState(NoCullMode);
                int indexStart = bottleSubsetIndexStart[i];
                int indexDrawAmount =  bottleSubsetIndexStart[i+1] - bottleSubsetIndexStart[i];
                if(!bottlematerial[bottleSubsetTexture[i]].transparent)
                    devcon->DrawIndexed( indexDrawAmount, indexStart, 0 );
            }
        }
    }

        if(bottleFlying)
    {
        for(int i = 0; i < bottleSubsets; ++i)
        {
            // Set the grounds index buffer
            devcon->IASetIndexBuffer( bottleIndexBuff, DXGI_FORMAT_R32_UINT, 0);
            // Set the grounds vertex buffer
            devcon->IASetVertexBuffers( 0, 1, &bottleVertBuff, &stride, &offset );

            // Set the WVP matrix and send it to the constant buffer in effect file
            WVP = thrownBottleWorld * camView * camProjection;
            cbPerObj.WVP = XMMatrixTranspose(WVP);    
            cbPerObj.World = XMMatrixTranspose(thrownBottleWorld);    
            cbPerObj.difColor = bottlematerial[bottleSubsetTexture[i]].difColor;
            cbPerObj.hasTexture = bottlematerial[bottleSubsetTexture[i]].hasTexture;
            cbPerObj.hasNormMap = bottlematerial[bottleSubsetTexture[i]].hasNormMap;
            devcon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
            devcon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
            devcon->PSSetConstantBuffers( 1, 1, &cbPerObjectBuffer );
            if(bottlematerial[bottleSubsetTexture[i]].hasTexture)
                devcon->PSSetShaderResources( 0, 1, &meshSRV[bottlematerial[bottleSubsetTexture[i]].texArrayIndex] );
            if(material[bottleSubsetTexture[i]].hasNormMap)
                devcon->PSSetShaderResources( 1, 1, &meshSRV[bottlematerial[bottleSubsetTexture[i]].normMapTexArrayIndex] );
            devcon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

            devcon->RSSetState(NoCullMode);
            int indexStart = bottleSubsetIndexStart[i];
            int indexDrawAmount =  bottleSubsetIndexStart[i+1] - bottleSubsetIndexStart[i];
            if(!bottlematerial[bottleSubsetTexture[i]].transparent)
                devcon->DrawIndexed( indexDrawAmount, indexStart, 0 );
        }
    }

    ///***Draw MD5 Model***///
    for(int i = 0; i < NewMD5Model.numSubsets; i ++)
    {
        //Set the grounds index buffer
        devcon->IASetIndexBuffer( NewMD5Model.subsets[i].indexBuff, DXGI_FORMAT_R32_UINT, 0);
        //Set the grounds vertex buffer
        devcon->IASetVertexBuffers( 0, 1, &NewMD5Model.subsets[i].vertBuff, &stride, &offset );

        //Set the WVP matrix and send it to the constant buffer in effect file
        WVP = smilesWorld * camView * camProjection;
        cbPerObj.WVP = XMMatrixTranspose(WVP);    
        cbPerObj.World = XMMatrixTranspose(smilesWorld);    
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
    // light.dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
    // light.ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    // light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    //Configure the Spotlight
    light.pos = XMFLOAT3(0.0f, 1.0f, 0.0f);
    light.dir = XMFLOAT3(0.0f, 0.0f, 1.0f);
    light.range = 1000.0f;
    light.cone = 20.0f;
    light.att = XMFLOAT3(0.4f, 0.02f, 0.0f);
    light.ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    //Heightmap stuff
    HeightMapInfo hmInfo;
    std::string heightm = "resc/heightmap.bmp";
    HeightMapLoad(heightm.c_str(), hmInfo);        // Load the heightmap and store it into hmInfo
    std::vector<DWORD> indices;
    std::vector<Vertex> v;
    setupHeightMap(hmInfo, indices, v);

    printf("C");

    //Square Index Buffer
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * NumFaces * 3;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = &indices[0];
    dev->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

    devcon->IASetIndexBuffer( squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    printf("R");

    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof( Vertex ) * NumVertices;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    printf("A");

    D3D11_SUBRESOURCE_DATA vertexBufferData; 

    ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
    vertexBufferData.pSysMem = &v[0];
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

   //Setup camera
    camPosition = XMVectorSet( 0.0f, 80.0f, 0.0f, 0.0f );
    camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
    camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

    camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );

    camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, (float)SCREEN_WIDTH/SCREEN_HEIGHT, 1.0f, 1000.0f);

    //Load the texture
    // D3DX11CreateShaderResourceViewFromFile( dev, L"braynzar.jpg", NULL, NULL, &CubesTexture, NULL );
    // LoadFromWICFile(L"resc/monkey.jpg",NULL, );
    CreateWICTextureFromFile(dev, devcon, L"resc/grass.jpg", NULL, &CubesTexture);
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
    smilesWorld = Scale * Translation;

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

    //Polygon Soup for Terrain
        // Since our terrain will not be transformed throughout our scene, we will set the our groundWorlds
    // world matrix here so that when we put the terrains positions in the "polygon soup", they will
    // already be transformed to world space
    groundWorld = XMMatrixIdentity();
    Scale = XMMatrixScaling( 10.0f, 10.0f, 10.0f );
    Translation = XMMatrixTranslation( -520.0f, -10.0f, -1020.0f );
    groundWorld = Scale * Translation;

    // Store the terrains vertex positions and indices in the
    // polygon soup that we will check for collisions with
    // We can store ALL static (non-changing) geometry in here that we want to check for collisions with
    int vertexOffset = collidableGeometryPositions.size();    // Vertex offset (each "mesh" will be added to the end of the positions array)

    // Temp arrays because we need to store the geometry in world space
    XMVECTOR tempVertexPosVec;
    XMFLOAT3 tempVertF3;

    // Push back vertex positions to the polygon soup
    for(int i = 0; i < v.size(); i++)
    {
        tempVertexPosVec = XMLoadFloat3(&v[i].pos);
        tempVertexPosVec = XMVector3TransformCoord(tempVertexPosVec, groundWorld);
        XMStoreFloat3(&tempVertF3, tempVertexPosVec);
        collidableGeometryPositions.push_back(tempVertF3);
    }

    // Push back indices for polygon soup. We need to make sure we are
    // pushing back the indices "on top" of the previous pushed back
    // objects vertex positions, hence "+ vertexOffset" (This is the
    // first object we are putting in here, so it really doesn't
    // matter right now, but I just wanted to show you how to do it
    for(int i = 0; i < indices.size(); i++)
    {
        collidableGeometryIndices.push_back(indices[i] + vertexOffset);
    }
    std::reverse(collidableGeometryIndices.begin(), collidableGeometryIndices.end());
}

//This is my function that just contains all the random stuff it wants to do
void setupHeightMap(HeightMapInfo hmInfo, std::vector<DWORD>& ind, std::vector<Vertex>& vert) {
    int cols = hmInfo.terrainWidth;
    int rows = hmInfo.terrainHeight;

    //Create the grid
    NumVertices = rows * cols;
    NumFaces  = (rows-1)*(cols-1)*2;

    std::vector<Vertex> v(NumVertices);
    // v = std::vector<Vertex>(NumVertices);

    for(DWORD i = 0; i < rows; ++i)
    {
        for(DWORD j = 0; j < cols; ++j)
        {
            v[i*cols+j].pos = hmInfo.heightMap[i*cols+j];
            v[i*cols+j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
        }
    }

    std::vector<DWORD> indices(NumFaces * 3);
    // indices = std::vector<DWORD>(NumFaces * 3);

    int k = 0;
    int texUIndex = 0;
    int texVIndex = 0;
    for(DWORD i = 0; i < rows-1; i++)
    {
        for(DWORD j = 0; j < cols-1; j++)
        {
            indices[k]   = i*cols+j;        // Bottom left of quad
            v[i*cols+j].texcoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 1.0f);

            indices[k+1] = i*cols+j+1;        // Bottom right of quad
            v[i*cols+j+1].texcoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

            indices[k+2] = (i+1)*cols+j;    // Top left of quad
            v[(i+1)*cols+j].texcoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);


            indices[k+3] = (i+1)*cols+j;    // Top left of quad
            v[(i+1)*cols+j].texcoord = XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

            indices[k+4] = i*cols+j+1;        // Bottom right of quad
            v[i*cols+j+1].texcoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

            indices[k+5] = (i+1)*cols+j+1;    // Top right of quad
            v[(i+1)*cols+j+1].texcoord = XMFLOAT2(texUIndex + 1.0f, texVIndex + 0.0f);

            k += 6; // next quad

            texUIndex++;
        }
        texUIndex = 0;
        texVIndex++;
    }

    //////////////////////Compute Normals///////////////////////////
    //Now we will compute the normals for each vertex using normal averaging
    std::vector<XMFLOAT3> tempNormal;

    //normalized and unnormalized normals
    XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

    //Used to get vectors (sides) from the position of the verts
    float vecX, vecY, vecZ;

    //Two edges of our triangle
    XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    //Compute face normals
    for(int i = 0; i < NumFaces; ++i)
    {
        //Get the vector describing one edge of our triangle (edge 0,2)
        vecX = v[indices[(i*3)]].pos.x - v[indices[(i*3)+2]].pos.x;
        vecY = v[indices[(i*3)]].pos.y - v[indices[(i*3)+2]].pos.y;
        vecZ = v[indices[(i*3)]].pos.z - v[indices[(i*3)+2]].pos.z;        
        edge1 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our first edge

        //Get the vector describing another edge of our triangle (edge 2,1)
        vecX = v[indices[(i*3)+2]].pos.x - v[indices[(i*3)+1]].pos.x;
        vecY = v[indices[(i*3)+2]].pos.y - v[indices[(i*3)+1]].pos.y;
        vecZ = v[indices[(i*3)+2]].pos.z - v[indices[(i*3)+1]].pos.z;        
        edge2 = XMVectorSet(vecX, vecY, vecZ, 0.0f);    //Create our second edge

        //Cross multiply the two edge vectors to get the un-normalized face normal
        XMStoreFloat3(&unnormalized, XMVector3Cross(edge1, edge2));
        tempNormal.push_back(unnormalized);            //Save unormalized normal (for normal averaging)
    }

    //Compute vertex normals (normal Averaging)
    XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    int facesUsing = 0;
    float tX;
    float tY;
    float tZ;

    //Go through each vertex
    for(int i = 0; i < NumVertices; ++i)
    {
        //Check which triangles use this vertex
        for(int j = 0; j < NumFaces; ++j)
        {
            if(indices[j*3] == i ||
                indices[(j*3)+1] == i ||
                indices[(j*3)+2] == i)
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

        //Store the normal in our current vertex
        v[i].normal.x = XMVectorGetX(normalSum);
        v[i].normal.y = XMVectorGetY(normalSum);
        v[i].normal.z = XMVectorGetZ(normalSum);

        //Clear normalSum and facesUsing for next vertex
        normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        facesUsing = 0;
    }

    vert = std::vector<Vertex>(v.begin(), v.end());
    ind = std::vector<DWORD>(indices.begin(), indices.end());
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

    float speed = 15.0f * time;

    if(keyboardState[DIK_A] & 0x80)
    {
        moveLeftRight -= speed;
    }
    if(keyboardState[DIK_D] & 0x80)
    {
        moveLeftRight += speed;
    }
    if(keyboardState[DIK_W] & 0x80)
    {
        moveBackForward += speed;
    }
    if(keyboardState[DIK_S] & 0x80)
    {
        moveBackForward -= speed;
    }
    if(keyboardState[DIK_1] & 0x80) {
        cdMethod = 0;
    }
    if(keyboardState[DIK_2] & 0x80) {
        cdMethod = 1;
    }
    if(keyboardState[DIK_R] & 0X80)
    {
        float timeFactor = 1.0f;    // You can speed up or slow down time by changing this
        UpdateMD5Model(NewMD5Model, time*timeFactor, 0);
    }

    //Left Mouse Button
    if(mouseCurrState.rgbButtons[0])
    {
        if(isShoot == false)
        {    
            bottleFlying = true;

            thrownBottleWorld = XMMatrixIdentity();
            Translation = XMMatrixTranslation( XMVectorGetX(camPosition), XMVectorGetY(camPosition), XMVectorGetZ(camPosition) );

            thrownBottleWorld = Translation;
            thrownBottleDir = camTarget - camPosition;

            /*POINT mousePos;

            //This gets the cursor position on the screen, which if windowed may not be relative to window
            GetCursorPos(&mousePos);  
            //This function accomodates for that.           
            ScreenToClient(hwnd, &mousePos);

            int mousex = mousePos.x;
            int mousey = mousePos.y;        

            float tempDist;
            float closestDist = FLT_MAX;
            int hitIndex;

            XMVECTOR prwsPos, prwsDir;
            pickRayVector(mousex, mousey, prwsPos, prwsDir);

            double pickOpStartTime = GetTime();        // Get the time before we start our picking operation

            for(int i = 0; i < numBottles; i++)
            {
                if(bottleHit[i] == 0) // No need to check bottles already hit
                {        
                    tempDist = FLT_MAX;

                    if(pickWhat == 0)
                    {                        
                        float pRToPointDist = 0.0f; // Closest distance from the pick ray to the objects center

                        XMVECTOR bottlePos = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
                        XMVECTOR pOnLineNearBottle = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

                        // For the Bounding Sphere to work correctly, we need to make sure we are testing
                        // the distance from the objects "actual" center and the pick ray. We have stored
                        // the distance from (0, 0, 0) in the objects model space to the object "actual"
                        // center in bottleCenterOffset. So now we just need to add that difference to
                        // the bottles world space position, this way the bounding sphere will be centered
                        // on the object real center.
                        bottlePos = XMVector3TransformCoord(bottlePos, bottleWorld[i]) + bottleCenterOffset;

                        // This equation gets the point on the pick ray which is closest to bottlePos
                        pOnLineNearBottle = prwsPos + XMVector3Dot((bottlePos - prwsPos), prwsDir) / XMVector3Dot(prwsDir, prwsDir) * prwsDir;

                        // Now we get the distance between bottlePos and pOnLineNearBottle
                        // This line is slightly less accurate, but it offers a performance increase by
                        // estimating the distance using XMVector3LengthEst()
                        //pRToPointDist = XMVectorGetX(XMVector3LengthEst(pOnLineNearBottle - bottlePos));                
                        pRToPointDist = XMVectorGetX(XMVector3Length(pOnLineNearBottle - bottlePos));

                        // If the distance between the closest point on the pick ray (pOnLineNearBottle) to bottlePos
                        // is less than the bottles bounding sphere (represented by a float called bottleBoundingSphere)
                        // then we know the pick ray has intersected with the bottles bounding sphere, and we can move on
                        // to testing if the pick ray has actually intersected with the bottle itself.
                        if(pRToPointDist < bottleBoundingSphere)
                        {
                            // This line is the distance to the pick ray intersection with the sphere
                            //tempDist = XMVectorGetX(XMVector3Length(pOnLineNearBottle - prwsPos));

                            // Check for picking with the actual model now
                            tempDist = pick(prwsPos, prwsDir, bottleVertPosArray, bottleVertIndexArray, bottleWorld[i]);
                        }
                    }

                    // Bounding Box picking test
                    if(pickWhat == 1)
                        tempDist = pick(prwsPos, prwsDir, bottleBoundingBoxVertPosArray, bottleBoundingBoxVertIndexArray, bottleWorld[i]);

                    // Check for picking directly with the model without bounding volumes testing first
                    if(pickWhat == 2)
                        tempDist = pick(prwsPos, prwsDir, bottleVertPosArray, bottleVertIndexArray, bottleWorld[i]);

                    if(tempDist < closestDist)
                    {
                        closestDist = tempDist;
                        hitIndex = i;
                    }
                }
            }

            // This is the time in seconds it took to complete the picking process
            pickOpSpeed = GetTime() - pickOpStartTime;

            if(closestDist < FLT_MAX)
            {
                bottleHit[hitIndex] = 1;
                pickedDist = closestDist;
                score++;
            }        
*/
            isShoot = true;
        }
    }
    if(!mouseCurrState.rgbButtons[0])
    {
        isShoot = false;
    }


    if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
    {
        camYaw += mouseLastState.lX * 0.001f;

        camPitch += mouseCurrState.lY * 0.001f;
        if(abs(camPitch) > 1.57) {
            if(camPitch > 0) camPitch = 1.57;
            else camPitch = -1.57;
        }

        mouseLastState = mouseCurrState;
    }

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
    camRotationMatrix = XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0);
    camTarget = XMVector3TransformCoord(DefaultForward, camRotationMatrix );
    camTarget = XMVector3Normalize(camTarget);

    // First-Person Camera
    XMMATRIX RotateYTempMatrix;
    RotateYTempMatrix = XMMatrixRotationY(camYaw);
    camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
    camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
    camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

    /*
    // Free-Look Camera
    camRight = XMVector3TransformCoord(DefaultRight, camRotationMatrix);
    camForward = XMVector3TransformCoord(DefaultForward, camRotationMatrix);
    camUp = XMVector3Cross(camForward, camRight);
    */

    CollisionPacket cameraCP;
    cameraCP.ellipsoidSpace = XMVectorSet(1.0f, 3.0f, 1.0f, 0.0f);
    cameraCP.w_Position = camPosition;
    cameraCP.w_Velocity = (moveLeftRight*camRight)+(moveBackForward*camForward);

    camPosition = CollisionSlide(cameraCP,
        collidableGeometryPositions,
        collidableGeometryIndices);

    /*camPosition += moveLeftRight*camRight;
    camPosition += moveBackForward*camForward;*/

    moveLeftRight = 0.0f;
    moveBackForward = 0.0f;

    camTarget = camPosition + camTarget;    

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

bool HeightMapLoad(const char* filename, HeightMapInfo &hminfo)
    {
        FILE *filePtr;                            // Point to the current position in the file
        BITMAPFILEHEADER bitmapFileHeader;        // Structure which stores information about file
        BITMAPINFOHEADER bitmapInfoHeader;        // Structure which stores information about image
        int imageSize, index;
        unsigned char height;
    
        // Open the file
        filePtr = fopen(filename,"rb");
        if (filePtr == NULL) {
            printf("Could not open heightmap\n");
            return 0;
        }
        // Read bitmaps header
        fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1,filePtr);
    
        // Read the info header
        fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
    
        // Get the width and height (width and length) of the image
        hminfo.terrainWidth = bitmapInfoHeader.biWidth;
        hminfo.terrainHeight = bitmapInfoHeader.biHeight;
    
        // Size of the image in bytes. the 3 represents RBG (byte, byte, byte) for each pixel
        imageSize = hminfo.terrainWidth * hminfo.terrainHeight * 3;
    
        // Initialize the array which stores the image data
        unsigned char* bitmapImage = new unsigned char[imageSize];
    
        // Set the file pointer to the beginning of the image data
        fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);
    
        // Store image data in bitmapImage
        fread(bitmapImage, 1, imageSize, filePtr);
    
        // Close file
        fclose(filePtr);
    
        // Initialize the heightMap array (stores the vertices of our terrain)
        hminfo.heightMap = new XMFLOAT3[hminfo.terrainWidth * hminfo.terrainHeight];
    
        // We use a greyscale image, so all 3 rgb values are the same, but we only need one for the height
        // So we use this counter to skip the next two components in the image data (we read R, then skip BG)
        int k=0;
    
        // We divide the height by this number to "water down" the terrains height, otherwise the terrain will
        // appear to be "spikey" and not so smooth.
        float heightFactor = 10.0f;
    
        // Read the image data into our heightMap array
        for(int j=0; j< hminfo.terrainHeight; j++)
        {
            for(int i=0; i< hminfo.terrainWidth; i++)
            {
                height = bitmapImage[k];
    
                index = ( hminfo.terrainHeight * j) + i;
    
                hminfo.heightMap[index].x = (float)i;
                hminfo.heightMap[index].y = (float)height / heightFactor;
                hminfo.heightMap[index].z = (float)j;
    
                k+=3;
            }
        }
    
        delete [] bitmapImage;
        bitmapImage = 0;
    
        return true;
    }

XMVECTOR CollisionSlide(CollisionPacket& cP,
    std::vector<XMFLOAT3>& vertPos,
    std::vector<DWORD>& indices)
{
    // Transform velocity vector to the ellipsoid space (e_ denotes ellipsoid space)
    cP.e_Velocity = cP.w_Velocity/cP.ellipsoidSpace;

    // Transform position vector to the ellipsoid space
    cP.e_Position = cP.w_Position/cP.ellipsoidSpace;

    // Now we check for a collision with our world, this function will
    // call itself 5 times at most, or until the velocity vector is
    // used up (very small (near zero to zero length))
    cP.collisionRecursionDepth = 0;
    XMVECTOR finalPosition = CollideWithWorld(cP, vertPos, indices);

    // Add gravity pull:
    // This is simply adding a new velocity vector in the downward
    // direction (defined globaly) to pull the ellipsoid down, then doing the
    // collision check against all the geometry again. The way it is now, the
    // ellipsoid will "slide" down even the most slightest slope. Consider this
    // an exercise: only impliment gravity when standing on a very steep slope,
    // or if you are not standing on anything at all (free fall)

    // To add gravity uncomment from here .....

    cP.e_Velocity = gravity / cP.ellipsoidSpace;    // We defined gravity in world space, so now we have
    // to convert it to ellipsoid space
    cP.e_Position = finalPosition;
    cP.collisionRecursionDepth = 0;
    finalPosition = CollideWithWorld(cP, vertPos, indices);

    // ... to here

    // Convert our final position from ellipsoid space to world space
    finalPosition = finalPosition * cP.ellipsoidSpace;

    // Return our final position!
    return finalPosition;
}

XMVECTOR CollideWithWorld(CollisionPacket& cP,
    std::vector<XMFLOAT3>& vertPos,
    std::vector<DWORD>& indices)
{
    // These are based off the unitsPerMeter from above
    float unitScale = unitsPerMeter / 100.0f;
    float veryCloseDistance = 0.005f * unitScale;    // This is used to keep the sphere from actually "touching" 
    // the triangle, as that would cause problems since
    // each loop it would ALWAYS find a collision instead
    // of just sliding along the triangle

    // This will stop us from entering an infinite loop, or a very long loop. For example, there are times when the sphere
    // might actually be pushed slightly into the triangles center, where the recursion will keep repeating and finding a collision
    // even though the velocity vector does not change (I had serious problems with this part for a couple days... I couldn't
    // figure out why the ellipsoid would LAUNCH at certain times, but if i set the ellipsoid space to (1,1,1) (a sphere), it would
    // act normal. Stupid me made a mistake and was returning w_Position here instead of e_Position, so that the world space position
    // was being multiplied by the ellipsoid space and "launching" it whenever it accidently got pushed into a triangle)
    if (cP.collisionRecursionDepth > 5)
        return cP.e_Position;

    // Normalize velocity vector
    cP.e_normalizedVelocity = XMVector3Normalize(cP.e_Velocity);    

    // Initialize collision packet stuff
    cP.foundCollision = false;
    cP.nearestDistance = 0.0f;

    // Loop through each triangle in mesh and check for a collision
    for(int triCounter = 0; triCounter < indices.size() / 3; triCounter++)
    {
        // Get triangle
        XMVECTOR p0, p1, p2, tempVec;
        p0 = XMLoadFloat3(&vertPos[indices[3*triCounter]]);
        p1 = XMLoadFloat3(&vertPos[indices[3*triCounter+1]]);
        p2 = XMLoadFloat3(&vertPos[indices[3*triCounter+2]]);

        // Put triangle into ellipsoid space
        p0 = p0/cP.ellipsoidSpace;
        p1 = p1/cP.ellipsoidSpace;
        p2 = p2/cP.ellipsoidSpace;

        // Calculate the normal for this triangle
        XMVECTOR triNormal;
        triNormal = XMVector3Normalize(XMVector3Cross((p1 - p0),(p2 - p0)));

        // Now we check to see if the sphere is colliding with the current triangle
        SphereCollidingWithTriangle(cP, p0, p1, p2, triNormal);
    }

    // If there was no collision, return the position + velocity
    if (cP.foundCollision == false) {
        return cP.e_Position + cP.e_Velocity;
    }

    // If we've made it here, a collision occured
    // destinationPoint is where the sphere would travel if there was
    // no collisions, however, at this point, there has a been a collision
    // detected. We will use this vector to find the new "sliding" vector
    // based off the plane created from the sphere and collision point
    XMVECTOR destinationPoint = cP.e_Position + cP.e_Velocity;

    XMVECTOR newPosition = cP.e_Position;    // Just initialize newPosition

    // If the position is further than "veryCloseDistance" from the point
    // of collision, we will move the sphere along the velocity path until
    // it "almost" touches the triangle, or point of collision. We do this so
    // that the next recursion (if there is one) does not detect a collision
    // with the triangle we just collided with. We don't need to find a collision
    // with the triangle we just collided with because we will be moving parallel
    // to it now, and if we were finding the collision with it every recursion
    // (since it's the closest triangle we would collide with), we would 
    // finish our 5 recursions (checked above) without ever moving after
    // touching a triangle, because before the triangle has a chance to move
    // down the new velocity path (even though it's about parallel with the triangle)
    // it would find the collision with the triangle, and simply recompute the same
    // velocity vector it computed the first time. This would happen because of
    // floating point innacuracy. theoretically, we would not have to worry about this
    // because after the new velocity vector is created, it SHOULD be perfectly parallel
    // to the triangle, and we detect that in our code and basically skip triangles
    // who are perfectly parallel with the velocity vector. But like i said, because
    // of innacuracy, the new velocity vector might be VERY SLIGHTLY pointed down towards
    // the triangles plane, which would make us waste a recursion just to recompute the same
    // velocity vector. Basically, the whole sliding thing works without this, but it's a lot
    // more "choppy" and "sticky", where you get stuck in random places.
    if (cP.nearestDistance >= veryCloseDistance)
    {
        // Move the new position down velocity vector to ALMOST touch the collision point
        XMVECTOR V = cP.e_Velocity;
        V = XMVector3Normalize(V);
        V = V * (cP.nearestDistance - veryCloseDistance);
        newPosition = cP.e_Position + V;

        // Adjust polygon intersection point (so sliding
        // plane will be unaffected by the fact that we
        // move slightly less than collision tells us)
        V = XMVector3Normalize(V);
        cP.intersectionPoint -= veryCloseDistance * V;
    }

    // This is our sliding plane (point in the plane and plane normal)
    XMVECTOR slidePlaneOrigin = cP.intersectionPoint;
    XMVECTOR slidePlaneNormal =    newPosition - cP.intersectionPoint;
    slidePlaneNormal = XMVector3Normalize(slidePlaneNormal);

    // We will use the sliding plane to compute our new "destination" point
    // and new velocity vector. To do this, we will need to solve another quadratic
    // equation (Ax + By + Cz + D = 0), where D is what we call the plane constant,
    // which we use to find the distance between the sliding plane and our original
    // destination point (original as up until now, since it's likely that this is
    // not the first recursion of this function, and the original original destination
    // has been changed up until now).
    // First the point in the plane
    float x = XMVectorGetX(slidePlaneOrigin);
    float y = XMVectorGetY(slidePlaneOrigin);
    float z = XMVectorGetZ(slidePlaneOrigin);

    // Next the planes normal
    float A = XMVectorGetX(slidePlaneNormal);
    float B = XMVectorGetY(slidePlaneNormal);
    float C = XMVectorGetZ(slidePlaneNormal);
    float D = -((A*x) + (B*y) + (C*z));

    // To keep the variable names clear, we will rename D to planeConstant
    float planeConstant = D;

    // Get the distance between sliding plane and destination point
    float signedDistFromDestPointToSlidingPlane = XMVectorGetX(XMVector3Dot(destinationPoint, slidePlaneNormal)) + planeConstant;

    // Now we calculate the new destination point. To get the new destination point, we will subtract
    // the distance from the plane to the original destination point (down the planes normal) from the
    // original destination point. It's easier to picture this in your head than explain, so let me try
    // to give you a very simple picture. Pretend you are this equation, standing on the plane, where UP
    // (your head) is pointing the same direction as the plane's normal. directly below you is the "destination"
    // point of the sphere. Your job as this equation is to "pull" the destination point up (towards the planes
    // normal) until it is resting "in" the plane. If you can picture this the way i'm trying to get you to, you
    // can see that the new velocity vector (from the point of collision between sphere and plane) to the new
    // destination is "shorter" and parallel to the plane, so that now when the sphere follows this new velocity
    // vector, it will be traveling parallel (sliding) across the triangle, at the same time, it does not travel
    // as far as it would have if there was no collision. This is exactly what we want, because when you think about
    // it, we do not run up mountains as fast as we run on flat ground, and if we run straight into a wall in our
    // game, we will just stop moving, or if we run ALMOST straight into the wall, we will not go cruising sideways,
    // but instead slowly move to either side. In my lesson on braynzarsoft.net, This is explained in pictures
    XMVECTOR newDestinationPoint = destinationPoint - signedDistFromDestPointToSlidingPlane * slidePlaneNormal;

    // I believe this line was covered briefly in the above explanation
    XMVECTOR newVelocityVector = newDestinationPoint - cP.intersectionPoint;

    // After this check, we will recurse. This check makes sure that we have not
    // come to the end of our velocity vector (or very close to it, because if the velocity
    // vector is very small, there is no reason to lose performance by doing an extra recurse
    // when we won't even notice the distance "thrown away" by this check anyway) before
    // we recurse
    if (XMVectorGetX(XMVector3Length(newVelocityVector)) < veryCloseDistance) {
        return newPosition;
    }

    // We are going to recurse now since a collision was found and the velocity
    // changed directions. we need to check if the new velocity vector will
    // cause the sphere to collide with other geometry.
    cP.collisionRecursionDepth++;
    cP.e_Position = newPosition;
    cP.e_Velocity = newVelocityVector;
    return CollideWithWorld(cP, vertPos, indices);
}

bool SphereCollidingWithTriangle(CollisionPacket& cP, XMVECTOR &p0, XMVECTOR &p1, XMVECTOR &p2, XMVECTOR &triNormal)
{
    // This function assumes p0, p1, p2, and the triangle normal are in ellipsoid space
    // and that e_Position e_Velocity, and e_normalizedVelocity are defined in ellipsoid space
    // In other words, this function checks for a collision between a SPHERE and a triangle,
    // not an ellipsoid and a triangle. Because of this, the results from this function
    // (specifically cP.nearestDistance and cP.intersectionPoint) are in ellipsoid space

    // Check to see if triangle is facing velocity vector
    // We will not triangle facing away from the velocity vector to speed this up
    // since we assume that we will never run into the back face of triangles
    float facing = XMVectorGetX(XMVector3Dot(triNormal, cP.e_normalizedVelocity));
    if(facing <= 0)
    { 
        // Create these because cP.e_Velocity and cP.e_Position add slightly to the difficulty
        // of reading the equations
        XMVECTOR velocity = cP.e_Velocity;
        XMVECTOR position = cP.e_Position;

        // t0 and t1 hold the time it takes along the velocity vector that the sphere (called a swept sphere)
        // will "collide" (resting on or touching), once on the front side of the triangle (t0), and once on the
        // backside after it goes "through" the triangle (t1) (or vertex or edge).
        float t0, t1;

        // If sphere is in the plane, it will not intersect with the center of the triangle
        // but instead possibly intersect with one of the vertices or edges first
        bool sphereInPlane = false;

        // Find the plane equation in which the triangle lies in (Ax + By + Cz + D = 0)
        // A, B, and C are the planes normal, x, y, and z respectively
        // We can find D (a.k.a the plane constant) using some simple algebra, which we will do below
        // x, y, and z in the equation defines a point in the plane. Any point in the plane
        // will do, so we will just use p0

        // First the point in the plane
        float x = XMVectorGetX(p0);
        float y = XMVectorGetY(p0);
        float z = XMVectorGetZ(p0);

        // Next the planes normal
        float A = XMVectorGetX(triNormal);
        float B = XMVectorGetY(triNormal);
        float C = XMVectorGetZ(triNormal);

        // Lets solve for D
        // step 1: 0 = Ax + By + Cz + D
        // step 2: subtract D from both sides
        //            -D = Ax + By + Cz
        // setp 3: multiply both sides by -1
        //            -D*-1 = -1 * (Ax + By + Cz)
        // final answer: D = -(Ax + By + Cz)
        float D = -((A*x) + (B*y) + (C*z));

        // To keep the variable names clear, we will rename D to planeConstant
        float planeConstant = D;

        // Get the signed distance from the cameras position (or object if you are using an object)
        // We can get the signed distance between a point and plane with the equation:
        // SignedDistance = PlaneNormal * Point + PlaneConstant 

        // I've mentioned this before, but i'll do it again. When using xna math library vector function
        // that return a scalar value (like a float) such as "XMVector3Dot", an XMVECTOR is returned, with
        // all elements (x,y,z,w) containing that scalar value. We need to extract one, and any will do since
        // they are all the same, so we extract the x component using "XMVectorGetX"
        float signedDistFromPositionToTriPlane = XMVectorGetX(XMVector3Dot(position, triNormal)) + planeConstant;

        // This will be used a couple times below, so we'll just calculate and store it now
        float planeNormalDotVelocity = XMVectorGetX(XMVector3Dot(triNormal, velocity));

        /////////////////////////////////////Sphere Plane Collision Test////////////////////////////////////////////
        // Check to see if the velocity vector is parallel with the plane
        if (planeNormalDotVelocity == 0.0f) 
        {
            if (fabs(signedDistFromPositionToTriPlane) >= 1.0f) 
            {
                // sphere not in plane, and velocity is
                // parallel to plane, no collision possible
                return false;
            }
            else 
            {
                // sphere is in the plane, so we will now only test for a collision
                // with the triangle's vertices and edges
                // Set sphereInPlane to true so we do not do the operation
                // which will divide by zero if the velocity and plane are parallel
                sphereInPlane = true;
            }
        }
        else 
        {
            // We know the velocity vector at some point intersects with the plane, we just
            // need to find how far down the velocity vector the sphere will "touch" or rest
            // on the plane. t0 is when it first touches the plane (front side of sphere touches)
            // and t1 is when the back side of the sphere touches.

            // To find when (the time or how far down the velocity vector) the "velocity vector" itself
            //intersects with the plane, we use the equation: (* stands for a dot product)
            // t = (PlaneNormal * Point + PlaneConstant) / (PlaneNormal * Velocity);
            // We have already calculated both sides of the divide sign "/", so:
            // t = signedDistance / normalDotVelocity;

            // Now remember we are working with a unit sphere (since everything has been moved from
            // the usual space to our ellipsoid space). The unit sphere means that the distance from
            // the center of the sphere to ANYWHERE on it's surface is "1". We are not interested in
            // finding when the actual velocity vector intersects with the plane, but instead when
            // the surface of the sphere "touches" the surface of the plane. We know that the distance
            // from the center of the sphere is "1", so all we have to do to find when the sphere touches
            // the plane is subtract and subtract 1 from the signed distance to get when both sides of the
            // sphere touch the plane (t0, and t1)
            t0 = ( 1.0f - signedDistFromPositionToTriPlane) / planeNormalDotVelocity;
            t1 = (-1.0f - signedDistFromPositionToTriPlane) / planeNormalDotVelocity;

            // We will make sure that t0 is smaller than t1, which means that t0 is when the sphere FIRST
            // touches the planes surface
            if(t0 > t1)
            {
                float temp = t0;
                t0 = t1;
                t1 = temp;
            }

            // If the swept sphere touches the plane outside of the 0 to 1 "timeframe", we know that
            // the sphere is not going to intersect with the plane (and of course triangle) this frame
            if (t0 > 1.0f || t1 < 0.0f) 
            {
                return false;
            }

            // If t0 is smaller than 0 then we will make it 0
            // and if t1 is greater than 1 we will make it 1
            if (t0 < 0.0) t0 = 0.0;
            if (t1 > 1.0) t1 = 1.0;
        }

        ////////////////////////////////Sphere-(Inside Triangle) Collision Test///////////////////////////////////////
        // If we've made it this far, we know that the sphere will intersect with the triangles plane
        // This frame, so now we will check to see if the collision happened INSIDE the triangle
        XMVECTOR collisionPoint;        // Point on plane where collision occured
        bool collidingWithTri = false;    // This is set so we know if there was a collision with the CURRENT triangle
        float t = 1.0;                    // Time 

        // If the sphere is not IN the triangles plane, we continue the sphere to inside of triangle test
        if (!sphereInPlane) 
        {
            // We get the point on the triangles plane where the sphere "touches" the plane
            // using the equation: planeIntersectionPoint = (Position - Normal) + t0 * Velocity
            // Where t0 is the distance down the velocity vector that the sphere first makes
            // contact with the plane
            XMVECTOR planeIntersectionPoint = (position + t0 * velocity - triNormal);

            // Now we call the function that checks if a point on a triangle's plane is inside the triangle
            if (checkPointInTriangle(planeIntersectionPoint,p0,p1,p2))
            {
                // If the point on the plane IS inside the triangle, we know that the sphere is colliding
                // with the triangle now, so we set collidingWithTri to true so we don't do all the extra
                // calculations on the triangle. We set t to t0, which is the time (or distance) down
                // the velocity vector that the sphere first makes contact, then we set the point of 
                // collision, which will be used later for our collision response
                collidingWithTri = true;
                t = t0;
                collisionPoint = planeIntersectionPoint;
            }
        }

        /////////////////////////////////////Sphere-Vertex Collision Test//////////////////////////////////////////////
        // If the sphere is not colliding with the triangles INSIDE, we check to see if it will collide with one of
        // the vertices of the triangle using the sweep test we did above, but this time check for each vertex instead
        // of the triangles plane
        if (collidingWithTri == false) 
        {
            // We will be working with the quadratic function "At^2 + Bt + C = 0" to find when (t) the "swept sphere"s center
            // is 1 unit (spheres radius) away from the vertex position. Remember the swept spheres position is actually a line defined
            // by the spheres position and velocity. t represents it's position along the velocity vector.
            // a = sphereVelocityLength * sphereVelocityLength
            // b = 2(sphereVelocity . (spherePosition - vertexPosition))    // . denotes dot product
            // c = (vertexPosition - spherePosition)^2 - 1
            // This equation allows for two solutions. One is when the sphere "first" touches the vertex, and the other is when
            // the "other" side of the sphere touches the vertex on it's way past the vertex. We need the first "touch"
            float a, b, c; // Equation Parameters

            // We can use the squared velocities length below when checking for collisions with the edges of the triangles
            // to, so to keep things clear, we won't set a directly
            float velocityLengthSquared = XMVectorGetX(XMVector3Length(velocity));
            velocityLengthSquared *= velocityLengthSquared;

            // We'll start by setting 'a', since all 3 point equations use this 'a'
            a = velocityLengthSquared;

            // This is a temporary variable to hold the distance down the velocity vector that
            // the sphere will touch the vertex.
            float newT;

            // P0 - Collision test with sphere and p0
            b = 2.0f * ( XMVectorGetX( XMVector3Dot( velocity, position - p0 )));
            c = XMVectorGetX(XMVector3Length((p0 - position)));
            c = (c*c) - 1.0f;
            if (getLowestRoot(a,b,c, t, &newT)) {    // Check if the equation can be solved
                // If the equation was solved, we can set a couple things. First we set t (distance
                // down velocity vector the sphere first collides with vertex) to the temporary newT,
                // Then we set collidingWithTri to be true so we know there was for sure a collision
                // with the triangle, then we set the exact point the sphere collides with the triangle,
                // which is the position of the vertex it collides with
                t = newT;
                collidingWithTri = true;
                collisionPoint = p0;
            }

            // P1 - Collision test with sphere and p1
            b = 2.0*(XMVectorGetX(XMVector3Dot(velocity, position - p1)));
            c = XMVectorGetX(XMVector3Length((p1 - position)));
            c = (c*c) - 1.0;
            if (getLowestRoot(a,b,c, t, &newT)) {
                t = newT;
                collidingWithTri = true;
                collisionPoint = p1;
            }

            // P2 - Collision test with sphere and p2
            b = 2.0*(XMVectorGetX(XMVector3Dot(velocity, position - p2)));
            c = XMVectorGetX(XMVector3Length((p2 - position)));
            c = (c*c) - 1.0;
            if (getLowestRoot(a,b,c, t, &newT)) {
                t = newT;
                collidingWithTri = true;
                collisionPoint = p2;
            }

            //////////////////////////////////////////////Sphere-Edge Collision Test//////////////////////////////////////////////
            // Even though there might have been a collision with a vertex, we will still check for a collision with an edge of the
            // triangle in case an edge was hit before the vertex. Again we will solve a quadratic equation to find where (and if)
            // the swept sphere's position is 1 unit away from the edge of the triangle. The equation parameters this time are a 
            // bit more complex: (still "Ax^2 + Bx + C = 0")
            // a = edgeLength^2 * -velocityLength^2 + (edge . velocity)^2
            // b = edgeLength^2 * 2(velocity . spherePositionToVertex) - 2((edge . velocity)(edge . spherePositionToVertex))
            // c =  edgeLength^2 * (1 - spherePositionToVertexLength^2) + (edge . spherePositionToVertex)^2
            // . denotes dot product

            // Edge (p0, p1):
            XMVECTOR edge = p1 - p0;
            XMVECTOR spherePositionToVertex = p0 - position;
            float edgeLengthSquared = XMVectorGetX(XMVector3Length(edge));
            edgeLengthSquared *= edgeLengthSquared;
            float edgeDotVelocity = XMVectorGetX(XMVector3Dot(edge, velocity));
            float edgeDotSpherePositionToVertex = XMVectorGetX(XMVector3Dot(edge, spherePositionToVertex));
            float spherePositionToVertexLengthSquared = XMVectorGetX(XMVector3Length(spherePositionToVertex));
            spherePositionToVertexLengthSquared = spherePositionToVertexLengthSquared * spherePositionToVertexLengthSquared;

            // Equation parameters
            a = edgeLengthSquared * -velocityLengthSquared + (edgeDotVelocity * edgeDotVelocity);
            b = edgeLengthSquared * (2.0f * XMVectorGetX(XMVector3Dot(velocity, spherePositionToVertex))) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
            c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

            // We start by finding if the swept sphere collides with the edges "infinite line"
            if (getLowestRoot(a,b,c, t, &newT)) {
                // Now we check to see if the collision happened between the two vertices that make up this edge
                // We can calculate where on the line the collision happens by doing this:
                // f = (edge . velocity)newT - (edge . spherePositionToVertex) / edgeLength^2
                // if f is between 0 and 1, then we know the collision happened between p0 and p1
                // If the collision happened at p0, the f = 0, if the collision happened at p1 then f = 1
                float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
                if (f >= 0.0f && f <= 1.0f) {
                    // If the collision with the edge happened, we set the results
                    t = newT;
                    collidingWithTri = true;
                    collisionPoint = p0 + f * edge;
                }
            }

            // Edge (p1, p2):
            edge = p2 - p1;
            spherePositionToVertex = p1 - position;
            edgeLengthSquared = XMVectorGetX(XMVector3Length(edge));
            edgeLengthSquared = edgeLengthSquared * edgeLengthSquared;
            edgeDotVelocity = XMVectorGetX(XMVector3Dot(edge, cP.e_Velocity));
            edgeDotSpherePositionToVertex = XMVectorGetX(XMVector3Dot(edge, spherePositionToVertex));
            spherePositionToVertexLengthSquared = XMVectorGetX(XMVector3Length(spherePositionToVertex));
            spherePositionToVertexLengthSquared = spherePositionToVertexLengthSquared * spherePositionToVertexLengthSquared;

            a = edgeLengthSquared * -velocityLengthSquared + (edgeDotVelocity * edgeDotVelocity);
            b = edgeLengthSquared * (2.0f * XMVectorGetX(XMVector3Dot(velocity, spherePositionToVertex))) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
            c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

            if (getLowestRoot(a,b,c, t, &newT)) {
                float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
                if (f >= 0.0f && f <= 1.0f) {
                    t = newT;
                    collidingWithTri = true;
                    collisionPoint = p1 + f * edge;
                }
            }

            // Edge (p2, p0):
            edge = p0 - p2;
            spherePositionToVertex = p2 - position;
            edgeLengthSquared = XMVectorGetX(XMVector3Length(edge));
            edgeLengthSquared = edgeLengthSquared * edgeLengthSquared;
            edgeDotVelocity = XMVectorGetX(XMVector3Dot(edge, velocity));
            edgeDotSpherePositionToVertex = XMVectorGetX(XMVector3Dot(edge, spherePositionToVertex));
            spherePositionToVertexLengthSquared = XMVectorGetX(XMVector3Length(spherePositionToVertex));
            spherePositionToVertexLengthSquared = spherePositionToVertexLengthSquared * spherePositionToVertexLengthSquared;

            a = edgeLengthSquared * -velocityLengthSquared + (edgeDotVelocity * edgeDotVelocity);
            b = edgeLengthSquared * (2.0f * XMVectorGetX(XMVector3Dot(velocity, spherePositionToVertex))) - (2.0f * edgeDotVelocity * edgeDotSpherePositionToVertex);
            c = edgeLengthSquared * (1.0f - spherePositionToVertexLengthSquared) + (edgeDotSpherePositionToVertex * edgeDotSpherePositionToVertex);

            if (getLowestRoot(a,b,c, t, &newT)) {
                float f = (edgeDotVelocity * newT - edgeDotSpherePositionToVertex) / edgeLengthSquared;
                if (f >= 0.0f && f <= 1.0f) {
                    t = newT;
                    collidingWithTri = true;
                    collisionPoint = p2 + f * edge;
                }
            }
        }

        // If we have found a collision, we will set the results of the collision here
        if (collidingWithTri == true) 
        {
            // We find the distance to the collision using the time variable (t) times the length of the velocity vector
            float distToCollision = t * XMVectorGetX(XMVector3Length(velocity));

            // Now we check if this is the first triangle that has been collided with OR it is 
            // the closest triangle yet that was collided with
            if (cP.foundCollision == false || distToCollision < cP.nearestDistance) {

                // Collision response information (used for "sliding")
                cP.nearestDistance = distToCollision;
                cP.intersectionPoint = collisionPoint;

                // Make sure this is set to true if we've made it this far
                cP.foundCollision = true;
                return true;
            }            
        }
    }
    return false;
}

bool checkPointInTriangle(const XMVECTOR& point, const XMVECTOR& triV1,const XMVECTOR& triV2, const XMVECTOR& triV3)
{
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
        }
    }
    return false;
}

// This function solves the quadratic eqation "At^2 + Bt + C = 0" and is found in Kasper Fauerby's paper on collision detection and response
bool getLowestRoot(float a, float b, float c, float maxR, float* root) 
{
    // Check if a solution exists
    float determinant = b*b - 4.0f*a*c;
    // If determinant is negative it means no solutions.
    if (determinant < 0.0f) return false;
    // calculate the two roots: (if determinant == 0 then
    // x1==x2 but lets disregard that slight optimization)
    float sqrtD = sqrt(determinant);
    float r1 = (-b - sqrtD) / (2*a);
    float r2 = (-b + sqrtD) / (2*a);
    // Sort so x1 <= x2
    if (r1 > r2) {
        float temp = r2;
        r2 = r1;
        r1 = temp;
    }
    // Get lowest root:
    if (r1 > 0 && r1 < maxR) {
        *root = r1;
        return true;
    }
    // It is possible that we want x2 - this can happen
    // if x1 < 0
    if (r2 > 0 && r2 < maxR) {
        *root = r2;
        return true;
    }

    // No (valid) solutions
    return false;
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
