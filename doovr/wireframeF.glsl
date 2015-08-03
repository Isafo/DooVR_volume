#version 400
#extension GL_EXT_gpu_shader4 : enable

layout( location = 0 ) out vec4 FragColor;

in vec3 rLight;
noperspective in vec3 dist;

const vec4 WIRE_COL = vec4(1.0f, 0.0f , 0.0f, 1.0f);

void main(void) {
	float d = min(dist[0], min( dist[1], dist[2]));
	float I = exp2(-2 * d * d);
	//FragColor = I * WIRE_COL + (1.0 - I) * vec4(rLight, 1.0);

	FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}