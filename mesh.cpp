#include "mesh.h"

/*
* Author: Matheus de Sousa Faria
* CPE 471 - Introduction to Computer Graphics
* Program 3
*/

#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" 

Mesh::Mesh(){}

Mesh::Mesh(std::string _filename): filename(_filename){
    loadShapes();
    normals = std::vector<float>(shape[0].mesh.positions.size(), 0);
}

void Mesh::init(){
    resizeObj();
    calculateNormals();
}

std::vector<float> Mesh::getNormals(){
    return normals;
}

std::vector<float> Mesh::getVertices(){
    return shape[0].mesh.positions;
}

std::vector<unsigned int> Mesh::getIndices(){
    return shape[0].mesh.indices;
}

std::string Mesh::getFileName(){
    return filename;
}

void Mesh::calculateNormals(){
    int idx1, idx2, idx3;
    glm::vec3 v1, v2, v3;

    for (int i = 0; i < shape[0].mesh.indices.size() / 3; i++) {
        idx1 = shape[0].mesh.indices[3 * i + 0];
        idx2 = shape[0].mesh.indices[3 * i + 1];
        idx3 = shape[0].mesh.indices[3 * i + 2];
        v1 = glm::vec3(shape[0].mesh.positions[3 * idx1 + 0], shape[0].mesh.positions[3 * idx1 + 1], shape[0].mesh.positions[3 * idx1 + 2]);
        v2 = glm::vec3(shape[0].mesh.positions[3 * idx2 + 0], shape[0].mesh.positions[3 * idx2 + 1], shape[0].mesh.positions[3 * idx2 + 2]);
        v3 = glm::vec3(shape[0].mesh.positions[3 * idx3 + 0], shape[0].mesh.positions[3 * idx3 + 1], shape[0].mesh.positions[3 * idx3 + 2]);

        glm::vec3 norm = glm::cross((v2 - v1), (v3 - v1));

        normals[3 * idx1 + 0] += norm.x;
        normals[3 * idx1 + 1] += norm.y;
        normals[3 * idx1 + 2] += norm.z;
        normals[3 * idx2 + 0] += norm.x;
        normals[3 * idx2 + 1] += norm.y;
        normals[3 * idx2 + 2] += norm.z;
        normals[3 * idx3 + 0] += norm.x;
        normals[3 * idx3 + 1] += norm.y;
        normals[3 * idx3 + 2] += norm.z;
    }

    for (int i = 0; i < normals.size() / 3; i++) {
        glm::vec3 norm = glm::normalize(glm::vec3(normals[3 * i + 0], normals[3 * i + 1], normals[3 * i + 2]));
        normals[3 * i + 0] += norm.x;
        normals[3 * i + 1] += norm.y;
        normals[3 * i + 2] += norm.z;
    }
}

void Mesh::resizeObj(){
    float minX, minY, minZ;
    float maxX, maxY, maxZ;
    float scaleX, scaleY, scaleZ;
    float shiftX, shiftY, shiftZ;
    float epsilon = 0.001;

    minX = minY = minZ = 1.1754E+38F;
    maxX = maxY = maxZ = -1.1754E+38F;

    //Go through all vertices to determine min and max of each dimension
    for (size_t i = 0; i < shape.size(); i++) {
        for (size_t v = 0; v < shape[i].mesh.positions.size() / 3; v++) {
            if (shape[i].mesh.positions[3 * v + 0] < minX) minX = shape[i].mesh.positions[3 * v + 0];
            if (shape[i].mesh.positions[3 * v + 0] > maxX) maxX = shape[i].mesh.positions[3 * v + 0];

            if (shape[i].mesh.positions[3 * v + 1] < minY) minY = shape[i].mesh.positions[3 * v + 1];
            if (shape[i].mesh.positions[3 * v + 1] > maxY) maxY = shape[i].mesh.positions[3 * v + 1];

            if (shape[i].mesh.positions[3 * v + 2] < minZ) minZ = shape[i].mesh.positions[3 * v + 2];
            if (shape[i].mesh.positions[3 * v + 2] > maxZ) maxZ = shape[i].mesh.positions[3 * v + 2];
        }
    }
    //From min and max compute necessary scale and shift for each dimension
    float maxExtent, xExtent, yExtent, zExtent;
    xExtent = maxX - minX;
    yExtent = maxY - minY;
    zExtent = maxZ - minZ;
    if (xExtent >= yExtent && xExtent >= zExtent) {
        maxExtent = xExtent;
    }
    if (yExtent >= xExtent && yExtent >= zExtent) {
        maxExtent = yExtent;
    }
    if (zExtent >= xExtent && zExtent >= yExtent) {
        maxExtent = zExtent;
    }
    scaleX = 2.0 / maxExtent;
    shiftX = minX + (xExtent / 2.0);
    scaleY = 2.0 / maxExtent;
    shiftY = minY + (yExtent / 2.0);
    scaleZ = 2.0 / maxExtent;
    shiftZ = minZ + (zExtent) / 2.0;

    //Go through all verticies shift and scale them
    for (size_t i = 0; i < shape.size(); i++) {
        for (size_t v = 0; v < shape[i].mesh.positions.size() / 3; v++) {
            shape[i].mesh.positions[3 * v + 0] = (shape[i].mesh.positions[3 * v + 0] - shiftX) * scaleX;
            shape[i].mesh.positions[3 * v + 1] = (shape[i].mesh.positions[3 * v + 1] - shiftY) * scaleY;
            shape[i].mesh.positions[3 * v + 2] = (shape[i].mesh.positions[3 * v + 2] - shiftZ) * scaleZ;
        }
    }
}

void Mesh::loadShapes(){
    std::string err = tinyobj::LoadObj(shape, materials, filename.c_str());
    if (!err.empty()) {
        std::cerr << err << std::endl;
    }
}