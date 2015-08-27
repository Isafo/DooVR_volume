#version 400
layout( location = 0 ) out vec4 FragColor;

in vec3 Position;
in vec3 Normal;
in vec4 uv;
in vec4 shadowUV;

uniform vec4 lightPos;
uniform vec4 lightPos2;

uniform sampler2D tex;
uniform sampler2D dTex;


void main () {
	vec3 LightIntensity;
	//vec2 finaluv = vec2(((uv.x/0.02) + 1.0f)/2.0f, ((uv.y/0.02) + 1.0f)/2.0f);
	
	//vec3 ProjCoords = shadowUV.xyz / shadowUV.w;
    //vec2 UVCoords;
    //UVCoords.x = 0.5 * ProjCoords.x + 0.5;
    //UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    //float z = 0.5 * ProjCoords.z + 0.5;
	//UVCoords.x = ProjCoords.x;
    //UVCoords.y = ProjCoords.y;
    //float z = ProjCoords.z;

	//float Depth = texture(dTex, UVCoords).x;
	
	//if(texture( dTex, shadowUV.xy ).z  >  shadowUV.z)
	//{
	//LightIntensity = vec3(0.6f, 0.1294117f, 0.0f);
	//}
	//else
	//{
	//LightIntensity = vec3(0.0f, 0.1294117f, 0.0f);
	//}

	if( (uv.z < 0.02) && (uv.z > 0.019) && texture( dTex, shadowUV.xy ).z  >  shadowUV.z - 0.001){	
		//LightIntensity = vec3(texture(tex, finaluv ));
		LightIntensity = vec3(1.0f, 1.0f, 1.0f);
	} 
	else {
		LightIntensity = vec3(0.6f, 0.1294117f, 0.0f);
	}
	
	vec3 Kd = vec3(0.8f, 0.8f, 0.8f);                // Diffuse reflectivity
	vec3 Ka = vec3(0.1f, 0.1f, 0.1f);                // Ambient reflectivity
	vec3 Ks = vec3( 0.7f, 0.7f, 0.7f);				 // Specular reflectivity
	float Shininess = 8.0f;						 // Specular shininess factor
	vec3 norm = normalize( Normal );			
	vec3 vie = normalize(vec3(-Position));			 // viewDir	
		
	float strength = 0.8f;

	//float distance0 = length( vec3(lPos) - Position);
	
	// 0th
	vec3 s = normalize( vec3(lightPos) - Position ); // lightDir
	vec3 r = reflect( -s, norm );						 // reflectDir
	//vec3 halfwayDir = normalize(lightDir + viewDir);  
	//float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);
	vec3 LI = LightIntensity * (  Ka + Kd * max( dot(s, norm), 0.0 )) + Ks * pow( max( dot(r,vie), 0.0 ), Shininess ) * strength;

	vec3 s2 = normalize( vec3(lightPos2) - Position ); // lightDir
	vec3 r2 = reflect( -s2, norm );						 // reflectDir
	vec3 LI2 = LightIntensity * (  Ka + Kd * max( dot(s2, norm), 0.0 ))  + Ks * pow( max( dot(r2,vie), 0.0 ), Shininess ) * strength;
	
	vec3 newResultLight = (LI + LI2 )/2.0f;
	FragColor = vec4(newResultLight, 1.0);
}