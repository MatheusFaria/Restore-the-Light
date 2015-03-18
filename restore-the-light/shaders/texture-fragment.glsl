#version 120

uniform sampler2D uTexID;

varying vec2 vTexCoord;

void main()
{
	gl_FragData[0] = texture2D(uTexID, vTexCoord);
}
