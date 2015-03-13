#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "GLIncludes.h"

namespace Global{
    const int screenHeight = 600;
    const int screenWidth = 600;

    const int gaussKernelSize = 27;

    const std::string gameName = "Restore the Light";

    GLFWwindow* window;
}

#endif