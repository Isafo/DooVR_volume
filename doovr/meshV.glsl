#version 400
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in float Selected;

out vec3 Position;
out vec3 Normal;
out vec3 resultLight;
out vec4 uv;

uniform mat4 MV;
uniform mat4 P;

uniform mat4 modelMatrix;

uniform vec3 wandPos;
uniform vec3 wandDirr;

void main () 
{
	Position =  vec3( MV * vec4(VertexPosition, 1.0));
	Normal = normalize(mat3(MV) * VertexNormal);	

	vec4 posWorld = modelMatrix * vec4(VertexPosition, 1.0);
	vec3 posVec = vec3(posWorld) - wandPos;
	
	float dotProd = dot(posVec, wandDirr);
	vec3 nWandDirr = dotProd*wandDirr;
	
	vec3 orthogonal = posVec - nWandDirr;
	
	vec3 up = cross(vec3(0.0, 1.0, 0.0), wandDirr);
	float cos = dot(orthogonal,up)/(length(orthogonal)*length(up));
	float sin = sqrt(1-cos*cos);
	
	uv = vec4(cos*length(orthogonal), sin*length(orthogonal),length(orthogonal), 1.0);
	
	gl_Position =  (P * MV) * vec4(VertexPosition, 1.0);

}