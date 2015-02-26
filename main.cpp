/*
* Author: Matheus de Sousa Faria
* CPE 471 - Introduction to Computer Graphics
* Program 3
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

#include "scene.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

# define M_PI 3.141592
float phi = 0, theta = -90;
bool g_lock_mouse = true;

GLFWwindow* window;
using namespace std;

int g_width;
int g_height;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //Move light in X axis
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        g_lock_mouse = !g_lock_mouse;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    phi += (g_height / 2 - ypos) * 360 / g_height;
    theta -= (g_width / 2 - xpos) * 360 / g_width;

    if (phi > 90)
        phi = 90;
    if (phi < -90)
        phi = -90;

    if (g_lock_mouse){
        glfwSetCursorPos(window, g_width / 2, g_height / 2);
    }
}

float degToRad(float angle){
    return angle*M_PI / 180;
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
    //glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPointSize(18);

    GLSL::checkVersion();

    int rows = 12, cols = 12;

    GameMap * gameMap = new GameMap(string(rows*cols, 'c'), rows , cols);
    gameMap->init();

    Hero * hero = new Hero(gameMap, 0);
    hero->init();
    //gameMap->addChild(hero);

    srand(time(NULL));
    int enemies[20] = {0};
    for (int i = 0; i < 20; i++){
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

        if (i < 10){
            Enemy * e = new Enemy(gameMap, cube_pos);
            e->init();
            gameMap->addChild(e);
        }
        else {
            Item* item = new Item(gameMap, cube_pos);
            item->init();
            gameMap->addChild(item);
        }
    }

    glm::mat4 Projection = glm::perspective(90.0f, (float)g_width / g_height, 0.1f, 100.f);

    Cam * mainCam = new Cam(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    CamManager::addCam(mainCam);
    CamManager::setCam(0);
    CamManager::getCam(0)->projectionMatrix = Projection;

    assert(glGetError() == GL_NO_ERROR);

    glm::vec3 viewVector = glm::vec3(0.0f);
    do{
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        mainCam->lookAt.x = cos(degToRad(phi))*cos(degToRad(theta)) + mainCam->eye.x;
        mainCam->lookAt.y = sin(degToRad(phi)) + mainCam->eye.y;
        mainCam->lookAt.z = cos(degToRad(phi))*cos(degToRad(90 - theta)) + mainCam->eye.z;
        
        viewVector = mainCam->eye - mainCam->lookAt;

        //Capturing the keybord input
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            mainCam->eye -= viewVector*0.1f;
            mainCam->lookAt -= viewVector*0.1f;
        }
        else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            mainCam->eye += viewVector*0.1f;
            mainCam->lookAt += viewVector*0.1f;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            glm::vec3 strafe = glm::cross(viewVector, mainCam->upVector);
            mainCam->eye += strafe*0.1f;
            mainCam->lookAt += strafe*0.1f;
        }
        else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            glm::vec3 strafe = glm::cross(viewVector, mainCam->upVector);
            mainCam->eye -= strafe*0.1f;
            mainCam->lookAt -= strafe*0.1f;
        }

        gameMap->draw();
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
