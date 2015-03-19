#version 120

uniform sampler2D uTex1ID;
uniform sampler2D uTex2ID;

varying vec2 vTexCoord;

void main()
{
	gl_FragData[0] = texture2D(uTex1ID, vTexCoord)*texture2D(uTex2ID, vTexCoord);
}
