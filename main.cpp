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

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

#define GOURAD 1
#define PHONG 0

#define ON 1
#define OFF 0

GLFWwindow* window;
using namespace std;

int g_SM = GOURAD;
int g_normal_mode = OFF;
int g_width;
int g_height;
float g_Camtrans = -2.5;
float g_angle = 0, g_x_angle = 0;
int g_mat_id = 0;
int g_obj_id = 0;
int g_mat_quantity = 4;
glm::vec3 g_trans(0, 0, 0);
glm::vec3 g_light(0.5, 0.5, 5);

Shader * shader;

/* helper function to make sure your matrix handle is correct */
inline void safe_glUniformMatrix4fv(const GLint handle, const GLfloat data[]) {
    if (handle >= 0)
        glUniformMatrix4fv(handle, 1, GL_FALSE, data);
}

/* helper function to set projection matrix - don't touch */
void SetProjectionMatrix() {
    glm::mat4 Projection = glm::perspective(90.0f, (float)g_width / g_height, 0.1f, 100.f);
    safe_glUniformMatrix4fv(shader->getHandle("uProjMatrix"), glm::value_ptr(Projection));
}

/* camera controls - do not change beyond the current set up to rotate*/
void SetView() {
    glm::mat4 Trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0, g_Camtrans));
    safe_glUniformMatrix4fv(shader->getHandle("uViewMatrix"), glm::value_ptr(Trans));
}


class Cube : public Object3D{
public:
    Cube(){}

    void init(){
        mesh = LoadManager::getMesh("cube.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert.glsl", "frag.glsl");
        shader->loadHandle("aPosition");
        shader->loadHandle("aNormal");
        shader->loadHandle("uProjMatrix");
        shader->loadHandle("uViewMatrix");
        shader->loadHandle("uModelMatrix");
        shader->loadHandle("uLightPos");
        shader->loadHandle("UaColor");
        shader->loadHandle("UdColor");
        shader->loadHandle("UsColor");
        shader->loadHandle("UeColor");
        shader->loadHandle("Ushine");
        shader->loadHandle("uShadeModel");
        shader->loadHandle("uNormalM");
        shader->loadHandle("uEye");
    }

    void drawObject(){
        SetProjectionMatrix();
        SetView();

        loadIdentity();
        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(-3, -3, -5)));
        bindModelMatrix("uModelMatrix");

        Material::SetMaterial(Material::BLUE_PLASTIC, shader);

        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        glUniform3f(shader->getHandle("uLightPos"), g_light.x, g_light.y, g_light.z);
        glUniform1i(shader->getHandle("uShadeModel"), g_SM);
        glUniform1i(shader->getHandle("uNormalM"), g_normal_mode);
        glUniform3f(shader->getHandle("uEye"), 0, 0, 0);

        drawElements();
    }
};

class Bunny : public Object3D{
public:
    Bunny(){}

    void init(){
        mesh = LoadManager::getMesh("bunny.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert.glsl", "frag.glsl");
        shader->loadHandle("aPosition");
        shader->loadHandle("aNormal");
        shader->loadHandle("uProjMatrix");
        shader->loadHandle("uViewMatrix");
        shader->loadHandle("uModelMatrix");
        shader->loadHandle("uLightPos");
        shader->loadHandle("UaColor");
        shader->loadHandle("UdColor");
        shader->loadHandle("UsColor");
        shader->loadHandle("UeColor");
        shader->loadHandle("Ushine");
        shader->loadHandle("uShadeModel");
        shader->loadHandle("uNormalM");
        shader->loadHandle("uEye");
    }

    void drawObject(){
        SetProjectionMatrix();
        SetView();

        loadIdentity();
        addTransformation(glm::rotate(glm::mat4(1.0f), g_x_angle, glm::vec3(1.0f, 0.0f, 0.0f)));
        addTransformation(glm::rotate(glm::mat4(1.0f), g_angle, glm::vec3(0.0f, 1, 0)));
        addTransformation(glm::translate(glm::mat4(1.0f), g_trans));
        bindModelMatrix("uModelMatrix");

        Material::SetMaterial(Material::CHOCOLATE, shader);

        glUniform3f(shader->getHandle("UeColor"), 0, 0, 0);
        glUniform3f(shader->getHandle("uLightPos"), g_light.x, g_light.y, g_light.z);
        glUniform1i(shader->getHandle("uShadeModel"), g_SM);
        glUniform1i(shader->getHandle("uNormalM"), g_normal_mode);
        glUniform3f(shader->getHandle("uEye"), 0, 0, 0);

        drawElements();
    }
};


Bunny myBunny;
Cube myCube;


void window_size_callback(GLFWwindow* window, int w, int h){
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    g_width = w;
    g_height = h;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
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

    //Switches the material
    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        g_mat_id = (g_mat_id + 1) % g_mat_quantity;

    //Switches between Phong and Goroud
    if (key == GLFW_KEY_Z && action == GLFW_PRESS){
        g_SM = !g_SM;
        g_normal_mode = OFF;
    }

    //Switches to normal coloring
    if (key == GLFW_KEY_N && action == GLFW_PRESS){
        g_SM = GOURAD;
        g_normal_mode = !g_normal_mode;
    }

    //Move light in X axis
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        g_light.x -= 0.25;
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
        g_light.x += 0.25;

    //Move light in Z axis
    if (key == GLFW_KEY_I && action == GLFW_PRESS)
        g_light.z -= 0.25;
    if (key == GLFW_KEY_K && action == GLFW_PRESS)
        g_light.z += 0.25;

    // Change the central shape (bunny, cube, sphere)
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
        g_obj_id = (g_obj_id + 1) % 3;
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
    glfwSetWindowSizeCallback(window, window_size_callback);

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

    shader = LoadManager::getShader("vert.glsl", "frag.glsl");
    
    myCube = Cube();
    myCube.init();

    myBunny = Bunny();
    myBunny.init();

    assert(glGetError() == GL_NO_ERROR);
    do{
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBunny.draw();
        myCube.draw();

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
