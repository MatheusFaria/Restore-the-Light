#version 120

uniform sampler2D uTexID;
uniform vec2 uDir;

varying vec2 vTexCoord;

vec3 gaussianBlur2Pass(sampler2D texID, vec2 texel, float resolution, float radius, vec2 dir){
    vec4 sum = vec4(0.0);
    float blur = radius / resolution;
    const int kernel_size = 27;
    const int half_kernel_size = kernel_size / 2;
    float gauss_kernel[] = {
        0.000745792,
        0.0015294,
        0.00296123,
        0.00541342,
        0.00934369,
        0.0152269,
        0.0234291,
        0.0340365,
        0.0466855,
        0.0604599,
        0.0739266,
        0.0853456,
        0.0930272,
        0.0957383,
        0.0930272,
        0.0853456,
        0.0739266,
        0.0604599,
        0.0466855,
        0.0340365,
        0.0234291,
        0.0152269,
        0.00934369,
        0.00541342,
        0.00296123,
        0.0015294,
        0.000745792,
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
