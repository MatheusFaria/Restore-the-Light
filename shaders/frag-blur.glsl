#version 120

uniform sampler2D uTexID;
uniform vec2 uDir;

varying vec2 vTexCoord;

vec3 gaussianBlur2Pass(sampler2D texID, vec2 texel, float resolution, float radius, vec2 dir){
    vec4 sum = vec4(0.0);
    float blur = radius / resolution;
    const int kernel_size = 23;
    const int half_kernel_size = kernel_size / 2;
    float gauss_kernel[] = {
        0.000893198,
        0.00206898,
        0.00442404,
        0.00873251,
        0.0159117,
        0.0267639,
        0.0415565,
        0.0595641,
        0.0788111,
        0.09626,
        0.108533,
        0.112962,
        0.108533,
        0.09626,
        0.0788111,
        0.0595641,
        0.0415565,
        0.0267639,
        0.0159117,
        0.00873251,
        0.00442404,
        0.00206898,
        0.000893198,
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
