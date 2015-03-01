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

#include "scene.h"
#include "hero.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr




GLFWwindow* window;
using namespace std;

int g_width;
int g_height;
bool g_lock_mouse = true;
bool g_first_mouse_movement = true;

Hero * hero;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //Move light in X axis
    //if (key == GLFW_KEY_L && action == GLFW_PRESS)
    //    g_lock_mouse = !g_lock_mouse;

    if (key == GLFW_KEY_I && action == GLFW_PRESS)
        g_light.y += 0.1f;
    else if (key == GLFW_KEY_K && action == GLFW_PRESS)
        g_light.y -= 0.1f;
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        g_light.x += 0.1f;
    else if (key == GLFW_KEY_J && action == GLFW_PRESS)
        g_light.x -= 0.1f;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (hero){
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
    }
}


void setupCams(){
    int numberCameras = 3;
    glm::mat4 Projection = glm::perspective(90.0f, (float)g_width / g_height, 0.1f, 100.f);

    for (int i = 0; i < numberCameras; i++){
        Cam * mainCam = new Cam(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
        CamManager::addCam(mainCam);
        CamManager::getCam(i)->projectionMatrix = Projection;
    }
}

void setupLights(){
    LightManager::init();
    LightManager::addLight(new Light(glm::vec3(0, 2, 0), glm::vec3(1, 0, 0), 
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
    if (!gladLoadGL()) {
        fprintf(stderr, "Unable to initialize glad");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set the background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPointSize(18);

    GLSL::checkVersion();

    setupCams();
    setupLights();
    installShaders();
    installMeshes();


    int rows = 25, cols = 25;

    //GameMap * gameMap = new GameMap("cccccc...cc...cc...cccccc", rows, cols);
    GameMap * gameMap = new GameMap(string(rows*cols, 'c'), rows, cols);
    gameMap->init();

    g_light = gameMap->getCubePos(rows/2*rows + cols/2);

    hero = new Hero(gameMap, 0);
    hero->init();
    gameMap->addChild(hero);

    LightObject * l = new LightObject();
    l->init();

    srand(time(NULL));
    int enemies[20] = {0};
    for (int i = 0; i < 1; i++){
        int cube_pos;
        bool in = true;
        do{
            cube_pos = (rand() % (rows*cols));
            if (cube_pos == 0)
                cube_pos = 1;
            for (int j = 0; j < i; j++){
                if (cube_pos == enemies[j]){
                    in = false;
                    break;
                }
                else{
                    in = true;
                }
            }
        } while (!in);
        enemies[i] = cube_pos;

        int randomRotation = rand() % 360;
        if (i < 10){
            Enemy * e = new Enemy(gameMap, 6*rows + 4, 30);
            e->init();
            gameMap->addChild(e);
        }
        else {
            Item* item = new Item(gameMap, cube_pos, randomRotation);
            item->init();
            gameMap->addChild(item);
        }
    }

    assert(glGetError() == GL_NO_ERROR);
    do{
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        hero->checkInput(window);

        gameMap->draw();
        l->draw();

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
