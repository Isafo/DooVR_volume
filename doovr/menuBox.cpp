#include "menuBox.h"


menuBox::menuBox(float x, float y, float z, float dX, float dY, float dZ, int tX, int tY, int tXLength, int tYLength, int sideX, int sideY) {
	oType = 'B';
	position[0] = x;
	position[1] = y;
	position[2] = z;
	dim[0] = dX;
	dim[1] = dY;
	dim[2] = dZ;

	GLfloat vertex_array_data[] = {
		//		Vertex									Normals					Texture  
		-dX / 2.0f,	-dY / 2.0f,  dZ / 2.0f,				0.0f, 0.0f, 1.0f,		(sideX*256.0f)/1536, (sideY*256.0f + 256.0f)/1536.0f,				//1 - 0
		dX / 2.0f,  -dY / 2.0f,  dZ / 2.0f,				0.0f, 0.0f, 1.0f,		(sideX*256.0f)/1536, (sideY*256.0f)/1536,							//2 - 1
		dX / 2.0f,   dY / 2.0f,  dZ / 2.0f,				0.0f, 0.0f, 1.0f,		(sideX*256.0f + 256.0f)/1536.0f, (sideY*256.0f)/1536,				//3 - 2
		-dX / 2.0f,  dY / 2.0f,  dZ / 2.0f,				0.0f, 0.0f, 1.0f,		(sideX*256.0f + 256.0f)/1536.0f, (sideY*256.0f + 256.0f)/1536.0f,	//4 - 3 

		-dX / 2.0f, -dY / 2.0f, -dZ / 2.0f,				0.0f, 0.0f, -1.0f,		(sideX*256.0f)/1536, (sideY*256.0f + 256.0f)/1536.0f,				//5 - 4 
		dX / 2.0f,  -dY / 2.0f, -dZ / 2.0f,				0.0f, 0.0f, -1.0f,		(sideX*256.0f)/1536, (sideY*256.0f)/1536,							//6 - 5
		dX / 2.0f,   dY / 2.0f, -dZ / 2.0f,				0.0f, 0.0f, -1.0f,		(sideX*256.0f + 256.0f)/1536.0f, (sideY*256.0f)/1536,				//7 - 6 
		-dX / 2.0f,  dY / 2.0f, -dZ / 2.0f,				0.0f, 0.0f, -1.0f,		(sideX*256.0f + 256.0f)/1536.0f, (sideY*256.0f + 256.0f)/1536.0f,	//8 - 7

		-dX / 2.0f, -dY / 2.0f, dZ / 2.0f,				0.0f, -1.0f, 0.0f,		(sideX*256.0f)/1536, (sideY*256.0f)/1536,							//1 - 8
		dX / 2.0f, -dY / 2.0f, dZ / 2.0f,				0.0f, -1.0f, 0.0f,		(sideX*256.0f + 256.0f)/1536.0f, (sideY*256.0f)/1536,				//2 - 9
		-dX / 2.0f, -dY / 2.0f, -dZ / 2.0f,				0.0f, -1.0f, 0.0f,		(sideX*256.0f)/1536, (sideY*256.0f + 256.0f)/1536.0f,				//5 - 10
		dX / 2.0f,  -dY / 2.0f, -dZ / 2.0f,				0.0f, -1.0f, 0.0f,		(sideX*256.0f + 256.0f)/1536.0f, (sideY*256.0f + 256.0f)/1536.0f,	//6 - 11

		dX / 2.0f,  dY / 2.0f,  dZ / 2.0f,				0.0f, 1.0f, 0.0f,		(tX * 256.0f + tXLength * 256.0f) / 1536.0f, (tY * 256.0f) / 1536.0f,						//3 - 12
		-dX / 2.0f, dY / 2.0f,  dZ / 2.0f,				0.0f, 1.0f, 0.0f,		(tX * 256.0f ) / 1536.0f, (tY*256.0f )/1536.0f,												//4 - 13
		dX / 2.0f,  dY / 2.0f, -dZ / 2.0f,				0.0f, 1.0f, 0.0f,		(tX * 256.0f + tXLength * 256.0f) / 1536.0f, (tY * 256.0f + tYLength*256.0f) / 1536.0f,		//7 - 14
		-dX / 2.0f, dY / 2.0f, -dZ / 2.0f,				0.0f, 1.0f, 0.0f,		(tX * 256.0f) / 1536.0f, (tY * 256.0f + tYLength * 256.0f) / 1536.0f,						//8 - 15

		-dX / 2.0f, -dY / 2.0f,  dZ / 2.0f,				-1.0f, 0.0f, 0.0f,		(sideX*256.0f + 256.0f)/1536.0f, (sideY*256.0f + 256.0f)/1536.0f,		//1 - 16
		-dX / 2.0f,  dY / 2.0f,  dZ / 2.0f,				-1.0f, 0.0f, 0.0f,		(sideX*256.0f)/1536, (sideY*256.0f + 256.0f)/1536.0f,					//4 - 17
		-dX / 2.0f, -dY / 2.0f, -dZ / 2.0f,				-1.0f, 0.0f, 0.0f,		(sideX*256.0f + 256.0f)/1536.0f, (sideY*256.0f)/1536,					//5 - 18
		-dX / 2.0f,  dY / 2.0f, -dZ / 2.0f,				-1.0f, 0.0f, 0.0f,		(sideX*256.0f)/1536, (sideY*256.0f)/1536,								//8 - 19

		dX / 2.0f, -dY / 2.0f,  dZ / 2.0f,				1.0f, 0.0f, 0.0f,		(sideX*256.0f)/1536, (sideY*256.0f)/1536,								//2 - 20
		dX / 2.0f,  dY / 2.0f,  dZ / 2.0f,				1.0f, 0.0f, 0.0f,		(sideX*256.0f + 256.0f)/1536.0f, (sideY*256.0f)/1536,					//3 - 21
		dX / 2.0f, -dY / 2.0f, -dZ / 2.0f,				1.0f, 0.0f, 0.0f,		(sideX*256.0f)/1536, (sideY*256.0f + 256.0f)/1536.0f,					//6 - 22
		dX / 2.0f,  dY / 2.0f, -dZ / 2.0f,				1.0f, 0.0f, 0.0f,		(sideX*256.0f + 256.0f)/1536.0f, (sideY*256.0f + 256.0f)/1536.0f		//7 - 23
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

// TODO: kolla varför den crachar när denna körs
menuBox::~menuBox(void) {
	std::cout << "A box has died." << std::endl;
	//delete[] vertexarray;
	//delete[] indexarray;
	clean();
}

void menuBox::clean() {
	if (glIsVertexArray(vao)) {
		glDeleteVertexArrays(1, &vao);
	}
	vao = 0;

	if (glIsBuffer(vertexbuffer)) {
		glDeleteBuffers(1, &vertexbuffer);
	}
	vertexbuffer = 0;

	if (glIsBuffer(indexbuffer)) {
		glDeleteBuffers(1, &indexbuffer);
	}
	indexbuffer = 0;
}

void menuBox::setDim(float x, float y, float z) {
	vertexarray[2 * 8 + 1] = y;
	vertexarray[3 * 8 + 1] = y;
	vertexarray[6 * 8 + 1] = y;
	vertexarray[7 * 8 + 1] = y;
	vertexarray[12 * 8 + 1] = y;
	vertexarray[13 * 8 + 1] = y;
	vertexarray[14 * 8 + 1] = y;
	vertexarray[15 * 8 + 1] = y;
	vertexarray[17 * 8 + 1] = y;
	vertexarray[19 * 8 + 1] = y;
	vertexarray[21 * 8 + 1] = y;
	vertexarray[23 * 8 + 1] = y;

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

void menuBox::render() {
	glBindVertexArray(vao);
	glDrawElements(GL_QUADS, nverts, GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex count, type, element array buffer offset)
	glBindVertexArray(0);
}