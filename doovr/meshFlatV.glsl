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

	//vec3 LightIntensity = vec3(0.58039f,0.423529f, 0.282352f);
	if(Selected > 3.0f) {
		//LightIntensity = vec3(0.125f, 0.230468f, 0.69921f);
		LightIntensity = vec3(0.69921f + abs(Selected), 0.230468f ,0.125f);
	} 
	else {
		LightIntensity = vec3(0.69921f, 0.230468f, 0.125f);
	}

	//! Convert position to clip coordinates and pass along to fragment shader
	gl_Position =  (P * MV) * vec4(VertexPosition, 1.0);

}