#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>

#include "GaussianBlur.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

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
#include "globals.h"

#include "scene.h"
#include "hero.h"

using namespace std;

bool g_lock_mouse = true;
bool g_first_mouse_movement = true;
int hot_key = 0;

Render::GeometryProcessor *gBuffer;
Render::LightProcessor *lightProcessor;
Render::PostProcessor *alphaPost, *bloomPost, *blurPost;

GameMap * gamemap;
Hero * hero;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
        g_lock_mouse = !g_lock_mouse;

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        hot_key = (hot_key + 1) % 6;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (hero){
        if (hero->isFPS()){
            if (g_first_mouse_movement){
                g_first_mouse_movement = false;
                glfwSetCursorPos(window, Global::screenWidth / 2, Global::screenHeight / 2);
                return;
            }

            float phi = CamManager::currentCam()->getPhi(), 
                theta = CamManager::currentCam()->getTheta();
            phi += (Global::screenHeight / 2 - ypos) * 360 / Global::screenHeight;
            theta -= (Global::screenWidth / 2 - xpos) * 360 / Global::screenWidth;

            if (phi > 80)
                phi = 80;
            if (phi < -80)
                phi = -80;

            CamManager::currentCam()->setAngles(theta, phi);

            if (g_lock_mouse){
                glfwSetCursorPos(window, Global::screenWidth / 2, Global::screenHeight / 2);
            }
        }
    }
}

