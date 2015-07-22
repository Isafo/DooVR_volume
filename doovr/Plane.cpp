#include "Plane.h"
#include "time.h"
#include "math.h"

Plane::Plane(float x, float y, float z, float dX, float dZ) {
	
	int temp = dX * dZ;
	texArray = new texCoords[temp];
	vertexArray = new vertex[temp];
	triangleArray = new triangle[temp * 2];

	vertex tempV;
	texCoords tempTex;
	triangle tempT;

	tempV.y = 0.0f;
	tempV.nx = 0.0f;
	tempV.ny = 1.0f;
	tempV.nz = 0.0f;

	position[0] = x;
	position[1] = y;
	position[2] = z;

	nrofVerts = 0;
	nrofTris = 0;


	for (float i = -dX/2.0f; i < dX/2.0f; i++) {
		for (float j = -dZ/2.0f ; j < dZ / 2.0f; j++) {
			tempV.x = i;
			tempV.z = j;
			vertexArray[nrofVerts] = tempV;
			
			if (fmod(j,2) == 0 && fmod(i,2) == 0) { // om j är jämn o i jämn
				tempTex.u = 0.0f;
				tempTex.v = 0.0f;
			}
			else if (fmod(j, 2) == 0 && fmod(i, 2) != 0) { // om j är jämn o i ojämn
				tempTex.u = 1.0f;
				tempTex.v = 0.0f;
			}
			else if (fmod(j, 2) != 0 && fmod(i, 2) != 0) { // om j är ojämn o i ojämn
				tempTex.u = 1.0f;
				tempTex.v = 1.0f;
			}
			else if (fmod(j, 2) != 0 && fmod(i, 2) == 0) { // om j är ojämn o i jämn
				tempTex.u = 0.0f;
				tempTex.v = 1.0f;
			}
			texArray[nrofVerts] = tempTex;
			nrofVerts++;
		}
	}
	
	for (int i = 0; i < dX - 1 ; i++) {
		for (int j = 0; j < dZ - 1; j++) { 

			tempT.index[0] = i*dX + j; 
			tempT.index[1] = i*dX + j + 1;
			tempT.index[2] = (i + 1)*dX + j;

			triangleArray[nrofTris] = tempT;
			nrofTris++;

			tempT.index[0] = (i + 1)*dX + j;
			tempT.index[1] = i*dX + j + 1;
			tempT.index[2] = (i + 1)*dX + j + 1;

			triangleArray[nrofTris] = tempT;
			nrofTris++;
		}
	}
	
	createBuffers();
}

Plane::~Plane(void) {
	cout << "A box has died." << endl;
}

void Plane::createBuffers() {
	vertex* vertexP;
	triangle* indexP;

	vertexP = &vertexArray[0];
	indexP = &triangleArray[0];
	// Generate one vertex array object (VAO) and bind it
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Generate two buffer IDs
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &indexbuffer);

	// Activate the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Present our vertex coordinates to OpenGL
	glBufferData(GL_ARRAY_BUFFER, nrofVerts * sizeof(vertex),
		vertexP, GL_STREAM_DRAW);

	// Specify how many attribute arrays we have in our VAO
	glEnableVertexAttribArray(0); // Vertex coordinates
	glEnableVertexAttribArray(1); // Normals
	glEnableVertexAttribArray(2); // Texture
	// Specify how OpenGL should interpret the vertex buffer data:
	// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
	// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
	// Type GL_FLOAT
	// Not normalized (GL_FALSE)
	// Stride 8 (interleaved array with 8 floats per vertex)
	// Array buffer offset 0, 3, 6 (offset into first vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GLfloat), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))); // texture


	// Activate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	// Present our vertex indices to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		3 * nrofTris * sizeof(GLuint), indexP, GL_STREAM_DRAW);


	// Deactivate (unbind) the VAO and the buffers again.
	// Do NOT unbind the buffers while the VAO is still bound.
	// The index buffer is an essential part of the VAO state.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Plane::render() {
	glBindVertexArray(vao);
	//glColor3f(color.x, color.y, color.z);

	glDrawElements(GL_TRIANGLES, 3 * nrofTris * sizeof(GLuint), GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex count, type, element array buffer offset)
	glBindVertexArray(0);
}
