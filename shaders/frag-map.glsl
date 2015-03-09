#version 120

uniform sampler2D uTexID;
uniform sampler2D uAlphaTexID;

uniform int uCompleteGlow;

uniform vec3 UdColor;
uniform vec3 UaColor;
uniform vec3 UsColor;
uniform vec3 UeColor;
uniform float Ushine;

varying vec2 vTexCoord;
varying vec3 vVertexPos;
varying vec3 vNormal;

vec3 grayScale(vec3 pixel){
	/*
	To get a better gray scale:

	http://hyperphysics.phy-astr.gsu.edu/hbase/vision/colcon.html
	The human eye does not see r g b equally, so, to gray scale a color
	we need to use the values (0.64, 0.32, 0.2) for (r, g, b)

	grayIntesity = r*0.64 + g*0.32 + b*0.2

	return vec3(dot(pixel, vec3(0.64, 0.32, 0.2)));
	*/

	return vec3(dot(pixel, vec3(1.0/3.0)));
}

vec3 alphaMap(vec3 pixel){
	if(length(pixel) > 0.9){
		return vec3(1);
	}
	return vec3(0);
}

void main()
{
	/*
	Output:
	0 : Texture (Diffuse Color) (rgb)
	1 : Alpha map (rgb)
	2 : Position (xyz)
	3 : Normals (xyz)
	4 : Specular Parameters (ks.rgb, shinness power n)
	5 : Ambient + Emission (rgb)
	*/
	vec4 texel = texture2D(uTexID, vTexCoord);
	vec4 alphaTexel = texture2D(uAlphaTexID, vTexCoord);

	gl_FragData[0] = texel;
	
	if(uCompleteGlow == 1)
		gl_FragData[1] = texel;
	else
		gl_FragData[1] = texel*alphaTexel;

	gl_FragData[2] = vec4(vVertexPos, 1);
	gl_FragData[3] = vec4(normalize(vNormal), 1);

	gl_FragData[4] = vec4(UsColor, Ushine);

	gl_FragData[5] = vec4(vec3(UaColor + UeColor), 1);
}
