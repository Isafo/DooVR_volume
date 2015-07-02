#version 400
layout( location = 0 ) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec2 UV;

uniform vec4 lightPos;
uniform sampler2D tex;

void main () {
	float lightIntensity = 0.7f;

	// DXT from directX, so we have to invert the UV coordinates 
	//(coord.u, 1.0-coord.v) to fetch the correct texel.
	vec2 invUV = vec2(UV.x, 1.0-UV.y);

	FragColor = texture( tex, invUV ).rgba;
}