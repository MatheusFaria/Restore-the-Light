/*
* Author: Matheus de Sousa Faria
* CPE 471 - Introduction to Computer Graphics
* Program 3
* Code modifed from ZJ Wood
*/

#include <iostream>
#include <cassert>
#include <vector>

#include "GLSL.h"
#include "mesh.h"
#include "shader.h"
#include "object3d.h"
#include "load_manager.h"
#include "material.h"
#include "virtual_camera.h"

#include "scene.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr


GLFWwindow* window;
using namespace std;

int g_width;
int g_height;
float g_Camtrans = -2.5;
glm::vec3 g_trans(0, 0, 0);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /*
    //Rotate the object around Y axis
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        g_angle += 10;
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
        g_angle -= 10;

    //Rotate the object around X axis
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
        g_x_angle += 10;
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
        g_x_angle -= 10;

        
    //Move light in X axis
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        g_light.x -= 0.25;
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        g_light.x += 0.25;

    //Move light in Z axis
    if (key == GLFW_KEY_I && action == GLFW_PRESS)
        g_light.z -= 0.25;
    if (key == GLFW_KEY_K && action == GLFW_PRESS)
        g_light.z += 0.25;*/
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
    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
    glPointSize(18);

    GLSL::checkVersion();

    GameMap * gameMap = new GameMap();
    gameMap->init();

    Hero * hero = new Hero(gameMap);
    hero->init();

    gameMap->addChild(hero);

    Enemy * e = new Enemy(gameMap);
    e->init();

    glm::mat4 Projection = glm::perspective(90.0f, (float)g_width / g_height, 0.1f, 100.f);

    Cam * isoCam = new Cam(glm::vec3(10, 10, 5), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    int isoCamId = CamManager::addCam(isoCam);

    Cam * mainCam = new Cam(glm::vec3(0, 0, 5), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    int mainCamId = CamManager::addCam(mainCam);

    Cam * topCam = new Cam(glm::vec3(0.1, 30, 0), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    int topCamId = CamManager::addCam(topCam);

    CamManager::setCam(mainCamId);

    assert(glGetError() == GL_NO_ERROR);
    do{
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CamManager::currentCam()->projectionMatrix = Projection;

        gameMap->draw();
        //hero->draw();

        //e->draw();

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
