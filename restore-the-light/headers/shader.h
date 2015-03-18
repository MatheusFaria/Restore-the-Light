#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <map>

#include "GLIncludes.h"

class Shader {
public:
    Shader();
    Shader(std::string _vertex, std::string _fragment);

    GLuint getId();
    bool load();

    GLint getHandle(std::string handle_name);
    bool loadHandle(std::string handle_name);

private:
    GLuint id;
    std::string vertexShader, fragmentShader;
    std::map<std::string, GLint> handles;
};

#endif