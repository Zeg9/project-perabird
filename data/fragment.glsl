#version 120
varying vec2 uv;
varying vec3 fragmentColor;

uniform sampler2D textureSampler;

void main()
{
	gl_FragColor = texture2D(textureSampler,uv)*vec4(fragmentColor,1);
}
