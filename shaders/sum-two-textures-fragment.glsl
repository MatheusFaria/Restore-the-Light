#version 120

uniform sampler2D uTex1ID;
uniform sampler2D uTex2ID;

varying vec2 vTexCoord;

vec3 alphaMap(vec3 pixel){
	if(length(pixel) > 0.1){
		return vec3(1);
	}
	return vec3(0);
}

void main()
{
	vec4 texel = vec4(alphaMap(vec3(texture2D(uTex2ID, vTexCoord))), 1);
	gl_FragData[0] = texel*texture2D(uTex1ID, vTexCoord);
}
