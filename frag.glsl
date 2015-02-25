#version 120

/*
* Author: Matheus de Sousa Faria
* CPE 471 - Introduction to Computer Graphics
* Program 3
*/

uniform vec3 UdColor;
uniform vec3 UaColor;
uniform vec3 UsColor;
uniform vec3 UeColor;
uniform float Ushine;

varying vec3 normalVec;
varying vec3 light;
varying vec3 H;

void main()
{
	vec3 L = normalize(light);
	vec3 N = normalize(normalVec);
		
	vec3 norm_H = normalize(H);
	vec3 kd = UdColor;
	vec3 Ia = UaColor;
	vec3 Ie = UeColor;
	vec3 ks = UsColor;
	float n = Ushine;

	vec3 Is = pow(max(dot(N, H), 0), n)*ks;
	vec3 Id = max(dot(N, L), 0)*kd;
	vec3 I = Id + Is + Ia + Ie;
	gl_FragColor = vec4(I.xyz, 1.0);
}
