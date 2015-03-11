#version 120

uniform sampler2D uDiffuseID;
uniform sampler2D uPositionID;
uniform sampler2D uNormalID;
uniform sampler2D uSpecularID;
uniform sampler2D uAmbientID;

uniform vec3 uEye;

uniform vec3 uLightDir; // In View Space
uniform vec3 uLightColor;

uniform int uAmbientPass;

varying vec2 vTexCoord;

void main()
{
	vec4 specularParams = texture2D(uSpecularID, vTexCoord);

	vec3 vertexPos = vec3(texture2D(uPositionID, vTexCoord));
	vec3 normalVec = vec3(texture2D(uNormalID, vTexCoord));
	vec3 N = normalize(normalVec);
	
	vec3 kd = vec3(texture2D(uDiffuseID, vTexCoord));
	vec3 Ia_Ie = vec3(texture2D(uAmbientID, vTexCoord));
	vec3 ks = specularParams.rgb;
	float n = specularParams.a;

	vec3 L = normalize(uLightDir);

	vec3 V = normalize(uEye - vertexPos);
		
	vec3 H = normalize(L + V);

	vec3 Is = pow(max(dot(N, H), 0), n)*ks;
	vec3 Id = max(dot(N, L), 0)*kd;

	vec3 Ic = uLightColor;

	vec3 I = Ic*(Id + Is);
	
	if(uAmbientPass == 1){
		I += Ia_Ie;
	}

	gl_FragData[0] = vec4(I, 1);
}
