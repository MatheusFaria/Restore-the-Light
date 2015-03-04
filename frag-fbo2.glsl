#version 120

uniform sampler2D uTextureID;

varying vec2 texCoord;

void main()
{
	gl_FragColor = texture2D(uTextureID, texCoord) + vec4(0, 1, 0, 1);
}
