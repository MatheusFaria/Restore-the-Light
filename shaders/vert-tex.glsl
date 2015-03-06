#version 120

attribute vec4 aPosition;
attribute vec2 aTexCoord;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;

varying vec2 vTexCoord;


void main()
{
	vTexCoord = aTexCoord;
	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
}

