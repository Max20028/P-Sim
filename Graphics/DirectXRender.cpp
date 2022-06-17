#include "Renderer.h"
#include <vector>
#include <iostream>

//Function Prototypes
bool InitDirectInput(HINSTANCE hInstance, HWND hwnd);

//Structures
HRESULT hr;

//This is the function that starts all the 3d stuff
void Renderer::InitRenderer(HWND hwnd, HINSTANCE hInstance) {
    // hwnd = hwn;
    // hInstance = hInstanc;
    InitD3D(hwnd, hInstance);

    Light light;

    light.pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    light.range = 100.0f;
    light.att = DirectX::XMFLOAT3(0.0f, 0.2f, 0.0f);
    light.ambient = DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
    light.diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    constbuffPerFrame.light = light;
}

void Renderer::EndRenderer(HWND hwnd) {
    CleanD3D(hwnd);
}

//TODO: FIGURE THIS SHIT OUT

void Renderer::startRenderFrame(CameraDetails camDets) {
    devcon->UpdateSubresource( cbPerFrameBuffer, 0, NULL, &constbuffPerFrame, 0, 0 );
    devcon->PSSetConstantBuffers(0, 1, &cbPerFrameBuffer); 

    //Reset the V and P Shaders
    devcon->VSSetShader(pVS, 0, 0);
    devcon->PSSetShader(pPS, 0, 0);

    devcon->ClearRenderTargetView(backbuffer, camDets.backColor);
    devcon->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

    devcon->OMSetBlendState(0, 0, 0xffffffff);

    //Setup camera
    DirectX::XMVECTOR camPosition = DirectX::XMVectorSet(camDets.position[0], camDets.position[1], camDets.position[2], 0.0f);
    
    DirectX::XMVECTOR camTarget = DirectX::XMVectorSet( camDets.camTarget[0], camDets.camTarget[1], camDets.camTarget[2], 0.0f );
    DirectX::XMVECTOR camUp = DirectX::XMVectorSet( camDets.camUp[0], camDets.camUp[1], camDets.camUp[2], 0.0f );

    camView = DirectX::XMMatrixLookAtLH( camPosition, camTarget, camUp );
    //The prior VertFOV was 0.4f*3.14f, prior near and far were 1.0f and 1000.0f
    camProjection = DirectX::XMMatrixPerspectiveFovLH( camDets.verticalFOV, (float)ClientWidth/ClientHeight, camDets.nearPlaneDist, camDets.farPlaneDist);
}

