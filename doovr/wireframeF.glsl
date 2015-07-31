#version 400
#extension GL_EXT_gpu_shader4 : enable

noperspective in vec3 dist;

const vec4 WIRE_COL = vec4(1.0f, 0.0f , 0.0f, 1.0f);
const vec4 FILL_COL = vec4(0.8 , 0.3 , 0.3 , 1.0f);

void main(void) {
	float d = min(dist[0], min( dist[1], dist[2]));
	float I = exp2(-2 * d * d);
	//gl_FragColor = I * WIRE_COL + (1.0 - I) * FILL_COL;

	gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}