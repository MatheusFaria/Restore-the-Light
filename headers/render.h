#ifndef __RENDER_H__
#define __RENDER_H__

#include <vector>

#include "glm/glm.hpp"

#include "fbo.h"
#include "object3d.h"

namespace Render{
    class Processor{
    public:
        Processor();
        Processor(int _width, int _height, int _nTextures, bool _depth, int _nFBOs, glm::vec4 _refreshColor);

        void init();

        void displayTexture(int fboID, int texID);

    protected:
        std::vector<FBO *> fbos;
        glm::vec4 refreshColor;
        int width, height, nTextures, nFBOs;
        bool depth;
    };

    class GeometryProcessor : public Processor{
    public:
        GeometryProcessor();
        GeometryProcessor(int _width, int _height, int _nTextures, glm::vec4 _refreshColor);

        void pass(std::vector<Object3D *> objects);

    private:
        FBO * gBuffer;
        glm::vec4 refreshColor;
        int width, height, nTextures;
    };
}

#endif