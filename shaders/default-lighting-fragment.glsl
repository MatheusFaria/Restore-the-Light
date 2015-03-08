#version 120

uniform sampler2D uDiffuseID;
uniform sampler2D uPositionID;
uniform sampler2D uNormalID;

uniform mat4 uViewMatrix;
uniform vec3 uEye;

uniform vec4 uLightPos;
uniform vec3 uLightFallOff;
uniform vec3 uLightColor;

varying vec2 vTexCoord;

void main()
{
	vec3 light = vec3(uViewMatrix * vec4(uLightPos.xyz, 1));
	vec3 vertexPos = vec3(texture2D(uPositionID, vTexCoord));
	vec3 normalVec = vec3(texture2D(uNormalID, vTexCoord));
	vec3 N = normalize(normalVec);
	
	vec3 kd = vec3(0.91, 0.782, 0.82);//vec3(texture2D(uDiffuseID, vTexCoord));
	vec3 Ia = vec3(1, 0, 0);//UaColor;
	vec3 Ie = vec3(0, 0, 0);//UeColor;
	vec3 ks = vec3(1.0, 0.913, 0.8);//UsColor;
	float n = 200.0;//Ushine;

	float d = length(light);

	vec3 L = normalize(light - vertexPos);
	vec3 V = normalize(uEye - vertexPos);
		
	vec3 H = normalize(L + V);

	vec3 Is = pow(max(dot(N, H), 0), n)*ks;
	vec3 Id = max(dot(N, L), 0)*kd;

	float a = uLightFallOff.x, b = uLightFallOff.y, c = uLightFallOff.z;
	vec3 Ic = uLightColor;

	//vec3 I = Ic*(Id + Is)/(a + b*d + c*d*d);
	vec3 I;
	I = Ic*(Id + Is);
	//I +=  Ia + Ie;

	gl_FragData[0] = vec4(I, 1);
}
