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
uniform vec3 UdColor;
uniform vec3 UaColor;
uniform vec3 UsColor;
uniform vec3 UeColor;
uniform float Ushine;
uniform int uNormalM;

varying vec3 vCol;
varying vec3 normalVec;
varying vec3 light;
varying vec3 H;

void main()
{
	vec3 vertexPos = vec3(uViewMatrix * uModelMatrix * aPosition);
	normalVec = vec3(uViewMatrix*uModelMatrix*vec4(aNormal, 0));
	vec3 eye = uEye;

	light = uLightPos - vertexPos;
	vec3 L = normalize(light);
	vec3 V = normalize(eye - vertexPos);
	H = normalize(L + V);
	vec3 N = normalize(normalVec);
	vec3 kd = UdColor;
	vec3 Ia = UaColor;
	vec3 Ie = UeColor;
	vec3 ks = UsColor;
	float n = Ushine;

	vec3 Is = pow(max(dot(N, H), 0), n)*ks;
	vec3 Id = max(dot(N, L), 0)*kd;
	vec3 I = Ia + Id + Is + Ie;

	if(uNormalM == 1)
		vCol = 0.5*(normalVec.xyz + 1.0);
	else
		vCol = I.xyz;

	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
}

