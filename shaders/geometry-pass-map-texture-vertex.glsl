#version 120

attribute vec4 aPosition;
attribute vec3 aNormal;
attribute vec2 aTexCoord;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;

varying vec2 vTexCoord;
varying vec3 vVertexPos;
varying vec3 vNormal;

void main()
{
	vTexCoord = aTexCoord;
	vVertexPos = vec3(uViewMatrix * uModelMatrix * aPosition);
	vNormal = normalize(vec3(uNormalMatrix*vec4(aNormal, 0)));

	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
}

