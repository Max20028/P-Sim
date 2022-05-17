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
bool LoadObjModel(std::wstring filename,        //.obj filename
    ID3D11Buffer** vertBuff,            //mesh vertex buffer
    ID3D11Buffer** indexBuff,            //mesh index buffer
    std::vector<int>& subsetIndexStart,        //start index of each subset
    std::vector<int>& subsetMaterialArray,        //index value of material for each subset
    std::vector<struct SurfaceMaterial>& material,        //vector of material structures
    int& subsetCount,                //Number of subsets in mesh
    bool isRHCoordSys,                //true if model was created in right hand coord system
    bool computeNormals);                //true to compute the normals, false to use the files normals

void StartTimer();
double GetTime();
double GetFrameTime();

struct Vertex    //Overloaded Vertex Structure
{
    Vertex(){}
    Vertex(float x, float y, float z,
        float u, float v,
        float nx, float ny, float nz)
        : pos(x,y,z), texcoord(u, v), normal(nx, ny, nz){}

    XMFLOAT3 pos;
    XMFLOAT2 texcoord;
    XMFLOAT3 normal;
};

struct cbPerObject
{
    XMMATRIX  WVP;
    XMMATRIX  World;
    XMFLOAT4 difColor;
    bool hasTexture;
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

struct SurfaceMaterial
{
    std::wstring matName;
    XMFLOAT4 difColor;
    int texArrayIndex;
    bool hasTexture;
    bool transparent;
};

std::vector<SurfaceMaterial> material;


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
        std::wostringstream printString;
        printString << text << inInt << "\nPitch: " << camPitch << "\nLightPos: " << light.pos.x << "," << light.pos.y << ", " << light.pos.z;
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

    light.dir.x = XMVectorGetX(camTarget) - light.pos.x;
    light.dir.y = XMVectorGetY(camTarget) - light.pos.y;
    light.dir.z = XMVectorGetZ(camTarget) - light.pos.z;

    meshWorld = XMMatrixIdentity();

    //Define cube1's world space matrix
    Rotation = XMMatrixRotationY(3.14f);
    Scale = XMMatrixScaling( 1.0f, 1.0f, 1.0f );
    Translation = XMMatrixTranslation( 0.0f, 0.0f, 0.0f );

    meshWorld = Rotation * Scale * Translation;
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
    scd.Windowed = FALSE;                                    // windowed/full-screen mode
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
        // printf("%d\n",cbPerObj.hasTexture);
        devcon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
        devcon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
        devcon->PSSetConstantBuffers( 1, 1, &cbPerObjectBuffer );
        if(material[meshSubsetTexture[i]].hasTexture)
            devcon->PSSetShaderResources( 0, 1, &meshSRV[material[meshSubsetTexture[i]].texArrayIndex] );
        devcon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

        devcon->RSSetState(NoCullMode);
        int indexStart = meshSubsetIndexStart[i];
        int indexDrawAmount =  meshSubsetIndexStart[i+1] - meshSubsetIndexStart[i];
        if(!material[meshSubsetTexture[i]].transparent)
            devcon->DrawIndexed( indexDrawAmount, indexStart, 0 );
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
        {"NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
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
    light.ambient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
    light.diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    //Vertex Buffer
    //For a square for the ground
    Vertex v[] =
    {
        // Bottom Face
        Vertex(-1.0f, -1.0f, -1.0f, 100.0f, 100.0f, 0.0f, 1.0f, 0.0f),
        Vertex( 1.0f, -1.0f, -1.0f,   0.0f, 100.0f, 0.0f, 1.0f, 0.0f),
        Vertex( 1.0f, -1.0f,  1.0f,   0.0f,   0.0f, 0.0f, 1.0f, 0.0f),
        Vertex(-1.0f, -1.0f,  1.0f, 100.0f,   0.0f, 0.0f, 1.0f, 0.0f),
    };
    
    DWORD indices[] = {
        0,  1,  2,
        0,  2,  3,
    };

    //Square Index Buffer
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * 2 * 3;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;


    D3D11_SUBRESOURCE_DATA iinitData;

    iinitData.pSysMem = indices;
    dev->CreateBuffer(&indexBufferDesc, &iinitData, &squareIndexBuffer);

    devcon->IASetIndexBuffer( squareIndexBuffer, DXGI_FORMAT_R32_UINT, 0);


    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof( Vertex ) * 4;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData; 

    ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
    vertexBufferData.pSysMem = v;
    dev->CreateBuffer( &vertexBufferDesc, &vertexBufferData, &squareVertBuffer);

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

    //Create perframe buffer
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbPerFrame);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;
    cbbd.MiscFlags = 0;

