#ifndef UNICODE
#define UNICODE
#endif

#ifdef DEBUG
#define WIREFRAME
#endif


#include <stdio.h>
#include <initguid.h> //Make COM happy with mingw

// include the basic windows header files and the Direct3D header files
#include <windows.h>
#include <windowsx.h>

#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\d3d11.h>
#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\directxmath.h>

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
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

using namespace DirectX;

// global declarations
IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
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


XMMATRIX WVP;
XMMATRIX camView;
XMMATRIX camProjection;

XMVECTOR camPosition;
XMVECTOR camTarget;
XMVECTOR camUp;

XMMATRIX cube1World;
XMMATRIX cube2World;

XMMATRIX Rotation;
XMMATRIX Scale;
XMMATRIX Translation;
float rot = 0.01f;

// function prototypes
void InitD3D(HWND hWnd);     // sets up and initializes Direct3D
void CleanD3D(void);         // closes Direct3D and releases memory
void RenderFrame();
void InitPipeline();
void InitGraphics();
void UpdateScene();

struct Vertex    //Overloaded Vertex Structure
{
    Vertex(){}
    Vertex(float x, float y, float z,
        float u, float v)
        : pos(x,y,z), texcoord(u, v){}

    XMFLOAT3 pos;
    XMFLOAT2 texcoord;
};

struct cbPerObject
{
    XMMATRIX  WVP;
};

cbPerObject cbPerObj;


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

    // Run the message loop.

    printf("Initialization Complete. Starting Message Loop");
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
        UpdateScene();
        RenderFrame();
    }

    //Clean D3d
    CleanD3D();

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


void UpdateScene(){
    //Keep the cubes rotating
    rot += .0005f;
    if(rot > 6.28f)
        rot = 0.0f;

    //Reset cube1World
    cube1World = XMMatrixIdentity();

    //Define cube1's world space matrix
    XMVECTOR rotaxis = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    Rotation = XMMatrixRotationAxis( rotaxis, rot);
    Translation = XMMatrixTranslation( 0.0f, 0.0f, 4.0f );

    //Set cube1's world space using the transformations
    cube1World = Translation * Rotation;

    //Reset cube2World
    cube2World = XMMatrixIdentity();

    //Define cube2's world space matrix
    Rotation = XMMatrixRotationAxis( rotaxis, -rot);
    Scale = XMMatrixScaling( 1.3f, 1.3f, 1.3f );

    //Set cube2's world space matrix
    cube2World = Rotation * Scale;
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
    bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
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
    scd.Windowed = TRUE;                                    // windowed/full-screen mode
    scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;     // allow full-screen switching

    // create a device, device context and swap chain using the information in the scd struct
    D3D11CreateDeviceAndSwapChain(NULL,
                                  D3D_DRIVER_TYPE_HARDWARE,
                                  NULL,
                                  0,
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
    pBackBuffer->Release();

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
void CleanD3D() {
    printf("Cleanup D3D\n");
    swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

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
}

// this is the function used to render a single frame
void RenderFrame(void) {
    // clear the back buffer to a deep blue
    // devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f)); //This is the tutorial version, requires d3dx
    // float color[4] = {0.0f, 0.2f, 0.4f, 1.0f};
    float color[4] = {0.5f,0.5f,0.5f,1.0f};
    devcon->ClearRenderTargetView(backbuffer, color);
    devcon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

    float blendFactor[] = {0.75f, 0.75f, 0.75f, 1.0f};

    devcon->OMSetBlendState(0, 0, 0xffffffff);

    //Render opaque objects here//

    //Now for transparent objects

    devcon->OMSetBlendState(Transparency, blendFactor, 0xffffffff);

    //Figure out which cube is further to render first, as we want to blend the closer on top of the further one

    XMVECTOR cubePos = DirectX::XMVectorZero();

    cubePos = DirectX::XMVector3TransformCoord(cubePos, cube1World);

    float distX = DirectX::XMVectorGetX(cubePos) - DirectX::XMVectorGetX(camPosition);
    float distY = DirectX::XMVectorGetY(cubePos) - DirectX::XMVectorGetY(camPosition);
    float distZ = DirectX::XMVectorGetZ(cubePos) - DirectX::XMVectorGetZ(camPosition);

    float cube1Dist = distX*distX + distY*distY + distZ*distZ;

    cubePos = DirectX::XMVectorZero();

    cubePos = DirectX::XMVector3TransformCoord(cubePos, cube2World);

    distX = DirectX::XMVectorGetX(cubePos) - DirectX::XMVectorGetX(camPosition);
    distY = DirectX::XMVectorGetY(cubePos) - DirectX::XMVectorGetY(camPosition);
    distZ = DirectX::XMVectorGetZ(cubePos) - DirectX::XMVectorGetZ(camPosition);

    float cube2Dist = distX*distX + distY*distY + distZ*distZ;

    if(cube1Dist < cube2Dist)
    {
        XMMATRIX tempMatrix = cube1World;
        cube1World = cube2World;
        cube2World = tempMatrix;
    }

    //Setup view

        //Set the WVP matrix and send it to the constant buffer in effect file
        WVP = cube1World * camView * camProjection;
        cbPerObj.WVP = XMMatrixTranspose(WVP);    
        devcon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
        devcon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );

        devcon->PSSetShaderResources( 0, 1, &CubesTexture );
        devcon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

        //Counter clockwise culling first because we need the back side of
        //the cube to be rendered first, so the front side can blend with it
        devcon->RSSetState(CCWcullMode);
        //Draw the first cube
        devcon->DrawIndexed( 36, 0, 0 );
        //Now the other side
        devcon->RSSetState(CWcullMode);
        devcon->DrawIndexed( 36, 0, 0 );

        // printf("Draw\n");
        WVP = cube2World * camView * camProjection;
        cbPerObj.WVP = XMMatrixTranspose(WVP);    
        devcon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
        devcon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );

        devcon->PSSetShaderResources( 0, 1, &CubesTexture );
        devcon->PSSetSamplers( 0, 1, &CubesTexSamplerState );

        //Draw the second cube
        devcon->RSSetState(CCWcullMode);
        devcon->DrawIndexed( 36, 0, 0 );
        devcon->RSSetState(CWcullMode);
        devcon->DrawIndexed( 36, 0, 0 );


    // switch the back buffer and the front buffer
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


    // encapsulate both shaders into shader objects
    dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
    dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);


    // set the shader objects
    devcon->VSSetShader(pVS, 0, 0);
    devcon->PSSetShader(pPS, 0, 0);
    


    // create the input layout object
    D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
    devcon->IASetInputLayout(pLayout);

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
    rtbd.SrcBlend                 = D3D11_BLEND_SRC_COLOR;
    rtbd.DestBlend                 = D3D11_BLEND_BLEND_FACTOR;
    rtbd.BlendOp                 = D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha             = D3D11_BLEND_ONE;
    rtbd.DestBlendAlpha             = D3D11_BLEND_ZERO;
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
}

