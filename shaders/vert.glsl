#version 120

#define LIGHTS_NUMBER 5

attribute vec4 aPosition;
attribute vec3 aNormal;
attribute vec2 aTexCoord;

uniform vec4 uLightPos[LIGHTS_NUMBER];

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uNormalMatrix;
uniform vec3 uEye;

varying vec2 texCoord;
varying vec3 normalVec;
varying vec3 outLight[LIGHTS_NUMBER];
varying vec3 H[LIGHTS_NUMBER];
varying float d[LIGHTS_NUMBER];

void main()
{
	vec3 vertexPos = vec3(uViewMatrix * uModelMatrix * aPosition);
	normalVec = normalize(vec3(uNormalMatrix*vec4(aNormal, 0)));
	vec3 eye = uEye;

	for(int i = 0; i < LIGHTS_NUMBER; i++){
		outLight[i] = vec3(uViewMatrix * vec4(uLightPos[i].xyz,1));
		d[i] = distance(outLight[i], vertexPos);
		outLight[i] = normalize(outLight[i] - vertexPos);
		
		vec3 L = normalize(outLight[i]);
		vec3 V = normalize(eye - vertexPos);
		H[i] = normalize(L + V);
	}

	texCoord = aTexCoord;
	gl_Position = uProjMatrix * uViewMatrix * uModelMatrix * aPosition;
}

