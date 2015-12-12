#include "Box.h"

Box::Box(float x, float y, float z, float dX, float dY, float dZ) {
	oType = 'B';
	position[0] = x;
	position[1] = y;
	position[2] = z;
	createBox(dX, dY, dZ);
}


Box::~Box(void) {
	std::cout << "A box has died." << std::endl;
	delete[] vertexarray;
	delete[] indexarray;
}

void Box::createBox(float dX, float dY, float dZ) {

	GLfloat vertex_array_data[] = {
		//		Vertex										 Normals					 Texture  						Normals					Texture  
			-dX / 2.0f, -dY / 2.0f, dZ / 2.0f,			0.0f, 0.0f, 1.0f,				0, 1,	//1 - 0
			dX / 2.0f, -dY / 2.0f, dZ / 2.0f,			0.0f, 0.0f, 1.0f,				0, 0,							//2 - 1
			dX / 2.0f, dY / 2.0f, dZ / 2.0f,			0.0f, 0.0f, 1.0f,				1 , 0,	//3 - 2
			-dX / 2.0f, dY / 2.0f, dZ / 2.0f,			0.0f, 0.0f, 1.0f,				1 , 1,	//4 - 3 

			-dX / 2.0f, -dY / 2.0f, -dZ / 2.0f,			0.0f, 0.0f, -1.0f,				0, 1,	//5 - 4 
			dX / 2.0f, -dY / 2.0f, -dZ / 2.0f,			0.0f, 0.0f, -1.0f,				0, 0,	//6 - 5
			dX / 2.0f, dY / 2.0f, -dZ / 2.0f,			0.0f, 0.0f, -1.0f,				1, 0,	//7 - 6 
			-dX / 2.0f, dY / 2.0f, -dZ / 2.0f,			0.0f, 0.0f, -1.0f,				1, 1,	//8 - 7

			-dX / 2.0f, -dY / 2.0f, dZ / 2.0f,			0.0f, -1.0f, 0.0f,				0, 0,							//1 - 8
			dX / 2.0f, -dY / 2.0f, dZ / 2.0f,			0.0f, -1.0f, 0.0f,				1, 0,	//2 - 9
			-dX / 2.0f, -dY / 2.0f, -dZ / 2.0f,			0.0f, -1.0f, 0.0f,				0, 1,	//5 - 10
			dX / 2.0f, -dY / 2.0f, -dZ / 2.0f,			0.0f, -1.0f, 0.0f,				1, 1,	//6 - 11

			dX / 2.0f, dY / 2.0f, dZ / 2.0f,			0.0f, 1.0f, 0.0f,				1, 0,	//3 - 12
			-dX / 2.0f, dY / 2.0f, dZ / 2.0f,			0.0f, 1.0f, 0.0f,				0, 0,	//4 - 13
			dX / 2.0f, dY / 2.0f, -dZ / 2.0f,			0.0f, 1.0f, 0.0f,				1, 1,	//7 - 14
			-dX / 2.0f, dY / 2.0f, -dZ / 2.0f,			0.0f, 1.0f, 0.0f,				0, 1,	//8 - 15

			-dX / 2.0f, -dY / 2.0f, dZ / 2.0f,			-1.0f, 0.0f, 0.0f,				1, 1,	//1 - 16
			-dX / 2.0f, dY / 2.0f, dZ / 2.0f,			-1.0f, 0.0f, 0.0f,				0, 1,	//4 - 17
			-dX / 2.0f, -dY / 2.0f, -dZ / 2.0f,			-1.0f, 0.0f, 0.0f,				1, 0,	//5 - 18
			-dX / 2.0f, dY / 2.0f, -dZ / 2.0f,			-1.0f, 0.0f, 0.0f,				0, 0,	//8 - 19

			dX / 2.0f, -dY / 2.0f, dZ / 2.0f,			1.0f, 0.0f, 0.0f,				0, 0,	//2 - 20
			dX / 2.0f, dY / 2.0f, dZ / 2.0f,			1.0f, 0.0f, 0.0f,				1,0,	//3 - 21
			dX / 2.0f, -dY / 2.0f, -dZ / 2.0f,			1.0f, 0.0f, 0.0f,				0, 1,	//6 - 22
			dX / 2.0f, dY / 2.0f, -dZ / 2.0f,			1.0f, 0.0f, 0.0f,				1, 1	//7 - 23
		};

	static const GLuint index_array_data[] = {
		0, 1, 2, 3,			// Front
		7, 6, 5, 4,			// Back
		10, 11, 9, 8,		// Bottom 
		12, 14, 15, 13,		// Top
		20, 22, 23, 21,		// Right
		16, 17, 19, 18		// Left
	};

	nverts = 24;
	nquads = 6;

	vertexarray = new GLfloat[8 * nverts]; // coordinates, normals and texture coordinates
	indexarray = new GLuint[4 * nquads];

	for (int i = 0; i < 8 * nverts; i++) {
		vertexarray[i] = vertex_array_data[i];
	}

	for (int i = 0; i < 4 * nquads; i++) {
		indexarray[i] = index_array_data[i];
	}

	// Generate one vertex array object (VAO) and bind it
	glGenVertexArrays(1, &(vao));
	glBindVertexArray(vao);

	// Generate two buffer IDs
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &indexbuffer);

	// Activate the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Present our vertex coordinates to OpenGL
	glBufferData(GL_ARRAY_BUFFER,
				 8 * nverts * sizeof(GLfloat), vertexarray, GL_STATIC_DRAW);

	// Specify how many attribute arrays we have in our VAO
	glEnableVertexAttribArray(0); // Vertex coordinates
	glEnableVertexAttribArray(1); // Normals
	glEnableVertexAttribArray(2); // Texture coordinates
	
	// Specify how OpenGL should interpret the vertex buffer data:
	// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
	// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
	// Type GL_FLOAT
	// Not normalized (GL_FALSE)
	// Stride 8 floats (interleaved array with 8 floats per vertex)
	// Array buffer offset 0, 3 or 6 floats (offset into first vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
						  8 * sizeof(GLfloat), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
						  8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
						  8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))); // texcoords

	// Activate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	// Present our vertex indices to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 4 * nquads * sizeof(GLuint), indexarray, GL_STATIC_DRAW);

	// Deactivate (unbind) the VAO and the buffers again.
	// Do NOT unbind the index buffer while the VAO is still bound.
	// The index buffer is an essential part of the VAO state.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Box::render() {
	glBindVertexArray(vao);
	glDrawElements(GL_QUADS, nverts, GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex count, type, element array buffer offset)
	glBindVertexArray(0);
}