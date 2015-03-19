#version 120

uniform sampler2D uTexID;
uniform vec2 uDir;

varying vec2 vTexCoord;

vec3 gaussianBlur2Pass(sampler2D texID, vec2 texel, float resolution, float radius, vec2 dir){
    vec4 sum = vec4(0.0);
    float blur = radius / resolution;
    const int kernel_size = 17;
    const int half_kernel_size = kernel_size / 2;
    float gauss_kernel[] = {
        0.00118045,
        0.0037082,
        0.00999995,
        0.02315,
        0.0460066,
        0.0784889,
        0.114951,
        0.144523,
        0.155983,
        0.144523,
        0.114951,
        0.0784889,
        0.0460066,
        0.02315,
        0.00999995,
        0.0037082,
        0.00118045,
    };
    for (int i = -half_kernel_size; i <= half_kernel_size; i++){
        sum += texture2D(texID, vec2(texel.x - i*blur*dir.x, texel.y - i*blur*dir.y)) * gauss_kernel[i + half_kernel_size];
    }
    return sum;
}

void main()
{
    gl_FragData[0] = vec4(gaussianBlur2Pass(uTexID, vTexCoord, 600, 3, uDir), 1);
}
