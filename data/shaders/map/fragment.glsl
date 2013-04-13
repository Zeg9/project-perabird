#version 120
varying vec4 fragPos_ms;
varying vec4 fragPos_vs;
varying vec2 uv;
uniform vec4 color;

uniform sampler2D textureSampler;

void main()
{
	gl_FragColor = texture2D(textureSampler,uv);
	gl_FragColor *= color;
	gl_FragColor *= fragPos_ms.y/10.0; // higher = lighter
	gl_FragColor += fragPos_vs.z/250.0; // fog
}
