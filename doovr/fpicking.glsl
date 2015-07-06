#version 400
out vec3 FragColor;


void main () {
	//FragColor = vec3(1.0f, 1.0f, 0.0f);
	FragColor = vec3(1.0f, 1.0f, float(gl_PrimitiveID + 1));
}