#version 120
varying vec4 fragPos_ms;
varying vec4 fragPos_vs;
varying vec2 uv;

uniform sampler2D textureSampler;

void main()
{
	gl_FragColor = texture2D(textureSampler,uv) + (fragPos_vs.z/100.0);
}