void setupCams(){
    int numberCameras = 3;
    glm::mat4 Projection = glm::perspective(90.0f, (float)Global::screenWidth / Global::screenHeight, 0.1f, 100.f);

    for (int i = 0; i < numberCameras; i++){
        Cam * mainCam = new Cam(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
        CamManager::addCam(mainCam);
        CamManager::getCam(i)->projectionMatrix = Projection;
    }

    CamManager::setCam(0);
}

void setupLights(){
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

    shader->loadHandle("UaColor");
    shader->loadHandle("UdColor");
    shader->loadHandle("UsColor");
    shader->loadHandle("UeColor");
    shader->loadHandle("Ushine");
    
    shader->loadHandle("uTexID");
    shader->loadHandle("aTexCoord");
    
    shader->loadHandle("uAlphaTexID");

    shader->loadHandle("uCompleteGlow");
    shader->loadHandle("uApplyTexture");

    shader = LoadManager::getShader("default-texture-vertex.glsl", "frag-blur.glsl");
    shader->loadHandle("aPosition");
    shader->loadHandle("uTexID");
    shader->loadHandle("uDir");

    shader = LoadManager::getShader("default-texture-vertex.glsl", "sum-two-textures-fragment.glsl");
    shader->loadHandle("aPosition");
    shader->loadHandle("uTex1ID");
    shader->loadHandle("uTex2ID");

    shader = LoadManager::getShader("default-texture-vertex.glsl", "get-glow-spots-fragment.glsl");
    shader->loadHandle("aPosition");
    shader->loadHandle("uTex1ID");
    shader->loadHandle("uTex2ID");
    shader->loadHandle("uTex3ID");
}

void installMeshes(){
    LoadManager::getMesh("sphere.obj");
}

void createGaussBlurShader(){
    vector<float> kernel = GetAppropriateSeparableGauss(Global::gaussKernelSize);
    
    std::stringstream shader;
    string eol = "\n";

    shader << 
        "#version 120" << eol << eol <<
        "uniform sampler2D uTexID;" << eol <<
        "uniform vec2 uDir;" << eol << eol <<
        "varying vec2 vTexCoord;" << eol << eol <<

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
        "}" << eol << eol <<

        "void main()" << eol <<
        "{" << eol <<
        "    gl_FragData[0] = vec4(gaussianBlur2Pass(uTexID, vTexCoord, 600, 1, uDir), 1);" << eol <<
        "}" << eol;

    ofstream file;
    file.open("shaders/frag-blur.glsl");
    file << shader.str();
    file.close();

}

void setupWorld(){
    createGaussBlurShader();

    setupCams();
    setupLights();

    installShaders();
    installMeshes();
}

void setupGame(){
    int rows = 36, cols = 30;

    stringstream mapss;
    mapss 
        << "cccccccccc...................."
        << "c............................."
        << "c............................."
        << "c............................."
        << "c............................."
        << "c............................."
        << "c.......c....................."
        << "c.......c....................."
        << "c.......c....................."
        << "c.......c....................."
        << "c.......c....................."
        << "ccccccccccccccccc............."
        << ".....c..........c............."
        << ".....c..........c............."
        << ".....c..........ccccccccc....."
        << ".....c........................"
        << ".....c........................"
        << ".....c........................"
        << ".....c........................"
        << ".....c........................"
        << ".....c........................"
        << "cccccc........................"
        << "c................ccccccccccccc"
        << "c................c...........c"
        << "c................c...........c"
        << "c................c...........c"
        << "c................c...........c"
        << "c................c...........c"
        << "cccccccc.........c...........c"
        << "c................ccccccccccccc"
        << "c......................c......"
        << "c......................c......"
        << "c......................c......"
        << "c......................c......"
        << "c......................c......"
        << "cccccccccccccccccccccccccccccc";


    gamemap = new GameMap(mapss.str(), rows, cols);
    gamemap->init();

    hero = new Hero(gamemap, 0);
    hero->init();
    gamemap->addChild(hero);
}

void setupRenderEngine(){
    lightProcessor = new Render::LightProcessor(Global::screenWidth, Global::screenHeight, 1);
    lightProcessor->init();

    gBuffer = new Render::GeometryProcessor(Global::screenWidth, Global::screenHeight, 6, glm::vec4(0, 0, 0, 1));
    gBuffer->init();

    blurPost = new Render::PostProcessor(Global::screenWidth / 4, Global::screenHeight / 4, 1, 2);
    blurPost->init();

    bloomPost = new Render::PostProcessor(Global::screenWidth, Global::screenHeight, 1, 2);
    bloomPost->init();

    alphaPost = new Render::PostProcessor(Global::screenWidth, Global::screenHeight, 1, 1);
    alphaPost->init();
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
    Global::window = glfwCreateWindow(Global::screenWidth, Global::screenHeight, Global::gameName.c_str(), NULL, NULL);
    if (Global::window == NULL){
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Global::window);
    glfwSetKeyCallback(Global::window, key_callback);
    glfwSetCursorPosCallback(Global::window, cursor_position_callback);
    glfwSetInputMode(Global::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // Initialize glad
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Unable to initialize glad");
        glfwDestroyWindow(Global::window);
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(Global::window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(Global::window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

    glEnable(GL_TEXTURE_2D);

    setupRenderEngine();
    setupWorld();
    setupGame();

    vector<Object3D *> objs;
    objs.push_back(gamemap);
   
    assert(glGetError() == GL_NO_ERROR);


    double time = glfwGetTime();
    int frames = 0;
    do{
        hero->checkInput(Global::window);

        gBuffer->pass(objs);

        lightProcessor->pass(gBuffer, LightManager::getPointLights(), 
            LightManager::getSpotLights(), LightManager::getDirectionalLights());

        alphaPost->passTernaryTextureOp(
            gBuffer->getOutFBO()->getTexture(1),
            gBuffer->getOutFBO()->getTexture(0), 
            lightProcessor->getOutFBO()->getTexture(0),
            LoadManager::getShader("default-texture-vertex.glsl", "get-glow-spots-fragment.glsl"));

        blurPost->passBlur(alphaPost, 2,
            LoadManager::getShader("default-texture-vertex.glsl", "frag-blur.glsl"));

        bloomPost->passBinaryTextureOp(
            blurPost->getOutFBO()->getTexture(0), 
            lightProcessor->getOutFBO()->getTexture(0), 
            LoadManager::getShader("default-texture-vertex.glsl", "sum-two-textures-fragment.glsl"));
        bloomPost->displayTexture(0);



        frames++;
        if (glfwGetTime() - time >= 1.0){
            cout << frames << endl;
            frames = 0;
            time = glfwGetTime();
        }

        // Swap buffers
        glfwSwapBuffers(Global::window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while (glfwGetKey(Global::window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
    glfwWindowShouldClose(Global::window) == 0);

    // Close OpenGL Global::window and terminate GLFW
    glfwTerminate();

    return 0;
}
