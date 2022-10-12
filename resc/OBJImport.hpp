#include <string>
#include <fstream>
#include "Graphics/Renderer.h"

struct vertexdata {
    int v;
    int t;
    int n;
};

void OldImportObj(std::string filepath, Vertex** vert, DWORD** ind, int* nvert, int* nind) {
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
            float a = atof(token);
            token = strtok(NULL, " ");
            float b = atof(token);
            token = strtok(NULL, "\n");
            float c = atof(token);
            // norms.push_back(DirectX::XMFLOAT3(a,b,c));
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

// THIS WILL NOT WORK FOR CONCAVE FACES
void ImportObj(std::string filepath, Vertex** vert, DWORD** ind, int* nvert, int* nind) {
    std::ifstream file;
    file.open(filepath);

    std::vector<Vertex> vertvec;
    std::vector<DWORD> indvec;
    std::vector<DirectX::XMFLOAT3> norms;
    std::vector<DirectX::XMFLOAT2> tex;
    std::vector<std::vector<vertexdata>> faces;

    while(true){
        std::string line;
        std::getline(file, line);
        if(file.eof()) break;
        int pos = line.find(' ');
        int lastpos = 0;

        std::string tp = line.substr(0,pos);
        if(tp == "v") {
            //Vertex Positions
            lastpos = pos;
            pos = line.find(' ', lastpos+1);
            float x = std::stof(line.substr(lastpos, pos-lastpos));
            lastpos = pos;
            pos = line.find(' ', lastpos+1);
            float y = std::stof(line.substr(lastpos, pos-lastpos));
            lastpos = pos;
            pos = line.find(' ', lastpos+1);
            float z = std::stof(line.substr(lastpos, pos-lastpos));
            vertvec.push_back(Vertex(x,y,z));
        } else if(tp == "vn") {
            //Vertex Normals
            lastpos = pos;
            pos = line.find(' ', lastpos+1);
            float xn = std::stof(line.substr(lastpos, pos-lastpos));
            lastpos = pos;
            pos = line.find(' ', lastpos+1);
            float yn = std::stof(line.substr(lastpos, pos-lastpos));
            lastpos = pos;
            pos = line.find(' ', lastpos+1);
            float zn = std::stof(line.substr(lastpos, pos-lastpos));
            norms.push_back(DirectX::XMFLOAT3(xn,yn,zn));
        } else if(tp == "vt") {
            //Vertex Texture Coordinates
            lastpos = pos;
            pos = line.find(' ', lastpos+1);
            float u = std::stof(line.substr(lastpos, pos-lastpos));
            lastpos = pos;
            pos = line.find(' ', lastpos+1);
            float v = std::stof(line.substr(lastpos, pos-lastpos));
            tex.push_back(DirectX::XMFLOAT2(u,v));
        } else if(tp == "f") {
            //Faces
            //This uses a triangulation algorithm for faces with more than 3 vertices
            //Get an array of each vertex
            std::vector<vertexdata> tempvert;
            //Load all the vertices in the face into a vector
            line.erase(0,2);
            while(line != "") {
                std::string vstr = line.substr(0, line.find(' ', 2));
                if(vstr.find('/') == std::string::npos) {
                    //If there is no '/' then this vertex only consists of a vertex. No tex or norm
                    int vi = std::stoi(vstr);
                    tempvert.push_back({vi, 0, 0});
                    continue;
                }
                //Otherwise, continue the work
                std::string vis = vstr.substr(0, vstr.find('/'));
                int vi = std::stoi(vis);
                vstr.erase(0, vstr.find('/') + 1);

                if(vstr.find('/') == std::string::npos) {
                    //If there is no '/' this means there is no normal data
                    int vt = std::stoi(vstr);
                    tempvert.push_back({vi, vt, 0});
                    continue;
                }
                //Otherwise, continue the work
                std::string vts = vstr.substr(0, vstr.find('/'));
                int vt;
                if(vts == "") {
                    //If this one is empty, then there is no tex for this vertex
                    vt = 0;
                } else {
                    vt = stoi(vts);
                }
                vstr.erase(0, vstr.find('/')+1);

                //Now all that should be left is the normal data
                int vn = stoi(vstr);
                tempvert.push_back({vi,vt,vn});
                int fg = line.find(' ');
                if(fg == std::string::npos)
                    line.erase(0,line.length());
                else
                    line.erase(0, line.find(' ')+1);
            }
            faces.push_back(tempvert);
        }
    }

    //Process each triangulated face
    for(int j = 0; j < faces.size(); j++) {
    std::vector<vertexdata> tempvert = faces[j];
    for(int i = 0; i < tempvert.size()-2; i++) {
        int a = 0;
        int b = i+1;
        int c = i+2;
        //Construct the indices
        indvec.push_back(tempvert[a].v-1);
        indvec.push_back(tempvert[b].v-1);
        indvec.push_back(tempvert[c].v-1);

        //Construct the Tex
        if(tempvert[a].t != 0)
            vertvec[tempvert[a].v-1].texcoord = tex[tempvert[a].t-1];
        if(tempvert[b].t != 0)
            vertvec[tempvert[b].v-1].texcoord = tex[tempvert[b].t-1];
        if(tempvert[c].t != 0)
            vertvec[tempvert[c].v-1].texcoord = tex[tempvert[c].t-1];

        //Construct the normals
        if(tempvert[a].n != 0)
            vertvec[tempvert[a].v-1].normal = norms[tempvert[a].n-1];
        if(tempvert[b].n != 0)
            vertvec[tempvert[b].v-1].normal = norms[tempvert[b].n-1];
        if(tempvert[c].n != 0)
            vertvec[tempvert[c].v-1].normal = norms[tempvert[c].n-1];
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
