#version 400
// Ouput data
//layout(location = 0) out float fragmentdepth;
 
out float FragColor;

void main(){
    // Not really needed, OpenGL does it anyway
   //fragmentdepth = gl_FragCoord.z;
   FragColor = float(gl_PrimitiveID + 1);
}