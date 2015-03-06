#version 120

uniform sampler2D uTexID;

varying vec2 vTexCoord;

void main()
{
	vec4 texel = texture2D(uTexID, vTexCoord);

	gl_FragData[0] = texel;
}
