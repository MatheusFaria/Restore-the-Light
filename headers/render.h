#ifndef __RENDER_H__
#define __RENDER_H__

#include <vector>
#include <list>

#include "glm/glm.hpp"
#include "GLIncludes.h"

#include "fbo.h"
#include "object3d.h"
#include "shader.h"
#include "light.h"
#include "mesh.h"

namespace Render{
    class Processor{
    public:
        Processor();
        Processor(int _width, int _height, int _nTextures, bool _depth, int _nFBOs, glm::vec4 _refreshColor);

        void init();

        void displayTextures(std::vector<int> texIDs);
        void displayTexture(int texID);

        bool hasOutput();
        FBO * getOutFBO();

    protected:
        std::vector<FBO *> fbos;
        FBO * outFBO;
        glm::vec4 refreshColor;
        int width, height, nTextures, nFBOs;
        bool depth;
        GLuint vertexBuf, elementBuf;

    private:
        Shader * shader;

        void prepareDisplay();
    };

    class GeometryProcessor : public Processor{
    public:
        GeometryProcessor();
        GeometryProcessor(int _width, int _height, int _nTextures, glm::vec4 _refreshColor);

        void pass(std::vector<Object3D *> objects);
    };

    class LightProcessor : public Processor{
    public:
        LightProcessor();
        LightProcessor(int _width, int _height, int _nTextures);

        void init();
        void pass(Processor * processor, std::list<Light *> pointLights, std::list<Light *> spotLights, 
            std::list<Light *> directionalLights);
        void passPointLights(Processor * processor, std::list<Light *> lights);
        void passSpotLights(Processor * processor, std::list<Light *> lights);
        void passDirectionalLights(Processor * processor, std::list<Light *> lights);
        void passAmbientLight(Processor * processor);

    private:
        Shader * directionalShader, *pointShader, *spotShader, *ambientShader;
        Mesh * pointLightArea;

        GLuint vertexBuffer, elementBuffer;

        void initShader();
        void initGeometry();
    };

    class PostProcessor : public Processor{
    public:
        PostProcessor();
        PostProcessor(int _width, int _height, int _nTextures, int _nFBOs);

        void passBlur(Processor * processor, int cycles, Shader * blurShader);
        void passBloom(Processor * processorAlpha, Processor * processorDiffuse,
            Shader * bloomShader, Shader * blurShader, int cycles);
    };
}

#endif