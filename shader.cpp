#include "shader.h"

/*
* Author: Matheus de Sousa Faria
* CPE 471 - Introduction to Computer Graphics
* Program 3
*/

#include <cctype>

#include "log.h"


Shader::Shader(){}

Shader::Shader(std::string _vertex, std::string _fragment) : 
    vertexShader(_vertex), fragmentShader(_fragment){
    load();
}

GLuint Shader::getId(){
    return id;
}

void Shader::loadHandle(std::string handle_name){
    switch (tolower(handle_name[0])){
        case 'a':
            handles[handle_name] = GLSL::getAttribLocation(id, handle_name.c_str());
            break;
        case 'u':
            handles[handle_name] = GLSL::getUniformLocation(id, handle_name.c_str());
            break;
        default:
            Log::error("Shader::loadHandle", "Your handle name should start with a or u",
                "uName or aName", handle_name);
    }
    if (handles[handle_name] < 0)
        Log::error("Shader::loadHandle", "Could not load handle",
        "Existing handle", handle_name);
}

GLint Shader::getHandle(std::string handle_name){
    if (handles.find(handle_name) == handles.end()){
        Log::error("Shader::getHandle", "Handle not loaded",
            "Loaded handle", handle_name);
        return -1;
    }
    return handles[handle_name];
}

void Shader::load()
{
    GLint rc;

    // Create shader handles
    GLuint VS = glCreateShader(GL_VERTEX_SHADER);
    GLuint FS = glCreateShader(GL_FRAGMENT_SHADER);

    // Read shader sources
    const char *vshader = GLSL::textFileRead(vertexShader.c_str());
    const char *fshader = GLSL::textFileRead(fragmentShader.c_str());
    glShaderSource(VS, 1, &vshader, NULL);
    glShaderSource(FS, 1, &fshader, NULL);

    // Compile vertex shader
    Log::out("Vertex ");
    glCompileShader(VS);
    GLSL::printError();
    glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
    GLSL::printShaderInfoLog(VS);
    if (!rc) {
        Log::error("Shader::load", "Error compiling vertex shader",
            "Correct shader", vertexShader);
    }

    // Compile fragment shader
    Log::out("Fragment ");
    glCompileShader(FS);
    GLSL::printError();
    glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
    GLSL::printShaderInfoLog(FS);
    if (!rc) {
        Log::error("Shader::load", "Error compiling fragment shader",
            "Correct shader", fragmentShader);
    }

    // Create the program and link
    id = glCreateProgram();
    glAttachShader(id, VS);
    glAttachShader(id, FS);
    glLinkProgram(id);
    GLSL::printError();
    glGetProgramiv(id, GL_LINK_STATUS, &rc);
    GLSL::printProgramInfoLog(id);
    if (!rc) {
        Log::error("Shader::load", "Error linking shaders",
            "", vertexShader + " " + fragmentShader);
    }
}

