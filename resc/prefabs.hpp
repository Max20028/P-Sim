#include "Core/Scene.hpp"
#include "Core/RobotMap.h"
#include "resc/Components/CoreComponents.h"
#include "resc/Components/MovementComponents.h"
#include "Graphics/Renderer.h"
#include <fstream>
#include <string>

extern Scene scene;
extern Renderer* renderer;

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
                // char* ptok = strtok(token, "//");
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

Entity createModelPrefab(std::string path) {
    Entity entity = scene.CreateEntity();
    //Add transform component
    scene.AddComponent(entity, Transform{
                                .translation={0.0f, 8.0f, 4.0f},
                                .rotation = {0.0f, 0.0f, 0.0f},
                                .scale = {1.0f, 1.0f, 1.0f}
                                });
    //Add Mesh Component
    SimpleMesh mesh;

    DWORD* indices;
    Vertex* verts;

    int numberInd;
    int numberVert;

    ImportObj(path, &verts, &indices, &numberVert, &numberInd);

    renderer->createBuffers(numberInd, numberVert, indices, verts, &mesh.indexBuffer, &mesh.vertBuffer);

    mesh.offset = 0;
    mesh.stride = sizeof(Vertex);
    mesh.indexLength = numberInd;

    scene.AddComponent(entity, mesh);

    //Add Rigidbody component
    scene.AddComponent(entity, Rigidbody{
                                    .isGravity = true,
                                    .isKinematic = true,
                                    .mass = 1.0f,
                                    .gravity = -1.0f
                                    });
    //Add SPIN
    scene.AddComponent(entity, Spin{.spinspeed=2.5});
    return entity;
}

Entity createCameraPrefab() {
    Entity entity = scene.CreateEntity();
    //Add transform component
    scene.AddComponent(entity, Transform{
                                .translation={0.0f, 3.0f, -8.0f},
                                .rotation = {0.0f, 0.0f, 0.0f},
                                .scale = {1.0f, 1.0f, 0.0f}
                                });
    //Add camera component
    //TODO: HORIZONTAL FOV MEANS NOTHING
    scene.AddComponent(entity, Camera{
                                .isMainCamera = true,
                                .VerticalFOV = 0.4*3.14,
                                .HorizontalFOV = 1,
                                .clearColor = {0.2f, 0.2f, 0.2f, 2.0f},
                                .nearPlaneDist = 0.1f,
                                .farPlaneDist = 1000.0f
                                });
    return entity;
}