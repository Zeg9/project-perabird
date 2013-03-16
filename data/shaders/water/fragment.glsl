#version 120
varying vec4 fragPos_ms;
varying vec4 fragPos_vs;
varying vec2 uv;
uniform vec4 color;
uniform sampler2D textureSampler;
uniform int ticks;

void main()
{
	gl_FragColor = texture2D(textureSampler,uv+ticks*.0005)*color*vec4(1,1,1,1);
}
