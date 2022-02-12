#ifndef UNICODE
#define UNICODE
#endif

#include <stdio.h>

// include the basic windows header files and the Direct3D header files
#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <directxmath.h>
// #include <d3dx11.h>
// #include <d3dx10.h>
#include <d3dcompiler.h>    //Shader compiler

// include the Direct3D Library file
// So, since I am using g++, i cant use these auto linkers. These libraries must be specified through the command line,
//  and thus through the Makefile. See the LIBFLAGS for the format, but essentialy -L adds a search directory and -l adds the lib
//      #pragma comment (lib, "C:\\msys64\\mingw64\\x86_64-w64-mingw32\\lib\\d3d11.a")
//      #pragma comment(linker, "C:\\msys64\\mingw64\\x86_64-w64-mingw32\\lib\\d3d11.a -verbose")
//      #pragma comment (lib, "d3dx11.lib")
//      #pragma comment (lib, "d3dx10.lib")

// define the screen resolution
#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

// global declarations
IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;    // global declaration
ID3D11VertexShader *pVS;    // the vertex shader
ID3D11PixelShader *pPS;     // the pixel shader
ID3D11Buffer *pVBuffer;     // GPU input buffer
ID3D11InputLayout *pLayout; // GPU Input Layout


// function prototypes
void InitD3D(HWND hWnd);     // sets up and initializes Direct3D
void CleanD3D(void);         // closes Direct3D and releases memory
void RenderFrame();
void InitPipeline();
void InitGraphics();

struct VERTEX {
    FLOAT X,Y,Z; //Position
    float Color[4];
};

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

    //Init directx3d
    InitD3D(hwnd);

    // Run the message loop.

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


// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd)
{
    // create a struct to hold information about the swap chain
    DXGI_SWAP_CHAIN_DESC scd;

    // clear out the struct for use
    ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

    // fill the swap chain description struct
    scd.BufferCount = 1;                                    // one back buffer
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
    scd.BufferDesc.Width = SCREEN_WIDTH;                    // set the back buffer width
    scd.BufferDesc.Height = SCREEN_HEIGHT;                  // set the back buffer height
    scd.OutputWindow = hWnd;                                // the window to be used
    scd.SampleDesc.Count = 4;                               // how many multisamples
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
    swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    // use the back buffer address to create the render target
    dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
    pBackBuffer->Release();

    // set the render target as the back buffer
    devcon->OMSetRenderTargets(1, &backbuffer, NULL);

// -----------------

    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = SCREEN_WIDTH;
    viewport.Height = SCREEN_HEIGHT;

    devcon->RSSetViewports(1, &viewport);

    InitPipeline();
    InitGraphics();
}

// this is the function that cleans up Direct3D and COM
void CleanD3D() {

    swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

    // close and release all existing COM objects
    pLayout->Release();
    pVS->Release();
    pPS->Release();
    pVBuffer->Release();
    swapchain->Release();
    backbuffer->Release();
    dev->Release();
    devcon->Release();
}

// this is the function used to render a single frame
void RenderFrame(void) {
    // clear the back buffer to a deep blue
    // devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f)); //This is the tutorial version, requires d3dx
    float color[4] = {0.0f, 0.2f, 0.4f, 1.0f};
    devcon->ClearRenderTargetView(backbuffer, color);


    // do 3D rendering on the back buffer here
        // select which vertex buffer to display
        UINT stride = sizeof(VERTEX);
        UINT offset = 0;
        devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

        // select which primtive type we are using
        devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // draw the vertex buffer to the back buffer
        devcon->Draw(3, 0);

    // switch the back buffer and the front buffer
    swapchain->Present(0, 0);
}

void InitPipeline()
{
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
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
    devcon->IASetInputLayout(pLayout);
}

void InitGraphics() {
    //The Triangle
    VERTEX triangle[] = {
        {0.0f, 0.5f, 0.0f, {1,0,0,1}},
        {0.45f, -0.5f, 0.0f, {0,1,0,1}},
        {-0.45f, -0.5f, 0.0f, {0,0,1,1}}
    };
    // VERTEX triangle[] = {
    //     {0.0f, 0.5f, 0.0f},
    //     {0.45f, -0.5f, 0.0f},
    //     {-0.45f, -0.5f, 0.0f}
    // };

    //Create Vertex Buffer
    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));

    bd.Usage = D3D11_USAGE_DYNAMIC;             //Write access by CPU and GPU
    bd.ByteWidth = sizeof(triangle);          //size is vertex*3
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;    //Use as a vertex buffer
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //Allow CPU to write in buffer

    dev->CreateBuffer(&bd, NULL, &pVBuffer);

    //Copy vertices into buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    devcon->Map(pVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);    //Map the buffer
    memcpy(ms.pData, triangle, sizeof(triangle));
    devcon->Unmap(pVBuffer, 0);
    
}