#version 400
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in float Selected;

out vec3 Position;
out vec3 LightIntensity;

uniform mat4 MV;
uniform mat4 P;
uniform vec4 lightPos;
uniform vec4 lightPos2;

void main () {
	Position =  vec3(  MV * vec4(VertexPosition, 1.0));

	LightIntensity = vec3(0.6f, 0.1294117f,0.0f);

	// Convert position to clip coordinates and pass along to fragment shader
	gl_Position =  (P * MV) * vec4(VertexPosition, 1.0);

}