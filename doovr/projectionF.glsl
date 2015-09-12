#version 400
// Ouput data
//layout(location = 0) out float fragmentdepth;
 
//layout(location = 0) out float FragColor;
layout(location = 0) out vec4 FragColor;

void main(){
    // Not really needed, OpenGL does it anyway
   //fragmentdepth = gl_FragCoord.z;
   FragColor = vec4(gl_PrimitiveID + 1, 1.0f, 1.0f, 1.0f);
   //FragColor = float(gl_PrimitiveID + 1);
   //FragColor = vec4(2.0f, 1.0f, 1.0f, 1.0f);
}