#include <iostream>
#include <cassert>
#include <vector>
#include <ctime>
#include <sstream>
#include <fstream>

#include "GaussianBlur.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //perspective, trans etc
#include "glm/gtc/type_ptr.hpp" //value_ptr

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
#include "render.h"

#define GAUSS_KERNEL_SIZE 35

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
        mesh = LoadManager::getMesh("cube-textures.obj");

        loadVertexBuffer("posBufObj");
        loadTextureBuffer("texBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert-tex.glsl", "frag-map.glsl");

        tex = TextureManager::getTexture("yellow.bmp");
        atex = TextureManager::getTexture("red-gray-alpha.bmp");
        //tex = TextureManager::getTexture("bricks.bmp");
    }

    void drawObject(){
        enableAttrArray2f("aTexCoord", "texBufObj");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex->getTexture());
        glUniform1i(shader->getHandle("uTexID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, atex->getTexture());
        glUniform1i(shader->getHandle("uAlphaTexID"), 1);

        glUniform1i(shader->getHandle("uCompleteGlow"), 1);

        loadIdentity();

        rot += 0.5f;
        addTransformation(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -1)));
        addTransformation(glm::rotate(glm::mat4(1.0f), rot, glm::vec3(0, 1, 0)));
        bindModelMatrix("uModelMatrix");

        drawElements();
    }
    float rot = 45;
    Texture * tex, * atex;
};

class Item : public Object3D{
public:
    Item() {}

    void init(){
        mesh = LoadManager::getMesh("sphere-tex.obj");

        loadVertexBuffer("posBufObj");
        loadNormalBuffer("norBufObj");
        loadTextureBuffer("texBufObj");
        loadElementBuffer();

        shader = LoadManager::getShader("vert-map.glsl", "frag-map.glsl");

        tex = TextureManager::getTexture("red-gray.bmp");
        atex = TextureManager::getTexture("red-gray-alpha.bmp");

        rotAxe = 0;
    }

    void drawObject(){
        enableAttrArray2f("aTexCoord", "texBufObj");

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex->getTexture());
        glUniform1i(shader->getHandle("uTexID"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, atex->getTexture());
        glUniform1i(shader->getHandle("uAlphaTexID"), 1);

        glUniform1i(shader->getHandle("uCompleteGlow"), 0);

        loadIdentity();

        glm::vec3 pos = glm::vec3(0, 0, -1);

        rotAxe += 0.05;
        addTransformation(glm::translate(glm::mat4(1.0f), pos));
        addTransformation(glm::rotate(glm::mat4(1.0f), rotAxe, glm::vec3(0, 1, 0)));
        addTransformation(glm::scale(glm::mat4(1.0f), glm::vec3(0.6)));
        bindModelMatrix("uModelMatrix");

        drawElements();
    }

    float rotAxe;
    Texture * tex, *atex;
};

Light * light;
int hot_key = 0;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        g_lock_mouse = !g_lock_mouse;

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        hot_key = (hot_key + 1) % 4;

    if (key == GLFW_KEY_A && action == GLFW_PRESS)
        light->pos.x -= 0.1;
    else if (key == GLFW_KEY_D && action == GLFW_PRESS)
        light->pos.x += 0.1;
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
    light = new Light(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1),
        glm::vec3(0, 0.03, 0), Light::POINT_LIGHT);
    LightManager::addLight(light);
}

