#version 120

uniform sampler2D uTexID;
uniform vec2 uDir;

varying vec2 vTexCoord;

vec3 gaussianBlur2Pass(sampler2D texID, vec2 texel, float resolution, float radius, vec2 dir){
    vec4 sum = vec4(0.0);

    float blur = radius/resolution; 

	const int kernel_size = 9;
	const int half_kernel_size = kernel_size/2;
	float gauss_kernel[] = {
		0.0162162162,
		0.0540540541,
		0.1216216216,
		0.1945945946,
		0.2270270270,
		0.1945945946,
		0.1216216216,
		0.0540540541,
		0.0162162162,
	};

	for(int i = -half_kernel_size; i <= half_kernel_size; i++){
		sum += texture2D(texID, vec2(texel.x - i*blur*dir.x, texel.y - i*blur*dir.y)) * gauss_kernel[i + half_kernel_size];
	}

    return sum;
}

void main()
{
	gl_FragData[0] = vec4(gaussianBlur2Pass(uTexID, vTexCoord, 600, 5, uDir), 1);
}
