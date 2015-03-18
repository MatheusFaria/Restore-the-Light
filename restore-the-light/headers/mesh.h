#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_PURE
#include "glm/glm.hpp"

#include "tiny_obj_loader.h"

class Mesh{
public:
    Mesh();
    Mesh(std::string _filename);

    void init(bool enableResize = true);

    std::vector<float> getNormals();
    std::vector<float> getVertices();
    std::vector<unsigned int> getIndices();
    std::string getFileName();
    std::vector<float> getTexCoords();
    glm::vec3 getMinPoint();
    glm::vec3 getMaxPoint();

private:
    std::string filename;
    std::vector<float> normals;

    std::vector<tinyobj::shape_t> shape;
    std::vector<tinyobj::material_t> materials;

    glm::vec3 min, max;

    void calculateNormals();
    void resizeObj();
    void loadShapes();
    void calculateBoundingBox();
};

#endif