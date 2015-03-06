#version 120

uniform sampler2D uTexID;
uniform sampler2D uGlowTexID;

varying vec2 vTexCoord;

void main()
{
	vec4 texel = texture2D(uTexID, vTexCoord);
	vec4 glowTexel = texture2D(uGlowTexID, vTexCoord);

	gl_FragData[0] = texel + glowTexel;
}