void InitGraphics() {
    printf("InitGraphics\n");
    //The Triangle
    // VERTEX triangle[] = {
    //     {0.0f, 0.5f, 0.0f, {1,0,0,1}},
    //     {0.45f, -0.5f, 0.0f, {0,1,0,1}},
    //     {-0.45f, -0.5f, 0.0f, {0,0,1,1}}
    // };
    // The Square
    // VERTEX v[] = {
    //     { -0.5f, -0.5f, 0.5f, {1.0f, 0.0f, 0.0f, 1.0f} },
    //     { -0.5f,  0.5f, 0.5f, {0.0f, 1.0f, 0.0f, 1.0f} },
    //     {  0.5f,  0.5f, 0.5f, {0.0f, 0.0f, 1.0f, 1.0f} },
    //     {  0.5f, -0.5f, 0.5f, {0.0f, 1.0f, 0.0f, 1.0f} },
    // };
    // DWORD indices[] = {
    //     0, 1, 2,
    //     0, 2, 3,
    // };

    // CUBES
    Vertex v[] =
        {
            // Front Face
            Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
            Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
            Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
            Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
    
            // Back Face
            Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
            Vertex( 1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
            Vertex( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
            Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),
    
            // Top Face
            Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
            Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f),
            Vertex( 1.0f, 1.0f,  1.0f, 1.0f, 0.0f),
            Vertex( 1.0f, 1.0f, -1.0f, 1.0f, 1.0f),
    
            // Bottom Face
            Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
            Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
            Vertex( 1.0f, -1.0f,  1.0f, 0.0f, 0.0f),
            Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f),
    
            // Left Face
            Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
            Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),
            Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
            Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
    
            // Right Face
            Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
            Vertex( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
            Vertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f),
            Vertex( 1.0f, -1.0f,  1.0f, 1.0f, 1.0f),
        };
    
        DWORD indices[] = {
            // Front Face
            0,  1,  2,
            0,  2,  3,
    
            // Back Face
            4,  5,  6,
            4,  6,  7,
    
            // Top Face
            8,  9, 10,
            8, 10, 11,
    
            // Bottom Face
            12, 13, 14,
            12, 14, 15,
    
            // Left Face
            16, 17, 18,
            16, 18, 19,
    
            // Right Face
            20, 21, 22,
            20, 22, 23
        };

    //Square Index Buffer
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3;
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
    vertexBufferDesc.ByteWidth = sizeof( Vertex ) * 24;
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
    devcon->IASetVertexBuffers( 0, 1, &squareVertBuffer, &stride, &offset );


    //Create constant buffer
    D3D11_BUFFER_DESC cbbd;    
    ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    cbbd.Usage = D3D11_USAGE_DEFAULT;
    cbbd.ByteWidth = sizeof(cbPerObject);
    cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbbd.CPUAccessFlags = 0;
    cbbd.MiscFlags = 0;

    dev->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);

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
    CreateWICTextureFromFile(dev, devcon, L"resc/monkey.jpg", NULL, &CubesTexture);
    // CreateShaderResourceView();

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

