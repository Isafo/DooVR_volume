#version 400
#extension GL_EXT_gpu_shader4 : enable

layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in float Selected;

uniform mat4 MV;
uniform mat4 P;

void main(void) {

	gl_Position =  (P * MV) * vec4(VertexPosition, 1.0);
}