#include "shader.h"

#include <cctype>
#include <iostream>

#include "globals.h"
#include "GLSL.h"

Shader::Shader(){}

Shader::Shader(std::string _vertex, std::string _fragment) : 
    vertexShader(_vertex), fragmentShader(_fragment){}

GLuint Shader::getId(){
    return id;
}

bool Shader::loadHandle(std::string handle_name){
    if (handles.find(handle_name) == handles.end()){
        switch (tolower(handle_name[0])){
        case 'a':
            handles[handle_name] = glGetAttribLocation(id, handle_name.c_str());
            break;
        case 'u':
            handles[handle_name] = glGetUniformLocation(id, handle_name.c_str());
            break;
        default:
            if (Global::debug){
                std::cout << "Shader::loadHandle" << 
                             "\nYour handle name should start with a or u: " << 
                             handle_name << "\n";
                return false;
            }
        }
        if (handles[handle_name] < 0){
            if (Global::debug)
                std::cout << "Shader::loadHandle" <<
                "\nCould not load handle: " <<
                handle_name << "\n";
            return false;
        }
    }
    return true;
}

GLint Shader::getHandle(std::string handle_name){
    if (handles.find(handle_name) == handles.end()){
        if (Global::debug)
            std::cout << "Shader::getHandle" << "\nHandle not loaded: " << handle_name << "\n";
        return -1;
    }
    return handles[handle_name];
}

bool Shader::load()
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
    if (Global::debug)
        std::cout << vertexShader << "\n";
    glCompileShader(VS);
    if (Global::debug)
        GLSL::printError();
    glGetShaderiv(VS, GL_COMPILE_STATUS, &rc);
    if (Global::debug)
        GLSL::printShaderInfoLog(VS);
    if (!rc) {
        if (Global::debug)
            std::cout << "Shader::load" << "\nError compiling vertex shader\n";
        return false;
    }

    // Compile fragment shader
    if (Global::debug)
        std::cout << fragmentShader << "\n";
    glCompileShader(FS);
    if (Global::debug)
        GLSL::printError();
    glGetShaderiv(FS, GL_COMPILE_STATUS, &rc);
    if (Global::debug)
        GLSL::printShaderInfoLog(FS);
    if (!rc) {
        if (Global::debug)
            std::cout << "Shader::load" << "\nError compiling fragment shader\n";
        return false;
    }

    // Create the program and link
    id = glCreateProgram();
    glAttachShader(id, VS);
    glAttachShader(id, FS);
    glLinkProgram(id);
    if (Global::debug)
        GLSL::printError();
    glGetProgramiv(id, GL_LINK_STATUS, &rc);
    if (Global::debug)
        GLSL::printProgramInfoLog(id);
    if (!rc) {
        if (Global::debug)
            std::cout << "Shader::load" << "\nError linking shaders\n";
        return false;
    }

    return true;
}

