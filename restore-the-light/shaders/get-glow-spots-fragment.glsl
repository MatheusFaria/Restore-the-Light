#version 120

uniform sampler2D uTex1ID;
uniform sampler2D uTex2ID;
uniform sampler2D uTex3ID;

varying vec2 vTexCoord;

vec3 alphaMap(vec3 pixel){
	if(length(pixel) > 0.1){
		return vec3(1);
	}
	return vec3(0);
}

void main()
{
	vec4 alphaMapTex = texture2D(uTex1ID, vTexCoord);
	vec4 diffuseTex = texture2D(uTex2ID, vTexCoord);
	vec4 lightTex = texture2D(uTex3ID, vTexCoord);

	vec4 diffusedAlphaTex = alphaMapTex*diffuseTex;
	vec4 alphaLightTex = vec4(alphaMap(vec3(lightTex)), 1);

	gl_FragData[0] = alphaMapTex*lightTex;
}
