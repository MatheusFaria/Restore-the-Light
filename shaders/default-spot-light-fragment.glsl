#version 120

uniform sampler2D uDiffuseID;
uniform sampler2D uPositionID;
uniform sampler2D uNormalID;
uniform sampler2D uSpecularID;

uniform vec3 uEye;

uniform vec3 uLightColor;
uniform vec3 uLightPos; // In ViewSpace
uniform vec3 uLightDir; // In ViewSpace
uniform vec3 uLightFallOff;
uniform float uLightCutOffAngle; // Cut Off Angle Cosine

uniform vec2 uScreenSize;

void main()
{
	vec2 vTexCoord = gl_FragCoord.xy/uScreenSize;

	vec3 vertexPos = vec3(texture2D(uPositionID, vTexCoord));

	vec3 pixelToLight = normalize(vertexPos - uLightPos);

	vec3 I = vec3(0);
	if(uLightCutOffAngle < dot(uLightDir, pixelToLight) && uLightCutOffAngle != 1.0){
		
		vec4 specularParams = texture2D(uSpecularID, vTexCoord);

		vec3 normalVec = vec3(texture2D(uNormalID, vTexCoord));
		vec3 N = normalize(normalVec);
	
		vec3 kd = vec3(texture2D(uDiffuseID, vTexCoord));
		vec3 ks = specularParams.rgb;
		float n = specularParams.a;

		float d = distance(uLightPos, vertexPos);

		vec3 L = normalize(uLightPos - vertexPos);
		vec3 V = normalize(uEye - vertexPos);
		vec3 H = normalize(L + V);

		vec3 Is = pow(max(dot(N, H), 0), n)*ks;
		vec3 Id = max(dot(N, L), 0)*kd;

		float a = uLightFallOff.x, b = uLightFallOff.y, c = uLightFallOff.z;
		vec3 Ic = uLightColor;

		I = Ic*(Id + Is)/(a + b*d + c*d*d);
		I = (1 - (1 - dot(uLightDir, pixelToLight))/(1 - uLightCutOffAngle))*I;
	}
	
	gl_FragData[0] = vec4(I, 1);
}
