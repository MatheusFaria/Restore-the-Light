#ifndef MESH_H
#define MESH_H

/*
* Author: Matheus de Sousa Faria
* CPE 471 - Introduction to Computer Graphics
* Program 3
*/

#include <string>
#include <vector>

#include "tiny_obj_loader.h"

class Mesh{
public:
    Mesh();
    Mesh(std::string _filename);

    void init();

    std::vector<float> getNormals();
    std::vector<float> getVertices();
    std::vector<unsigned int> getIndices();
    std::string getFileName();

private:
    std::string filename;
    std::vector<float> normals;

    std::vector<tinyobj::shape_t> shape;
    std::vector<tinyobj::material_t> materials;

    void calculateNormals();
    void resizeObj();
    void loadShapes();
};

#endif