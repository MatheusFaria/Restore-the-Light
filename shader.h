#ifndef SHADER_H
#define SHADER_H

/*
* Author: Matheus de Sousa Faria
* CPE 471 - Introduction to Computer Graphics
* Program 3
*/

#include <string>
#include <map>
#include "GLSL.h"

class Shader {
public:
    Shader();
    Shader(std::string _vertex, std::string _fragment);

    GLuint getId();
    GLint getHandle(std::string handle_name);
    void loadHandle(std::string handle_name);

private:
    GLuint id;
    std::string vertexShader, fragmentShader;
    std::map<std::string, GLint> handles;

    void load();
};

#endif