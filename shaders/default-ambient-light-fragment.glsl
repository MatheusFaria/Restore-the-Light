#version 120

uniform sampler2D uAmbientID;
uniform vec2 uScreenSize;

void main()
{
	vec2 vTexCoord = gl_FragCoord.xy/uScreenSize;
	vec3 Ia_Ie = vec3(texture2D(uAmbientID, vTexCoord));

	gl_FragData[0] = vec4(Ia_Ie, 1);
}
