#ifndef __RENDER_H__
#define __RENDER_H__

#include <vector>

#include "glm/glm.hpp"
#include "GLIncludes.h"

#include "fbo.h"
#include "object3d.h"
#include "shader.h"

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
        GLuint vertexBuf;

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

    class PostProcessor : public Processor{
    public:
        PostProcessor();
        PostProcessor(int _width, int _height, int _nTextures, int _nFBOs);

        void passBlur(Processor * processor, int cycles, Shader * blurShader);
        void passBloom(Processor * processor, Shader * bloomShader, Shader * blurShader, int cycles);
    };
}

#endif