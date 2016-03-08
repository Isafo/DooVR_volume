#version 400
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;

out vec3 Position;

uniform mat4 MV;
uniform mat4 P;

void main () {
	Position =  vec3(  MV * vec4(VertexPosition, 1.0));

	// Convert position to clip coordinates and pass along to fragment shader
	gl_Position =  (P * MV) * vec4(VertexPosition, 1.0);

}