void Renderer::renderObject(Renderable renderable) {
    //Set the grounds index buffer
    devcon->IASetIndexBuffer( renderable.IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    //Set the grounds vertex buffer
    devcon->IASetVertexBuffers( 0, 1, &renderable.VertBuffer, &renderable.stride, &renderable.offset );
    //Set the WVP matrix and send it to the constant buffer in effect file
    DirectX::XMMATRIX WVP = renderable.World * camView * camProjection;
    cbPerObj.WVP = XMMatrixTranspose(WVP);    
    cbPerObj.World = XMMatrixTranspose(renderable.World);    
    cbPerObj.difColor = renderable.difColor;
    cbPerObj.hasTexture = false;
    cbPerObj.hasNormMap = false;
    devcon->UpdateSubresource( cbPerObjectBuffer, 0, NULL, &cbPerObj, 0, 0 );
    devcon->VSSetConstantBuffers( 0, 1, &cbPerObjectBuffer );
    devcon->PSSetConstantBuffers( 1, 1, &cbPerObjectBuffer );
    // if(bottlematerial[bottleSubsetTexture[i]].hasTexture)
    //     devcon->PSSetShaderResources( 0, 1, &meshSRV[bottlematerial[bottleSubsetTexture[i]].texArrayIndex] );
    // if(bottlematerial[bottleSubsetTexture[i]].hasNormMap)
    //     devcon->PSSetShaderResources( 1, 1, &meshSRV[bottlematerial[bottleSubsetTexture[i]].normMapTexArrayIndex] );
    devcon->PSSetSamplers( 0, 1, &TexSamplerState );
    devcon->RSSetState(NoCullMode);
    int indexStart = 0;
    int indexDrawAmount =  renderable.indexLength;
    // if(!bottlematerial[bottleSubsetTexture[i]].transparent)
    devcon->DrawIndexed( indexDrawAmount, indexStart, 0 );

    // DWORD indi[36];
    // memcpy(indi, renderable.IndexBuffer, sizeof(DWORD) * 36);
    // for(int i = 0; i < 36; i++)
    //     std::cout << indi[i] << " ";
    // std::cout << std::endl;

}

void Renderer::finishRenderFrame(int fps) {
    //Draw all the 2d stuff on top
    drawstuff(L"FPS: ", fps);
    // switch the back buffer and the front buffer
    swapchain->Present(0, 0);
}

void Renderer::createBuffers(int numIndices, int numVerts, std::vector<DWORD> indices, std::vector<Vertex> vertices, ID3D11Buffer** indexBuffer, ID3D11Buffer** vertBuffer) {
    //Init Index Buffer
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * numIndices;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData;

    ZeroMemory( &iinitData, sizeof(iinitData) );
    iinitData.pSysMem = &indices[0];

    hr = dev->CreateBuffer(&indexBufferDesc, NULL, indexBuffer);
    if(!SUCCEEDED(hr))
        std::cout << "Index Failed\n";

    //Init Vert Buffer
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof( Vertex ) * numVerts;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData; 

    ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
    vertexBufferData.pSysMem = &vertices[0];

    hr = dev->CreateBuffer( &vertexBufferDesc, NULL, vertBuffer);
    if(!SUCCEEDED(hr))
        std::cout << "Vertex Failed\n";

    // //Create constant buffer
    // D3D11_BUFFER_DESC cbbd;    
    // ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    // cbbd.Usage = D3D11_USAGE_DEFAULT;
    // cbbd.ByteWidth = sizeof(cbPerObject);
    // cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    // cbbd.CPUAccessFlags = 0;
    // cbbd.MiscFlags = 0;

    // dev->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);
}

void Renderer::createBuffers(int numIndices, int numVerts, DWORD* indices, Vertex* vertices, ID3D11Buffer** indexBuffer, ID3D11Buffer** vertBuffer) {
    //Init Index Buffer
    D3D11_BUFFER_DESC indexBufferDesc;
    ZeroMemory( &indexBufferDesc, sizeof(indexBufferDesc) );

    indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    indexBufferDesc.ByteWidth = sizeof(DWORD) * numIndices;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData;

    ZeroMemory( &iinitData, sizeof(iinitData) );
    iinitData.pSysMem = indices;

    hr = dev->CreateBuffer(&indexBufferDesc, NULL, indexBuffer);
    if(!SUCCEEDED(hr))
        std::cout << "Index Failed\n";

    //Init Vert Buffer
    D3D11_BUFFER_DESC vertexBufferDesc;
    ZeroMemory( &vertexBufferDesc, sizeof(vertexBufferDesc) );

    vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    vertexBufferDesc.ByteWidth = sizeof( Vertex ) * numVerts;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = 0;
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexBufferData; 

    ZeroMemory( &vertexBufferData, sizeof(vertexBufferData) );
    vertexBufferData.pSysMem = vertices;

    hr = dev->CreateBuffer( &vertexBufferDesc, NULL, vertBuffer);
    if(!SUCCEEDED(hr))
        std::cout << "Vertex Failed\n";

    // //Create constant buffer
    // D3D11_BUFFER_DESC cbbd;    
    // ZeroMemory(&cbbd, sizeof(D3D11_BUFFER_DESC));

    // cbbd.Usage = D3D11_USAGE_DEFAULT;
    // cbbd.ByteWidth = sizeof(cbPerObject);
    // cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    // cbbd.CPUAccessFlags = 0;
    // cbbd.MiscFlags = 0;

    // dev->CreateBuffer(&cbbd, NULL, &cbPerObjectBuffer);
}


// this function initializes and prepares Direct3D for use
void Renderer::InitD3D(HWND hwnd, HINSTANCE hInstance)
{
    printf("InitD3D\n");

    //Describe our SwapChain Buffer
    DXGI_MODE_DESC bufferDesc;

    ZeroMemory(&bufferDesc, sizeof(DXGI_MODE_DESC));

    bufferDesc.Width = INITIAL_SCREEN_WIDTH;
    bufferDesc.Height = INITIAL_SCREEN_HEIGHT;
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
    scd.OutputWindow = hwnd;                                // the window to be used
    scd.SampleDesc.Count = 1;                               // how many multisamples
    scd.SampleDesc.Quality = 0;
    scd.Windowed = INITIALLY_WINDOWED;                      // windowed/full-screen mode
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

    // Set the viewport
    D3D11_VIEWPORT viewport;
    ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = INITIAL_SCREEN_WIDTH;
    viewport.Height = INITIAL_SCREEN_HEIGHT;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    devcon->RSSetViewports(1, &viewport);

    // set the render target as the back buffer
    devcon->OMSetRenderTargets(1, &backbuffer, depthStencilView);

    //Init Direct Input
    if(!InitDirectInput(hInstance, hwnd)) {
        MessageBox(0, L"Direct Input Initialization - Failed",L"Error", MB_OK);
        CleanD3D(hwnd);
        return;
    }
}

bool Renderer::InitDirectInput(HINSTANCE hInstance, HWND hwnd) {
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

// this is the function that cleans up Direct3D and COM
void Renderer::CleanD3D(HWND hwnd) {
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
    depthStencilView->Release();
    depthStencilBuffer->Release();
    cbPerObjectBuffer->Release();
    WireFrame->Release();
    Transparency->Release();
    CCWcullMode->Release();
    CWcullMode->Release();
    NoCullMode->Release();
    d2dfactory->Release();
    d2drendertarget->Release();
    dwritefactory->Release();
    dwritetextformat->Release();
    blackbrush->Release();
    cbPerFrameBuffer->Release();

    DIKeyboard->Unacquire();
    DIMouse->Unacquire();
    DirectInput->Release();

}

void Renderer::InitPipeline()
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
    depthStencilDesc.Width     = INITIAL_SCREEN_WIDTH;
    depthStencilDesc.Height    = INITIAL_SCREEN_HEIGHT;
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

    //From here this was previously in InitPipeline

    #ifdef WIREFRAME
    //Rasterizer Stage
    D3D11_RASTERIZER_DESC wfdesc;
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    wfdesc.CullMode = D3D11_CULL_NONE;
    dev->CreateRasterizerState(&wfdesc, &WireFrame);
    #endif

    devcon->RSSetState(WireFrame);


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

    //Create Sampler State
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory( &sampDesc, sizeof(sampDesc) );
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    dev->CreateSamplerState( &sampDesc, &TexSamplerState);
}

void Renderer::drawstuff(std::wstring text, int inInt) {
        static const WCHAR sc_helloWorld[] = L"Hello, World!";


        std::wostringstream printString;
        printString << text << inInt << "\n";
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

void Renderer::DetectInput(double time, HWND hwnd)
{
    DIMOUSESTATE mouseCurrState;

    BYTE keyboardState[256];

    DIKeyboard->Acquire();
    DIMouse->Acquire();

    DIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrState);

    DIKeyboard->GetDeviceState(sizeof(keyboardState),(LPVOID)&keyboardState);

    if(keyboardState[DIK_ESCAPE] & 0x80)
        PostMessage(hwnd, WM_DESTROY, 0, 0);

    if(keyboardState[DIK_A] & 0x80)
    {
    }
    if(keyboardState[DIK_D] & 0x80)
    {
    }
    if(keyboardState[DIK_W] & 0x80)
    {
    }
    if(keyboardState[DIK_S] & 0x80)
    {
    }
    if(keyboardState[DIK_1] & 0x80) {
    }
    if(keyboardState[DIK_2] & 0x80) {
    }
    if(keyboardState[DIK_R] & 0X80)
    {
    }

    //Left Mouse Button
    if(mouseCurrState.rgbButtons[0]){ }

    if(!mouseCurrState.rgbButtons[0])
    {
    }


    if((mouseCurrState.lX != mouseLastState.lX) || (mouseCurrState.lY != mouseLastState.lY))
    {
        // camYaw += mouseLastState.lX * 0.001f;

        // camPitch += mouseCurrState.lY * 0.001f;
        // if(abs(camPitch) > 1.57) {
        //     if(camPitch > 0) camPitch = 1.57;
        //     else camPitch = -1.57;
        // }

        // mouseLastState = mouseCurrState;
    }


    return;
}

