#version 400
layout(location = 0) in vec3 VertexPosition;
layout(location = 1) in vec3 VertexNormal;
layout(location = 2) in float Selected;

out vec3 Position;
out vec3 Normal;
out vec3 resultLight;
out vec4 uv;
out vec4 shadowUV;

uniform mat4 MV;
uniform mat4 P;

uniform mat4 LMVP;
uniform mat4 PP;
uniform vec3 IntersectionP;
uniform vec3 IntersectionN;
//uniform float Radius;

uniform mat4 modelMatrix;

uniform vec3 wandPos;
uniform vec3 wandDirr;

void main () 
{
	Position =  vec3( MV * vec4(VertexPosition, 1.0));
	Normal = normalize(mat3(MV) * VertexNormal);	

	//vec4 posWorld = modelMatrix * vec4(VertexPosition, 1.0);
	//vec3 posVec = vec3(posWorld) - wandPos;
	
	//float dotProd = dot(posVec, wandDirr);
	//vec3 nWandDirr = dotProd*wandDirr;
	
	//vec3 orthogonal = posVec - nWandDirr;
	//float oLength = length(orthogonal);
	
	//vec3 up = cross(vec3(0.0, 1.0, 0.0), wandDirr);
	//float cos = dot(orthogonal,up)/(length(orthogonal)*length(up));
	//float sin = sqrt(1-cos*cos);
	//if (dotProd < 0.02f && dotProd > -0.02f ) {
	//	uv = vec4(cos*oLength, sin*oLength,oLength, dotProd);
	//}
	//else {
	//	//vec3 sphereLength = vec3(VertexPosition) - Intersection;
	//	if(oLength < 0.02)
	//	{
	//		uv = vec4(cos*oLength, sin*oLength,-1.0f,dotProd);
	//	}
	//	else {
	//	uv = vec4(cos*oLength, sin*oLength,-1.0f,-1.0f);
	//	}
		
	//}

	//vec4 posWorld = modelMatrix * vec4(VertexPosition, 1.0);
	vec3 posVec = vec3(VertexPosition) - IntersectionP;
	
	float dotProd = dot(posVec, IntersectionN);
	vec3 nIntersectionN = dotProd*IntersectionN;
	
	vec3 orthogonal = posVec - nIntersectionN;
	float oLength = length(orthogonal);
	
	vec3 up = cross(vec3(0.0, 1.0, 0.0), IntersectionN);
	float cos = dot(orthogonal,up)/(length(orthogonal)*length(up));
	float sin = sqrt(1-cos*cos);
	//if (dotProd < 0.02f && dotProd > -0.02f ) {
	vec3 sphereLength = vec3(VertexPosition) - IntersectionP;
		uv = vec4(cos*oLength, sin*oLength, oLength, length(sphereLength));
	//}
	//else {
	//	//vec3 sphereLength = vec3(VertexPosition) - Intersection;
	//	if(oLength < 0.02)
	//	{
	//		uv = vec4(cos*oLength, sin*oLength,-1.0f,dotProd);
	//	}
	//	else {
	//	uv = vec4(cos*oLength, sin*oLength,-1.0f,-1.0f);
	//	}
		
	//}
	
	shadowUV = ( LMVP) * vec4(VertexPosition, 1.0f);

	gl_Position =  (P * MV) * vec4(VertexPosition, 1.0);

}