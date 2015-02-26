#version 120

/*
* Author: Matheus de Sousa Faria
* CPE 471 - Introduction to Computer Graphics
* Program 3
*/

attribute vec4 aPosition;
attribute vec3 aNormal;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec3 uLightPos;
uniform vec3 uEye;

varying vec3 normalVec;
varying vec3 light;
varying vec3 H;

void main()
{
	vec3 vertexPos = vec3(uViewMatrix * uModelMatrix * aPosition);
	normalVec = normalize(vec3(uViewMatrix*uModelMatrix*vec4(aNormal, 0)));
	vec3 eye = uEye;

	light = normalize(uLightPos);//normalize(uLightPos - vertexPos);
	vec3 L = normalize(light);
	vec3 V = normalize(eye - vertexPos);
	H = normalize(L + V);


	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
}

