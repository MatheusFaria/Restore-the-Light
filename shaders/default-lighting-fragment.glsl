#version 120
    
#define UNDEFINED   -1
#define DIRECTIONAL  0
#define POINT_LIGHT  1
#define SPOT_LIGHT   2

uniform sampler2D uDiffuseID;
uniform sampler2D uPositionID;
uniform sampler2D uNormalID;
uniform sampler2D uSpecularID;
uniform sampler2D uAmbientID;

uniform mat4 uViewMatrix;
uniform vec3 uEye;

uniform vec4 uLightPos;
uniform vec3 uLightDir;
uniform vec3 uLightFallOff;
uniform vec3 uLightColor;
uniform float uCutOffAngle;

varying vec2 vTexCoord;

void main()
{
	vec4 specularParams = texture2D(uSpecularID, vTexCoord);

	vec3 light = vec3(uViewMatrix * vec4(uLightPos.xyz, 1));
	vec3 lightDir = vec3(uViewMatrix * vec4(uLightDir, 0));
	float lightType = uLightPos.w;

	vec3 vertexPos = vec3(texture2D(uPositionID, vTexCoord));
	vec3 normalVec = vec3(texture2D(uNormalID, vTexCoord));
	vec3 N = normalize(normalVec);
	
	vec3 kd = vec3(texture2D(uDiffuseID, vTexCoord));
	vec3 Ia_Ie = vec3(texture2D(uAmbientID, vTexCoord));
	vec3 ks = specularParams.rgb;
	float n = specularParams.a;

	float d = distance(light, vertexPos);

	vec3 L;
	if(lightType == DIRECTIONAL)
		L = normalize(lightDir);
	else
		L = normalize(light - vertexPos);

	vec3 V = normalize(uEye - vertexPos);
		
	vec3 H = normalize(L + V);

	vec3 Is = pow(max(dot(N, H), 0), n)*ks;
	vec3 Id = max(dot(N, L), 0)*kd;

	float a = uLightFallOff.x, b = uLightFallOff.y, c = uLightFallOff.z;
	vec3 Ic = uLightColor;

	vec3 I = vec3(0);
	if(lightType == DIRECTIONAL){
		I = Ic*(Id + Is);
	}
	else if(lightType == SPOT_LIGHT){
		vec3 pixelToLight = normalize(vertexPos - light);

		if(cos(uCutOffAngle) < dot(lightDir, pixelToLight) && cos(uCutOffAngle) != 1){
			I = Ic*(Id + Is)/(a + b*d + c*d*d);
			I = (1 - (1 - dot(lightDir, pixelToLight))/(1 - cos(uCutOffAngle)))*I;
		}
	}
	else{
		I = Ic*(Id + Is)/(a + b*d + c*d*d);
	}

	gl_FragData[0] = vec4(I, 1);
}
