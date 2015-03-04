#version 120

attribute vec3 aPosition;

varying vec2 texCoord;

void main()
{
	texCoord = (aPosition.xy+vec2(1,1))/2.0;
	gl_Position = vec4(aPosition.xyz, 1);
}

