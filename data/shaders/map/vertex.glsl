#version 120
attribute vec3 vertexPosition;
attribute vec2 vertexUV;
varying vec4 fragPos_ms;
varying vec4 fragPos_vs;
varying vec2 uv;
uniform mat4 MVP;

void main()
{
	vec4 v = vec4(vertexPosition, 1.0);
	gl_Position = MVP*v;
	fragPos_ms = v;
	fragPos_vs = gl_Position;
	uv = vertexUV;
}
