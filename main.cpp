/*
* Author: Matheus de Sousa Faria
* CPE 471 - Introduction to Computer Graphics
* Program 4
* Code modifed from ZJ Wood
*/

#include <iostream>
#include <cassert>
#include <vector>
#include <ctime>

#include "GLSL.h"
#include "mesh.h"
#include "shader.h"
#include "object3d.h"
#include "load_manager.h"
#include "material.h"
#include "virtual_camera.h"
#include "light.h"
#include "texture.h"
#include "image.h"
#include "fbo.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

GLFWwindow* window;
using namespace std;

int g_width;
int g_height;
bool g_lock_mouse = true;
bool g_first_mouse_movement = true;

class Ball : public Object3D{
public:
    Ball(){}

    void init(){
        mesh = LoadManager::getMesh("sphere-tex.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadTextureBuffer("texBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert.glsl", "frag.glsl");

        Texture * tex = new Texture(LoadManager::getImage("bloomtex.bmp"));
        tex->load();
        TextureManager::addTexture("cubetex", tex);
    }

    void drawObject(){
        enableAttrArray2f("aTexCoord", "texBufObj");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureManager::getTexture("cubetex")->getTexture());
        glUniform1i(shader->getHandle("uTextureID"), 0);

        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        glUniform3f(shader->getHandle("uEye"), CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);
        LightManager::loadLights(shader->getHandle("uLightPos"),
            shader->getHandle("uLightColor"),
            shader->getHandle("uLightFallOff"));

        Material::SetMaterial(Material::EMERALD, shader);
        loadIdentity();

        rot += 0.05f;
        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5)));
        addTransformation(glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 0)));
        bindModelMatrix("uModelMatrix");

        drawElements();
    }
    float rot = 0;
};

class Cube : public Object3D{
public:
    Cube(GLuint _tex): tex(_tex){}

    void init(){
        mesh = LoadManager::getMesh("cube-textures.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadTextureBuffer("texBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert.glsl", "frag.glsl");
    }

    void drawObject(){
        enableAttrArray2f("aTexCoord", "texBufObj");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);
        glUniform1i(shader->getHandle("uTextureID"), 0);

        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        glUniform3f(shader->getHandle("uEye"), CamManager::currentCam()->eye.x,
            CamManager::currentCam()->eye.y,
            CamManager::currentCam()->eye.z);
        LightManager::loadLights(shader->getHandle("uLightPos"),
            shader->getHandle("uLightColor"),
            shader->getHandle("uLightFallOff"));

        Material::SetMaterial(Material::EMERALD, shader);
        loadIdentity();

        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5)));
        addTransformation(glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 0)));
        bindModelMatrix("uModelMatrix");

        drawElements();
    }
    float rot = 45;
    GLuint tex;
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //Move light in X axis
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        g_lock_mouse = !g_lock_mouse;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    /*if (hero){
        if (hero->isFPS()){
            if (g_first_mouse_movement){
                g_first_mouse_movement = false;
                glfwSetCursorPos(window, g_width / 2, g_height / 2);
                return;
            }

            float phi = CamManager::currentCam()->getPhi(), 
                theta = CamManager::currentCam()->getTheta();
            phi += (g_height / 2 - ypos) * 360 / g_height;
            theta -= (g_width / 2 - xpos) * 360 / g_width;

            if (phi > 80)
                phi = 80;
            if (phi < -80)
                phi = -80;

            CamManager::currentCam()->setAngles(theta, phi);

            if (g_lock_mouse){
                glfwSetCursorPos(window, g_width / 2, g_height / 2);
            }
        }
    }*/
}

void setupCams(){
    int numberCameras = 3;
    glm::mat4 Projection = glm::perspective(90.0f, (float)g_width / g_height, 0.1f, 100.f);

    for (int i = 0; i < numberCameras; i++){
        Cam * mainCam = new Cam(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
        CamManager::addCam(mainCam);
        CamManager::getCam(i)->projectionMatrix = Projection;
    }

    CamManager::setCam(0);
}

void setupLights(){
    LightManager::init();
    LightManager::addLight(new Light(glm::vec3(0, 2, 0), glm::vec3(1, 1, 1), 
        glm::vec3(0, 0.03, 0), Light::POINT_LIGHT), 0);
    LightManager::addLight(new Light(glm::vec3(-50, 2, 0), glm::vec3(0, 1, 0),
        glm::vec3(0, 0.03, 0), Light::POINT_LIGHT), 1);
    LightManager::addLight(new Light(glm::vec3(-50, 2, -50), glm::vec3(0, 0, 1),
        glm::vec3(0, 0.03, 0), Light::POINT_LIGHT), 2);
    LightManager::addLight(new Light(glm::vec3(0, 2, -50), glm::vec3(1, 1, 0),
        glm::vec3(0, 0.03, 0), Light::POINT_LIGHT), 3);
    LightManager::addLight(new Light(glm::vec3(-25, 2, -25), glm::vec3(1),
        glm::vec3(0, 0.03, 0), Light::POINT_LIGHT), 4);
}

void installShaders(){
    Shader * shader = LoadManager::getShader("vert.glsl", "frag.glsl");
    shader->loadHandle("aPosition");
    shader->loadHandle("aNormal");
    shader->loadHandle("uProjMatrix");
    shader->loadHandle("uViewMatrix");
    shader->loadHandle("uModelMatrix");
    shader->loadHandle("UaColor");
    shader->loadHandle("UdColor");
    shader->loadHandle("UsColor");
    shader->loadHandle("UeColor");
    shader->loadHandle("Ushine");
    shader->loadHandle("uEye");
    shader->loadHandle("uMV_IT"); 

    shader->loadHandle("uLightPos");
    shader->loadHandle("uLightFallOff");
    shader->loadHandle("uLightColor");

    shader->loadHandle("uTextureID");
    shader->loadHandle("aTexCoord");
}

void installMeshes(){
    LoadManager::getMesh("sphere.obj");
}

int main(int argc, char **argv)
{
    // Initialise GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // Open a window and create its OpenGL context
    g_width = 600;
    g_height = 600;
    window = glfwCreateWindow(g_width, g_height, "P3 - shading", NULL, NULL);
    if (window == NULL){
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Initialize glad
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Unable to initialize glad");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the background color
    glClearColor(0.4f, 0.4f, 0.8f, 1.0f);
    glPointSize(18);

    GLSL::checkVersion();

    setupCams();
    setupLights();
    installShaders();
    installMeshes();

    //createFrameBuffers();

    FBO myFBO = FBO(g_width, g_height, 1, true);
    myFBO.init();

    Ball * ball = new Ball();
    ball->init();

    Cube * cube = new Cube(myFBO.getTexture(0));
    cube->init();

    assert(glGetError() == GL_NO_ERROR);
    do{
        glClearColor(0.4f, 0.4f, 0.8f, 1.0f);

        myFBO.enable();

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ball->draw();

        myFBO.disable();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube->draw();

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(window) == 0);

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}
