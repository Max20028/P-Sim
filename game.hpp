#include "Graphics/Renderer.h"
#include "Core/RobotMap.h"
#include <C:\Program Files (x86)\Windows Kits\10\Include\10.0.20348.0\um\directxmath.h>
#include <iostream>
#include <fstream>

HWND hwnd;

Renderer* renderer;

float rot = 0;




struct SimpleModel {
    ID3D11Buffer* indexBuffer;
    ID3D11Buffer* vertBuffer;
};

Renderer::CameraDetails camdets;
Renderable cubeRender;


void ImportObj(std::string filepath, Vertex** vert, DWORD** ind, int* nvert, int* nind);

void updateGame(float dt, int fps) {
    //Run update code here
    renderer->startRenderFrame(camdets);
    renderer->renderObject(cubeRender);
    renderer->finishRenderFrame(fps);

    rot += 0.0005f * dt;
    if(rot > 6.28f)
        rot = 0.0f;

    DirectX::XMVECTOR rotaxis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(rotaxis, rot);
    cubeRender.World = DirectX::XMMatrixTranslation( 0.0f, 0.0f, 4.0f) * Rotation;
}

void startGame(HWND hwn, HINSTANCE hInstance) {
    renderer = new Renderer();
    hwnd = hwn;
    renderer->InitRenderer(hwnd, hInstance);

    SimpleModel monkey;

    DWORD* indices;
    Vertex* verts;

    int numberInd;
    int numberVert;

    ImportObj("resc/misc/monkey.obj", &verts, &indices, &numberVert, &numberInd);

    renderer->createBuffers(numberInd, numberVert, indices, verts, &monkey.indexBuffer, &monkey.vertBuffer);


    camdets.backColor[0] = 0.2f;
    camdets.backColor[1] = 0.2f;
    camdets.backColor[2] = 0.2f;
    camdets.backColor[3] = 1.0f;

    camdets.camTarget[0] = 0.0f;
    camdets.camTarget[1] = 0.0f;
    camdets.camTarget[2] = 0.0f;

    camdets.camUp[0] = 0.0f;
    camdets.camUp[1] = 1.0f;
    camdets.camUp[2] = 0.0f;

    camdets.farPlaneDist = 1000;
    camdets.nearPlaneDist = 1;

    camdets.position[0] = 0;
    camdets.position[1] = 3;
    camdets.position[2] = -8;

    camdets.verticalFOV = 0.4*3.14;


    cubeRender.difColor = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

    cubeRender.IndexBuffer = monkey.indexBuffer;
    cubeRender.VertBuffer = monkey.vertBuffer;

    cubeRender.offset = 0;
    cubeRender.stride = sizeof(Vertex);

    cubeRender.indexLength = numberInd;
    DirectX::XMVECTOR rotaxis = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(rotaxis, rot);
    cubeRender.World = DirectX::XMMatrixTranslation( 0.0f, 0.0f, 4.0f) * Rotation;
}

void endGame(HWND hwnd) {
    renderer->EndRenderer(hwnd);
}

void ImportObj(std::string filepath, Vertex** vert, DWORD** ind, int* nvert, int* nind) {
    std::ifstream file;
    file.open(filepath);

    std::vector<Vertex> vertvec;
    std::vector<DWORD> indvec;

    while(true){
        std::string lines;
        std::getline(file, lines);
        if(file.eof()) break;
        char line[lines.length()+1];
        strcpy(line, lines.c_str());
        // Returns first token 
        char *token = strtok(line, " ");
        // std::vector<XMFLOAT3> norms;
        if(strcmp(token, "v") == 0) {
            //If this is a vertex, add to vertex array
            token = strtok(NULL, " ");
            float a = atof(token);
            token = strtok(NULL, " ");
            float b = atof(token);
            token = strtok(NULL, "\n");
            float c = atof(token);
            // Vertex ver = Vertex(a,b,c,1.0f,1.0f,1.0f,1.0f);
            vertvec.push_back(Vertex(a,b,c));
        } else if(strcmp(token, "f") == 0) {
            token = strtok(NULL, " ");
            int a = atoi(token);
            token = strtok(NULL, " ");
            int b = atoi(token);
            token = strtok(NULL, "\n");
            int c = atoi(token);
            indvec.push_back(a-1);
            indvec.push_back(b-1);
            indvec.push_back(c-1);
        } else if(strcmp(token, "vn") == 0) {
            token = strtok(NULL, " ");
            int a = atoi(token);
            token = strtok(NULL, " ");
            int b = atoi(token);
            token = strtok(NULL, "\n");
            int c = atoi(token);
            // norms.push_back(XMFLOAT3(a,b,c));
        }
    }
    *nvert = vertvec.size();
    *nind = indvec.size();

    *vert = new Vertex[*nvert];
    std::copy(vertvec.begin(), vertvec.end(), *vert);
    *ind = new DWORD[*nind];
    std::copy(indvec.begin(), indvec.end(), *ind);
    file.close();
}

