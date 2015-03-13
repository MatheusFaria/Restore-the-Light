#version 120

uniform sampler2D uTexID;
uniform vec2 uDir;

varying vec2 vTexCoord;

vec3 gaussianBlur2Pass(sampler2D texID, vec2 texel, float resolution, float radius, vec2 dir){
    vec4 sum = vec4(0.0);
    float blur = radius / resolution;
    const int kernel_size = 35;
    const int half_kernel_size = kernel_size / 2;
    float gauss_kernel[] = {
        0.000573372,
        0.000997489,
        0.00167805,
        0.00272979,
        0.00429416,
        0.00653211,
        0.00960849,
        0.0136673,
        0.018799,
        0.0250043,
        0.0321603,
        0.0399992,
        0.048107,
        0.0559489,
        0.0629218,
        0.0684283,
        0.071961,
        0.0731786,
        0.071961,
        0.0684283,
        0.0629218,
        0.0559489,
        0.048107,
        0.0399992,
        0.0321603,
        0.0250043,
        0.018799,
        0.0136673,
        0.00960849,
        0.00653211,
        0.00429416,
        0.00272979,
        0.00167805,
        0.000997489,
        0.000573372,
    };
    for (int i = -half_kernel_size; i <= half_kernel_size; i++){
        sum += texture2D(texID, vec2(texel.x - i*blur*dir.x, texel.y - i*blur*dir.y)) * gauss_kernel[i + half_kernel_size];
    }
    return sum;
}

void main()
{
    gl_FragData[0] = vec4(gaussianBlur2Pass(uTexID, vTexCoord, 600, 1, uDir), 1);
}
