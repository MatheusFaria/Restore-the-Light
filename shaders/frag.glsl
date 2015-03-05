#version 120

#define LIGHTS_NUMBER 5

uniform vec3 UdColor;
uniform vec3 UaColor;
uniform vec3 UsColor;
uniform vec3 UeColor;
uniform float Ushine;

uniform sampler2D uTextureID;

uniform vec3 uLightFallOff[LIGHTS_NUMBER];
uniform vec3 uLightColor[LIGHTS_NUMBER];

varying vec2 texCoord;
varying vec3 normalVec;
varying vec3 outLight[LIGHTS_NUMBER];
varying vec3 H[LIGHTS_NUMBER];
varying float d[LIGHTS_NUMBER];

void main()
{

	vec3 N = normalize(normalVec);
	
	vec3 kd = UdColor;
	vec3 Ia = UaColor;
	vec3 Ie = UeColor;
	vec3 ks = UsColor;
	float n = Ushine;

	vec3 I = vec3(0);
	for(int i = 0; i < LIGHTS_NUMBER; i++){
		//float d = distance(vec4(outLight[i], 1), gl_FragCoord);
		vec3 L = normalize(outLight[i]);
		
		vec3 norm_H = normalize(H[i]);

		vec3 Is = pow(max(dot(N, H[i]), 0), n)*ks;
		vec3 Id = max(dot(N, L), 0)*kd;

		float a = uLightFallOff[i].x, b = uLightFallOff[i].y, c = uLightFallOff[i].z;
		vec3 Ic = uLightColor[i];

		I += Ic*(Id + Is)/(a + b*d[i] + c*d[i]*d[i]);
		//I += Ic*(Id + Is)/(a + b*d + c*d*d);
	}
	I +=  Ia + Ie;

	vec4 tex = texture2D(uTextureID, texCoord);
	tex.w = 1;

	//gl_FragColor = tex + (vec4(I.xyz, 1.0) - vec4(1))*vec4(0, 0, 0, 1);
	gl_FragData[0] = tex + (vec4(I.xyz, 1.0) - vec4(1))*vec4(0, 0, 0, 1);
	//gl_FragColor = vec4(I.xyz, 1.0);
}
