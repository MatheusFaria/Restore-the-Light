#version 120

uniform sampler2D uTexID;
uniform sampler2D uBlurTexID;

varying vec2 vTexCoord;

void main()
{
	vec4 texel = texture2D(uTexID, vTexCoord);
	vec4 blurTexel = texture2D(uBlurTexID, vTexCoord);

	gl_FragData[0] = texel + blurTexel;
}
