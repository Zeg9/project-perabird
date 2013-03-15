#version 120
varying vec4 fragPos;
varying vec2 uv;

uniform sampler2D textureSampler;

void main()
{
	gl_FragColor = texture2D(textureSampler,uv)*(fragPos.y/10.0f);
}