    dev->CreateBuffer(&cbbd, NULL, &cbPerFrameBuffer);

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
    camPosition = XMVectorSet( 0.0f, 3.0f, -8.0f, 0.0f );
    camTarget = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
    camUp = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );

    camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );

    camProjection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, (float)SCREEN_WIDTH/SCREEN_HEIGHT, 1.0f, 1000.0f);

    //Load the texture
    // D3DX11CreateShaderResourceViewFromFile( dev, L"braynzar.jpg", NULL, NULL, &CubesTexture, NULL );
    // LoadFromWICFile(L"resc/monkey.jpg",NULL, );
    // CreateWICTextureFromFile(dev, devcon, L"resc/metalpanel.jpg", NULL, &CubesTexture);
    // CreateShaderResourceView();
    if(!LoadObjModel(L"resc/spaceCompound.obj", &meshVertBuff, &meshIndexBuff, meshSubsetIndexStart, meshSubsetTexture, material, meshSubsets, true, false)) {
        return;
    }
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

    
}

bool InitDirectInput(HINSTANCE hInstance, HWND hwnd) {
    DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DirectInput, NULL);

    DirectInput->CreateDevice(GUID_SysKeyboard, &DIKeyboard, NULL);

    DirectInput->CreateDevice(GUID_SysMouse, &DIMouse, NULL);

    DIKeyboard->SetDataFormat(&c_dfDIKeyboard);
    DIKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

    DIMouse->SetDataFormat(&c_dfDIMouse);
    DIMouse->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);

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

    XMMATRIX RotateYTempMatrix;
    RotateYTempMatrix = XMMatrixRotationY(camYaw);

    camRight = XMVector3TransformCoord(DefaultRight, RotateYTempMatrix);
    camUp = XMVector3TransformCoord(camUp, RotateYTempMatrix);
    camForward = XMVector3TransformCoord(DefaultForward, RotateYTempMatrix);

    camPosition += moveLeftRight*camRight;
    camPosition += moveBackForward*camForward;

    moveLeftRight = 0.0f;
    moveBackForward = 0.0f;

    camTarget = camPosition + camTarget;    

    camView = XMMatrixLookAtLH( camPosition, camTarget, camUp );
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
    bool computeNormals)
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
    int matCount = material.size();    //total materials

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
                                        //Tutorial Version
                                        // hr = D3DX11CreateShaderResourceViewFromFile( dev, fileNamePath.c_str(),
                                        //     NULL, NULL, &tempMeshSRV, NULL );
                                        //My Version
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
    }

    //////////////////////Compute Normals///////////////////////////
    //If computeNormals was set to true then we will create our own
    //normals, if it was set to false we will use the obj files normals
    if(computeNormals)
    {
        std::vector<XMFLOAT3> tempNormal;

        //normalized and unnormalized normals
        XMFLOAT3 unnormalized = XMFLOAT3(0.0f, 0.0f, 0.0f);

        //Used to get vectors (sides) from the position of the verts
        float vecX, vecY, vecZ;

        //Two edges of our triangle
        XMVECTOR edge1 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        XMVECTOR edge2 = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

        //Compute face normals
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
            tempNormal.push_back(unnormalized);            //Save unormalized normal (for normal averaging)
        }

        //Compute vertex normals (normal Averaging)
        XMVECTOR normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
        int facesUsing = 0;
        float tX;
        float tY;
        float tZ;

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
                    facesUsing++;
                }
            }

            //Get the actual normal by dividing the normalSum by the number of faces sharing the vertex
            //Original version from guide
            // normalSum = normalSum / facesUsing;
            //My version
            normalSum = DirectX::XMVectorScale(normalSum, 1/float(facesUsing));

            //Normalize the normalSum vector
            normalSum = DirectX::XMVector3Normalize(normalSum);

            //Store the normal in our current vertex
            vertices[i].normal.x = XMVectorGetX(normalSum);
            vertices[i].normal.y = XMVectorGetY(normalSum);
            vertices[i].normal.z = XMVectorGetZ(normalSum);

            //Clear normalSum and facesUsing for next vertex
            normalSum = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
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