void installShaders(){
    Shader * shader;

    shader = LoadManager::getShader("vert.glsl", "frag.glsl");
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
    shader->loadHandle("uNormalMatrix"); 

    shader->loadHandle("uLightPos");
    shader->loadHandle("uLightFallOff");
    shader->loadHandle("uLightColor");

    shader->loadHandle("uTextureID");
    shader->loadHandle("aTexCoord");

    shader = LoadManager::getShader("vert-map.glsl", "frag-map.glsl");
    shader->loadHandle("aPosition");
    shader->loadHandle("aNormal");

    shader->loadHandle("uProjMatrix");
    shader->loadHandle("uViewMatrix");
    shader->loadHandle("uModelMatrix");
    shader->loadHandle("uNormalMatrix");
    
    shader->loadHandle("uTexID");
    shader->loadHandle("aTexCoord");
    
    shader->loadHandle("uAlphaTexID");

    shader->loadHandle("uCompleteGlow");

    shader = LoadManager::getShader("default-texture-vertex.glsl", "temp.glsl");
    shader->loadHandle("aPosition");
    shader->loadHandle("uTexID");
    shader->loadHandle("uDir");

    shader = LoadManager::getShader("default-texture-vertex.glsl", "frag-glow.glsl");
    shader->loadHandle("aPosition");
    shader->loadHandle("uTexID");
    shader->loadHandle("uBlurTexID");
}

void installMeshes(){
    LoadManager::getMesh("sphere.obj");
}


void createGaussBlurShader(){
    vector<float> kernel = GetAppropriateSeparableGauss(GAUSS_KERNEL_SIZE);
    
    std::stringstream shader;
    string eol = "\n";

    shader << 
        "#version 120" << eol << 
        "uniform sampler2D uTexID;" << eol <<
        "uniform vec2 uDir;" << eol <<
        "varying vec2 vTexCoord;" << eol <<

        "vec3 gaussianBlur2Pass(sampler2D texID, vec2 texel, float resolution, float radius, vec2 dir){" << eol <<
        "    vec4 sum = vec4(0.0);" << eol <<

        "    float blur = radius / resolution;" << eol <<

        "    const int kernel_size = " << kernel.size() << ";" << eol <<
        "    const int half_kernel_size = kernel_size / 2;" << eol <<
        "    float gauss_kernel[] = {" << eol;
                for (int i = 0; i < kernel.size(); i++)
                     shader << "        " << kernel[i] << "," << eol;
    shader <<
        "    };" << eol <<

        "    for (int i = -half_kernel_size; i <= half_kernel_size; i++){" << eol <<
        "        sum += texture2D(texID, vec2(texel.x - i*blur*dir.x, texel.y - i*blur*dir.y)) * gauss_kernel[i + half_kernel_size];" << eol <<
        "    }" << eol <<

        "    return sum;" << eol <<
        "}" << eol <<

        "void main()" << eol <<
        "{" << eol <<
        "    gl_FragData[0] = vec4(gaussianBlur2Pass(uTexID, vTexCoord, 600, 2, uDir), 1);" << eol <<
        "}" << eol;

    ofstream file;
    file.open("shaders/temp.glsl");
    file << shader.str();
    file.close();

}

int main(int argc, char **argv)
{
    createGaussBlurShader();

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

    //Ball * ball = new Ball();
    //ball->init();

    Item * item = new Item();
    item->init();
    
    Render::GeometryProcessor * gBuffer = new Render::GeometryProcessor(g_width, g_height, 4, glm::vec4(0,0,0,1));
    gBuffer->init();

    vector<Object3D *> objs;
    objs.push_back(item);

    Render::PostProcessor * blurPost = new Render::PostProcessor(g_width, g_height, 1, 2);
    blurPost->init();

    Render::LightProcessor * lProcessor = new Render::LightProcessor(g_width, g_height, 1);
    lProcessor->init();

    Shader * shader = LoadManager::getShader("default-texture-vertex.glsl", "temp.glsl");
    Shader * bloomshader = LoadManager::getShader("default-texture-vertex.glsl", "frag-glow.glsl");
   

    assert(glGetError() == GL_NO_ERROR);
    do{
        gBuffer->pass(objs);
        //gBuffer->displayTexture(hot_key);

        lProcessor->pass(gBuffer, LightManager::getLights());
        lProcessor->displayTexture(0);

        //blurPost->passBloom(gBuffer, bloomshader, shader, 1);
        //blurPost->displayTexture(0);

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
