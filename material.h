#ifndef __MATERIALS_H__
#define __MATERIALS_H__

#include "shader.h"

namespace Material{
    const int 
        BLUE_PLASTIC = 0,
        FLAT_GREY = 1,
        GOLD = 2,
        CHOCOLATE = 3;

    void SetMaterial(int id, Shader * shader) {
        switch (id) {
        case BLUE_PLASTIC:
            glUniform3f(shader->getHandle("UaColor"), 0.02, 0.02, 0.1);
            glUniform3f(shader->getHandle("UdColor"), 0.0, 0.08, 0.5);
            glUniform3f(shader->getHandle("UsColor"), 0.14, 0.14, 0.4);
            glUniform1f(shader->getHandle("Ushine"), 120.0);
            break;
        case FLAT_GREY:
            glUniform3f(shader->getHandle("UaColor"), 0.13, 0.13, 0.14);
            glUniform3f(shader->getHandle("UdColor"), 0.3, 0.3, 0.4);
            glUniform3f(shader->getHandle("UsColor"), 0.3, 0.3, 0.4);
            glUniform1f(shader->getHandle("Ushine"), 4.0);
            break;
        case GOLD:
            glUniform3f(shader->getHandle("UaColor"), 0.09, 0.07, 0.08);
            glUniform3f(shader->getHandle("UdColor"), 0.91, 0.782, 0.82);
            glUniform3f(shader->getHandle("UsColor"), 1.0, 0.913, 0.8);
            glUniform1f(shader->getHandle("Ushine"), 200.0);
            break;
        case CHOCOLATE:
            glUniform3f(shader->getHandle("UaColor"), 0.141, 0.1, 0.0);
            glUniform3f(shader->getHandle("UdColor"), 0.267, 0.141, 0.0);
            glUniform3f(shader->getHandle("UsColor"), 0.709, 0.478, 0.216);
            glUniform1f(shader->getHandle("Ushine"), 100.0);
            break;
        }
    }
}

#endif