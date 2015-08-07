#version 400
layout( location = 0 ) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec2 UV;

uniform vec4 lightPos;
uniform sampler2D tex;

const float texDimension = 1536.0f;
const float texScaler =  1.0/texDimension;
const float texOffset = -0.5/texDimension;

const vec2 size = vec2(1536.0f, 1536.0f);
const int samples = 16; // pixels per axis; higher = bigger glow, worse performance
const float quality = 1.0; // lower = smaller glow, better quality

void main () {
	// DXT from directX, so we have to invert the UV coordinates 
	//(coord.u, 1.0-coord.v) to fetch the correct texel.
	vec2 invUV = vec2(UV.x, 1.0-UV.y);

	vec4 source = texture(tex, invUV);
	vec4 sum = vec4(0);
	int diff = (samples - 1) / 2;
	vec2 sizeFactor = vec2(1) / size * quality;

	for (int x = -diff; x <= diff; x++) {
		for (int y = -diff; y <= diff; y++) {
			vec2 offset = vec2(x, y) * sizeFactor;
			sum += texture(tex, invUV + offset);
		}
	}
  
	vec4 color = vec4(0.1f, 0.4f, 0.6f, 1.0f);
  
	FragColor = ((sum / (samples * samples)) + source);
}