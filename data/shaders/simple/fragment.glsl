#version 120
varying vec4 fragPos_ms;
varying vec4 fragPos_vs;
varying vec2 uv;
uniform vec4 color;

uniform sampler2D textureSampler;

void main()
{
	gl_FragColor = texture2D(textureSampler,uv)*color;
}
