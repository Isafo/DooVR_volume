#include "DynamicMesh.h"
#include "time.h"
#include "math.h"
#include <algorithm>
#include <iterator>
#include <sstream>
#include <fstream>
#include <string>

#define M_PI 3.14159265358979323846
#define EPSILON 0.000001

DynamicMesh::DynamicMesh() {
	vertexArray = new vertex[MAX_NR_OF_VERTICES];
	vInfoArray = new vInfo[MAX_NR_OF_VERTICES];

	triangleArray = new triangle[MAX_NR_OF_TRIANGLES];
	triEPtr = new int[MAX_NR_OF_TRIANGLES];

	e = new halfEdge[MAX_NR_OF_EDGES];

	//sTail = new sVert;
	/*sHead = new sVert;
	sTail->next = sHead;
	sTail->vec[0] = 100.0f;
	sTail->vec[1] = 100.0f;
	sTail->vec[2] = 100.0f;
	sHead->next = sTail;
	sHead->vec[0] = 100.0f;
	sHead->vec[1] = 100.0f;
	sHead->vec[2] = 100.0f;
	*///sMid = sHead;
	//sNR = 0;

	position[0] = 0.0f; position[1] = 0.15f; position[2] = 0.0f;

	orientation[0] = 1.0f; orientation[1] = 0.0f; orientation[2] = 0.0f; orientation[3] = 0.0f;
	orientation[4] = 0.0f; orientation[5] = 1.0f; orientation[6] = 0.0f; orientation[7] = 0.0f;
	orientation[8] = 0.0f; orientation[9] = 0.0f; orientation[10] = 1.0f; orientation[11] = 0.0f;
	orientation[12] = 0.0f; orientation[13] = 0.0f; orientation[14] = 0.0f; orientation[15] = 1.0f;
}

DynamicMesh::DynamicMesh(std::string fileName) {
	//vertexArray = new vertex[MAX_NR_OF_VERTICES];
	//vInfoArray = new vInfo[MAX_NR_OF_VERTICES];

	//triangleArray = new triangle[MAX_NR_OF_TRIANGLES];
	//triEPtr = new int[MAX_NR_OF_TRIANGLES];

	//e = new halfEdge[MAX_NR_OF_EDGES];

	load(fileName);

	createBuffers();
}


DynamicMesh::~DynamicMesh(void) {

}


void DynamicMesh::createBuffers() {
	triangle* indexP;
	dBufferData * vertexP;

	//vertexP = &vertexArray[0];
	//indexP = &triangleArray[0];

	// Generate one vertex array object (VAO) and bind it
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Generate two buffer IDs
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &indexbuffer);

	// Activate the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Present our vertex coordinates to OpenGL
	glBufferData(GL_ARRAY_BUFFER,
		(MAX_NR_OF_VERTICES)*sizeof(dBufferData), NULL, GL_STREAM_DRAW);

	vertexP = (dBufferData*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(dBufferData) *MAX_NR_OF_VERTICES,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 0; i < MAX_NR_OF_VERTICES; i++) {
		vertexP[i].x = vertexArray[i].xyz[0];
		vertexP[i].y = vertexArray[i].xyz[1];
		vertexP[i].z = vertexArray[i].xyz[2];
		vertexP[i].nx = vertexArray[i].nxyz[0];
		vertexP[i].ny = vertexArray[i].nxyz[1];
		vertexP[i].nz = vertexArray[i].nxyz[2];
		vertexP[i].selected = vInfoArray[i].selected;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// Specify how many attribute arrays we have in our VAO
	glEnableVertexAttribArray(0); // Vertex coordinates
	glEnableVertexAttribArray(1); // Normals
	glEnableVertexAttribArray(2); // Selected
	// Specify how OpenGL should interpret the vertex buffer data:
	// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
	// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
	// Type GL_FLOAT
	// Not normalized (GL_FALSE)
	// Stride 8 (interleaved array with 8 floats per vertex)
	// Array buffer offset 0, 3, 6 (offset into first vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(dBufferData), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(dBufferData), (void*)(3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(dBufferData), (void*)(6 * sizeof(GLfloat))); // normals

	// Activate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	// Present our vertex indices to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		(MAX_NR_OF_TRIANGLES)*sizeof(triangle), NULL, GL_STREAM_DRAW);

	indexP = (triangle*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(triangle) * MAX_NR_OF_TRIANGLES,
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 0; i < MAX_NR_OF_TRIANGLES; i++) {
		indexP[i].index[0] = triangleArray[i].index[0];
		indexP[i].index[1] = triangleArray[i].index[1];
		indexP[i].index[2] = triangleArray[i].index[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// Deactivate (unbind) the VAO and the buffers again.
	// Do NOT unbind the buffers while the VAO is still bound.
	// The index buffer is an essential part of the VAO state.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DynamicMesh::updateOGLData() {
	triangle* indexP;
	dBufferData* vertexP;

	//KAN BEHÖVAS VETTE
	//vertexP = &vertexArray[0];

	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

	vertexP = (dBufferData*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(dBufferData)*(vertexCap + 1),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 0; i <= vertexCap; i++) {
		vertexP[i].x = vertexArray[i].xyz[0];
		vertexP[i].y = vertexArray[i].xyz[1];
		vertexP[i].z = vertexArray[i].xyz[2];
		vertexP[i].nx = vertexArray[i].nxyz[0];
		vertexP[i].ny = vertexArray[i].nxyz[1];
		vertexP[i].nz = vertexArray[i].nxyz[2];
		vertexP[i].selected = vInfoArray[i].selected;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	// Specify how many attribute arrays we have in our VAO
	glEnableVertexAttribArray(0); // Vertex coordinates
	glEnableVertexAttribArray(1); // Normals
	glEnableVertexAttribArray(2); // selected

	// Specify how OpenGL should interpret the vertex buffer data:
	// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
	// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
	// Type GL_FLOAT
	// Not normalized (GL_FALSE)
	// Stride 8 (interleaved array with 8 floats per vertex)
	// Array buffer offset 0, 3, 6 (offset into first vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(dBufferData), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(dBufferData), (void*)(3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
		sizeof(dBufferData), (void*)(6 * sizeof(GLfloat))); // selected

	// Activate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

	// Present our vertex <indices to OpenGL
	indexP = (triangle*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(triangle) * (triangleCap + 1),
		GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

	for (int i = 0; i <= triangleCap; i++) {
		indexP[i].index[0] = triangleArray[i].index[0];
		indexP[i].index[1] = triangleArray[i].index[1];
		indexP[i].index[2] = triangleArray[i].index[2];
	}

	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	// Deactivate (unbind) the VAO and the buffers again.
	// Do NOT unbind the buffers while the VAO is still bound.
	// The index buffer is an essential part of the VAO state.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DynamicMesh::cleanBuffer() {

	if (vertexRange != 0) {

		
		dBufferData* vertexP;

		//KAN BEHÖVAS VETTE
		//vertexP = &vertexArray[0];

		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);

		vertexP = (dBufferData*)glMapBufferRange(GL_ARRAY_BUFFER, vertexCap + 1, sizeof(dBufferData) * (vertexRange),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

		for (int i = 0; i < vertexRange; i++) {
			vertexP[i].x = 1000;
			vertexP[i].y = 1000;
			vertexP[i].z = 1000;
			vertexP[i].nx = 0;
			vertexP[i].ny = 0;
			vertexP[i].nz = -1;
			vertexP[i].selected = 0;
		}
		glUnmapBuffer(GL_ARRAY_BUFFER);

		// Specify how many attribute arrays we have in our VAO
		glEnableVertexAttribArray(0); // Vertex coordinates
		glEnableVertexAttribArray(1); // Normals
		glEnableVertexAttribArray(2); // selected

		// Specify how OpenGL should interpret the vertex buffer data:
		// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
		// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
		// Type GL_FLOAT
		// Not normalized (GL_FALSE)
		// Stride 8 (interleaved array with 8 floats per vertex)
		// Array buffer offset 0, 3, 6 (offset into first vertex)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
			sizeof(dBufferData), (void*)0); // xyz coordinates
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
			sizeof(dBufferData), (void*)(3 * sizeof(GLfloat))); // normals
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,
			sizeof(dBufferData), (void*)(6 * sizeof(GLfloat))); // selected
	}

	if (triangleRange != 0) {
		triangle* indexP;

		// Activate the index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);

		// Present our vertex <indices to OpenGL
		indexP = (triangle*)glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, triangleCap + 1, sizeof(triangle) * (triangleRange),
			GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT);

		for (int i = 0; i < triangleRange; i++) {
			indexP[i].index[0] = 0;
			indexP[i].index[1] = 0;
			indexP[i].index[2] = 0;
		}

		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

		// Deactivate (unbind) the VAO and the buffers again.
		// Do NOT unbind the buffers while the VAO is still bound.
		// The index buffer is an essential part of the VAO state.
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void DynamicMesh::render() {
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, (triangleCap + 1) * sizeof(triangle), GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex uN, type, element array buffer offset)
	glBindVertexArray(0);
}

void DynamicMesh::sphereSubdivide(float rad) {

	vertexArray = new vertex[MAX_NR_OF_VERTICES];
	vInfoArray = new vInfo[MAX_NR_OF_VERTICES];

	triangleArray = new triangle[MAX_NR_OF_TRIANGLES];
	triEPtr = new int[MAX_NR_OF_TRIANGLES];

	e = new halfEdge[MAX_NR_OF_EDGES];

	float stepRad = MIN_LENGTH;
	float tempP1[3];
	sVert tempSV;

	std::vector<int> changedVertices;
	changedVertices.reserve(10000);

	nrofVerts = 6; vertexCap = 5;
	nrofTris = 8; triangleCap = 7;
	nrofEdges = 24; edgeCap = 23;

	//create queue for vertices
	for (int i = 0; i < MAX_NR_OF_VERTICES; i++) {
		vInfoArray[i].edgePtr = -(i + 1);
	}
	emptyV = -6;
	//create queue for Triangles
	for (int i = 0; i < MAX_NR_OF_TRIANGLES; i++) {
		triEPtr[i] = -(i + 1);
	}
	emptyT = -8;
	//create queue for Edges
	for (int i = 0; i < MAX_NR_OF_EDGES; i++) {
		e[i].nextEdge = -(i + 1);
	}
	emptyE = -24;

	// place vertecies
	// Y 0
	vertexArray[0].xyz[0] = 0.0f;	vertexArray[0].xyz[1] = MAX_LENGTH / 2.0f;	vertexArray[0].xyz[2] = 0.0f; /*normal*/	vertexArray[0].nxyz[0] = 0.0f;	vertexArray[0].nxyz[1] = 1;	vertexArray[0].nxyz[2] = 0.0f;
	vertexArray[1].xyz[0] = 0.0f;	vertexArray[1].xyz[1] = -MAX_LENGTH / 2.0f;	vertexArray[1].xyz[2] = 0.0f; /*normal*/	vertexArray[1].nxyz[0] = 0.0f;	vertexArray[1].nxyz[1] = -1;	vertexArray[1].nxyz[2] = 0.0f;

	// X 2
	vertexArray[2].xyz[0] = MAX_LENGTH / 2.0f;	vertexArray[2].xyz[1] = 0.0f;	vertexArray[2].xyz[2] = 0.0f; /*normal*/	vertexArray[2].nxyz[0] = 1;	vertexArray[2].nxyz[1] = 0.0f;	vertexArray[2].nxyz[2] = 0.0f;
	vertexArray[3].xyz[0] = -MAX_LENGTH / 2.0f;	vertexArray[3].xyz[1] = 0.0f;	vertexArray[3].xyz[2] = 0.0f; /*normal*/	vertexArray[3].nxyz[0] = -1;	vertexArray[3].nxyz[1] = 0.0f;	vertexArray[3].nxyz[2] = 0.0f;

	// Z 4
	vertexArray[4].xyz[0] = 0.0f;	vertexArray[4].xyz[1] = 0.0f;	vertexArray[4].xyz[2] = -MAX_LENGTH / 2.0f; /*normal*/	vertexArray[4].nxyz[0] = 0.0f;	vertexArray[4].nxyz[1] = 0.0f;	vertexArray[4].nxyz[2] = -1;
	vertexArray[5].xyz[0] = 0.0f;	vertexArray[5].xyz[1] = 0.0f;	vertexArray[5].xyz[2] = MAX_LENGTH / 2.0f; /*normal*/	vertexArray[5].nxyz[0] = 0.0f;	vertexArray[5].nxyz[1] = 0.0f;	vertexArray[5].nxyz[2] = 1;

	// bind triangles
	triangleArray[0].index[0] = 0;	triangleArray[0].index[1] = 3; triangleArray[0].index[2] = 5;
	triangleArray[1].index[0] = 0;	triangleArray[1].index[1] = 5; triangleArray[1].index[2] = 2;
	triangleArray[2].index[0] = 0;	triangleArray[2].index[1] = 2;	triangleArray[2].index[2] = 4;
	triangleArray[3].index[0] = 0;	triangleArray[3].index[1] = 4;	triangleArray[3].index[2] = 3;
	triangleArray[4].index[0] = 1;	triangleArray[4].index[1] = 5;	triangleArray[4].index[2] = 3;
	triangleArray[5].index[0] = 1;	triangleArray[5].index[1] = 2;	triangleArray[5].index[2] = 5;
	triangleArray[6].index[0] = 1;	triangleArray[6].index[1] = 4; triangleArray[6].index[2] = 2;
	triangleArray[7].index[0] = 1; triangleArray[7].index[1] = 3; triangleArray[7].index[2] = 4;

	// Bind halfEdges
	//TOP///////////////////////
	//first tri-----------------
	e[0].vertex = 0; e[0].triangle = 0;
	triEPtr[0] = 0;

	e[0].nextEdge = 1; e[1].vertex = 3; e[1].triangle = 0;
	e[1].nextEdge = 2; e[2].vertex = 5; e[2].triangle = 0;
	e[2].nextEdge = 0;

	vInfoArray[0].edgePtr = 2;

	//second tri--------------
	e[3].vertex = 0; e[3].triangle = 1;
	triEPtr[1] = 3;

	e[3].nextEdge = 4; e[4].vertex = 5; e[4].triangle = 1;
	e[4].nextEdge = 5; e[5].vertex = 2; e[5].triangle = 1;
	e[5].nextEdge = 3;

	vInfoArray[5].edgePtr = 3;

	//third tri----------------
	e[6].vertex = 0; e[6].triangle = 2;
	triEPtr[2] = 6;

	e[6].nextEdge = 7; e[7].vertex = 2; e[7].triangle = 2;
	e[7].nextEdge = 8; e[8].vertex = 4; e[8].triangle = 2;
	e[8].nextEdge = 6;

	vInfoArray[2].edgePtr = 6;

	//fourth tri-----------------
	e[9].vertex = 0; e[9].triangle = 3;
	triEPtr[3] = 9;

	e[9].nextEdge = 10; e[10].vertex = 4; e[10].triangle = 3;
	e[10].nextEdge = 11; e[11].vertex = 3; e[11].triangle = 3;
	e[11].nextEdge = 9;

	vInfoArray[4].edgePtr = 9;

	//BOTTOM///////////////////////////////////
	//fifth tri---------------------------
	e[12].vertex = 1; e[12].triangle = 4;
	triEPtr[4] = 12;

	e[12].nextEdge = 13; e[13].vertex = 5; e[13].triangle = 4;
	e[13].nextEdge = 14; e[14].vertex = 3; e[14].triangle = 4;
	e[14].nextEdge = 12;

	vInfoArray[1].edgePtr = 14;

	//sixth tri-----------------------
	e[15].vertex = 1; e[15].triangle = 5;
	triEPtr[5] = 15;

	e[15].nextEdge = 16; e[16].vertex = 2; e[16].triangle = 5;
	e[16].nextEdge = 17; e[17].vertex = 5; e[17].triangle = 5;
	e[17].nextEdge = 15;

	//seventh tri---------------------
	e[18].vertex = 1; e[18].triangle = 6;
	triEPtr[6] = 18;

	e[18].nextEdge = 19; e[19].vertex = 4; e[19].triangle = 6;
	e[19].nextEdge = 20; e[20].vertex = 2; e[20].triangle = 6;
	e[20].nextEdge = 18;

	//seventh tri
	e[21].vertex = 1; e[21].triangle = 7;
	triEPtr[7] = 21;

	e[21].nextEdge = 22; e[22].vertex = 3; e[22].triangle = 7;
	e[22].nextEdge = 23; e[23].vertex = 4; e[23].triangle = 7;
	e[23].nextEdge = 21;

	vInfoArray[3].edgePtr = 21;

	//TOP SIBLINGS
	e[triEPtr[0]].sibling = e[e[triEPtr[3]].nextEdge].nextEdge;
	e[e[e[triEPtr[3]].nextEdge].nextEdge].sibling = triEPtr[0];

	e[triEPtr[3]].sibling = e[e[triEPtr[2]].nextEdge].nextEdge;
	e[e[e[triEPtr[2]].nextEdge].nextEdge].sibling = triEPtr[3];

	e[triEPtr[2]].sibling = e[e[triEPtr[1]].nextEdge].nextEdge;
	e[e[e[triEPtr[1]].nextEdge].nextEdge].sibling = triEPtr[2];

	e[triEPtr[1]].sibling = e[e[triEPtr[0]].nextEdge].nextEdge;
	e[e[e[triEPtr[0]].nextEdge].nextEdge].sibling = triEPtr[1];

	//BOTTOM SIBLINGS
	e[triEPtr[4]].sibling = e[e[triEPtr[5]].nextEdge].nextEdge;
	e[e[e[triEPtr[5]].nextEdge].nextEdge].sibling = triEPtr[4];

	e[triEPtr[5]].sibling = e[e[triEPtr[6]].nextEdge].nextEdge;
	e[e[e[triEPtr[6]].nextEdge].nextEdge].sibling = triEPtr[5];

	e[triEPtr[6]].sibling = e[e[triEPtr[7]].nextEdge].nextEdge;
	e[e[e[triEPtr[7]].nextEdge].nextEdge].sibling = triEPtr[6];

	e[triEPtr[7]].sibling = e[e[triEPtr[4]].nextEdge].nextEdge;
	e[e[e[triEPtr[4]].nextEdge].nextEdge].sibling = triEPtr[7];

	//MIDDLE SIBLINGS
	e[e[triEPtr[0]].nextEdge].sibling = e[triEPtr[4]].nextEdge;
	e[e[triEPtr[4]].nextEdge].sibling = e[triEPtr[0]].nextEdge;

	e[e[triEPtr[1]].nextEdge].sibling = e[triEPtr[5]].nextEdge;
	e[e[triEPtr[5]].nextEdge].sibling = e[triEPtr[1]].nextEdge;

	e[e[triEPtr[2]].nextEdge].sibling = e[triEPtr[6]].nextEdge;
	e[e[triEPtr[6]].nextEdge].sibling = e[triEPtr[2]].nextEdge;

	e[e[triEPtr[3]].nextEdge].sibling = e[triEPtr[7]].nextEdge;
	e[e[triEPtr[7]].nextEdge].sibling = e[triEPtr[3]].nextEdge;

	while (stepRad < rad) {
		for (int j = 0; j < vertexCap + 1; j++) {
			//tempP1[0] = vertexArray[j].x; tempP1[1] = vertexArray[j].y; tempP1[2] = vertexArray[j].z;
			if (vInfoArray[j].edgePtr > 0) {
				linAlg::normVec(vertexArray[j].xyz);

				vertexArray[j].xyz[0] = vertexArray[j].xyz[0] * stepRad;
				vertexArray[j].xyz[1] = vertexArray[j].xyz[1] * stepRad;
				vertexArray[j].xyz[2] = vertexArray[j].xyz[2] * stepRad;

				//tempSV.index = j;
				changedVertices.push_back(j);
				vInfoArray[j].selected = 3.0f;
			}
		}
		updateArea(&changedVertices[0], changedVertices.size());
		changedVertices.clear();

		stepRad += MIN_LENGTH;
	}
	stepRad = rad;
	for (int i = 0; i < 60; i++) {
		for (int j = 0; j < vertexCap + 1; j++) {
			//tempP1[0] = vertexArray[j].x; tempP1[1] = vertexArray[j].y; tempP1[2] = vertexArray[j].z;
			if (vInfoArray[j].edgePtr > 0) {
				linAlg::normVec(vertexArray[j].xyz);

				vertexArray[j].xyz[0] = vertexArray[j].xyz[0] * stepRad;
				vertexArray[j].xyz[1] = vertexArray[j].xyz[1] * stepRad;
				vertexArray[j].xyz[2] = vertexArray[j].xyz[2] * stepRad;

				//tempSV.index = j;
				changedVertices.push_back(j);
				vInfoArray[j].selected = 3.0f;
			}
		}
		updateArea(&changedVertices[0], changedVertices.size());
		changedVertices.clear();
	}

	for (int i = 0; i < vertexCap; i++) {
		vInfoArray[i].selected = 0.0f;
	}

}

void DynamicMesh::load(std::string fileName) {
	
	int prevVertexCap = vertexCap;
	int prevTriangleCap = triangleCap;

	delete vertexArray;
	delete vInfoArray;

	delete triangleArray;
	delete triEPtr;

	delete e;

	vertexArray = new vertex[MAX_NR_OF_VERTICES];
	vInfoArray = new vInfo[MAX_NR_OF_VERTICES];

	triangleArray = new triangle[MAX_NR_OF_TRIANGLES];
	triEPtr = new int[MAX_NR_OF_TRIANGLES];

	e = new halfEdge[MAX_NR_OF_EDGES];

	// read mesh from file
	std::cout << "loading mesh..." << std::endl;

	std::ifstream file("../savedFiles/" + fileName, std::ios::in | std::ios::binary);
	if (file.is_open()) {

		int bitCount = 0;

		// read the size of the arrays
		file.seekg(0);
		file.read((char*)&vertexCap, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&triangleCap, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&edgeCap, sizeof(int));
		bitCount += sizeof(int);

		// read mesh position
		file.seekg(bitCount);
		file.read((char*)&position[0], sizeof(float) * 3);
		bitCount += sizeof(float) * 3;

		// read mesh orientation
		file.seekg(bitCount);
		file.read((char*)&orientation[0], sizeof(float) * 16);
		bitCount += sizeof(float) * 16;

		// read vertecies
		file.seekg(bitCount);
		file.read((char*)&nrofVerts, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&vertexArray[0], sizeof(vertex) * (vertexCap + 1));
		bitCount += sizeof(vertex) * (vertexCap + 1);

		// read triangles
		file.seekg(bitCount);
		file.read((char*)&nrofTris, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&triangleArray[0], sizeof(triangle) * (triangleCap + 1));
		bitCount += sizeof(triangle) * (triangleCap + 1);

		// read empty index
		file.seekg(bitCount);
		file.read((char*)&emptyV, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&emptyT, sizeof(int));
		bitCount += sizeof(int);

		file.seekg(bitCount);
		file.read((char*)&emptyE, sizeof(int));
		bitCount += sizeof(int);

		// read halfEdges
		file.seekg(bitCount);
		file.read((char*)&e[0], sizeof(halfEdge) * (edgeCap + 1));
		bitCount += sizeof(halfEdge) * (edgeCap + 1);

		// read vInfoArray
		file.seekg(bitCount);
		file.read((char*)&vInfoArray[0], sizeof(vInfo) * (vertexCap + 1));
		bitCount += sizeof(vInfo) * (vertexCap + 1);

		// read triEdgePtr
		file.seekg(bitCount);
		file.read((char*)&triEPtr[0], sizeof(int) * (triangleCap + 1));
		bitCount += sizeof(int) * (triangleCap + 1);

		// read emptyV
		file.seekg(bitCount);
		file.read((char*)&emptyV, sizeof(int));
		bitCount += sizeof(int);

		// read emptyT
		file.seekg(bitCount);
		file.read((char*)&emptyT, sizeof(int));

		file.close();

		std::cout << "load complete" << std::endl;
	}
	else {
		std::cout << "could not open file" << std::endl;
	}

	
	if (vertexCap < prevVertexCap)
		vertexRange = prevVertexCap;
	else
		vertexRange = 0;
	if (triangleCap < prevTriangleCap)
		triangleRange = prevTriangleCap;
	else
		triangleRange = 0;

	//create queue for vertices
	for (int i = vertexCap + 1; i < MAX_NR_OF_VERTICES; i++) {
		vInfoArray[i].edgePtr = -(i + 1);
	}
	//create queue for Triangles
	for (int i = triangleCap + 1; i < MAX_NR_OF_TRIANGLES; i++) {
		triEPtr[i] = -(i + 1);
	}
	//create queue for Edges
	for (int i = edgeCap + 1; i < MAX_NR_OF_EDGES; i++) {
		e[i].nextEdge = -(i + 1);
	}
}

void DynamicMesh::save() {

	std::cout << "saving..." << std::endl;

	// get date and time
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);

	int bitCount = 0;

	strftime(buf, sizeof(buf), "%Y-%m-%d_%X", &tstruct);

	// replace : with - to be able to save file
	buf[13] = '-';
	buf[16] = '-';

	std::string fileName(buf);


	std::fstream file;
	file.open("../savedFiles/" + fileName + ".bin", std::ios::out | std::ios::binary);

	if (file.is_open()) {

		// write the size of the different arrays to the file
		file.seekp(0);
		file.write((char*)&vertexCap, sizeof(int));
		bitCount += sizeof(int);

		file.seekp(bitCount);
		file.write((char*)&triangleCap, sizeof(int));
		bitCount += sizeof(int);

		file.seekp(bitCount);
		file.write((char*)&edgeCap, sizeof(int));
		bitCount += sizeof(int);

		// write position and orientation
		file.seekp(bitCount);
		file.write((char*)&position[0], sizeof(float) * 3);
		bitCount += sizeof(float) * 3;

		file.seekp(bitCount);
		file.write((char*)&orientation[0], sizeof(float) * 16);
		bitCount += sizeof(float) * 16;

		// write verticies
		file.seekp(bitCount);
		file.write((char*)&nrofVerts, sizeof(int));
		bitCount += sizeof(int);

		file.seekp(bitCount);
		file.write((char*)&vertexArray[0], sizeof(vertex) * (vertexCap + 1));
		bitCount += sizeof(vertex) * (vertexCap + 1);

		// write triangles
		file.seekp(bitCount);
		file.write((char*)&nrofTris, sizeof(int));
		bitCount += sizeof(int);

		file.seekp(bitCount);
		file.write((char*)&triangleArray[0], sizeof(triangle) * (triangleCap + 1));
		bitCount += sizeof(triangle) * (triangleCap + 1);

		// write empty index
		file.seekp(bitCount);
		file.write((char*)&emptyV, sizeof(int));
		bitCount += sizeof(int);

		file.seekp(bitCount);
		file.write((char*)&emptyT, sizeof(int));
		bitCount += sizeof(int);

		file.seekp(bitCount);
		file.write((char*)&emptyE, sizeof(int));
		bitCount += sizeof(int);

		// write edges
		file.seekp(bitCount);
		file.write((char*)&e[0], sizeof(halfEdge) * (edgeCap + 1));
		bitCount += sizeof(halfEdge) * (edgeCap + 1);

		// write vertex edge pointers
		file.seekp(bitCount);
		file.write((char*)&vInfoArray[0], sizeof(vInfo) * (vertexCap + 1));
		bitCount += sizeof(vInfo) * (vertexCap + 1);

		// write triangle edge pointers
		file.seekp(bitCount);
		file.write((char*)&triEPtr[0], sizeof(int) * (triangleCap + 1));
		bitCount += sizeof(int) * (triangleCap + 1);

		// write emptyV
		file.seekp(bitCount);
		file.write((char*)&emptyV, sizeof(int));
		bitCount += sizeof(int);

		// write emptyT
		file.seekp(bitCount);
		file.write((char*)&emptyT, sizeof(int));

		file.close();

		std::cout << "save complete" << std::endl;

	}
	else {
		std::cout << "could not open file for saving" << std::endl;
	}
}

void DynamicMesh::updateArea(int* changeList, int listSize) {

	static float* vPoint1; static float* vPoint2; static float* vPoint3; static float* vPoint4;
	static float tempVec1[3], tempVec2[3], tempVec3[3];
	static float tempNorm1[3] = { 0.0f, 0.0f, 0.0f };
	static float tempNorm2[3] = { 0.0f, 0.0f, 0.0f };
	int tempEdge; int tempE;
	static int vert1, vert2, vert3; 
	static float edgeLength, edgeLength2;

	//sIt = sHead->next;
	for (int i = 0; i < listSize; i++)
	{
		vert3 = changeList[i];
		vInfoArray[vert3].selected = 4.0f;
		
		if (vInfoArray[vert3].edgePtr <= 0)
			continue;
			
		tempEdge = e[e[vInfoArray[vert3].edgePtr].nextEdge].sibling;
		vPoint1 = vertexArray[vert3].xyz;

		do {
			if (vInfoArray[e[tempEdge].vertex].selected < 4.0f)
			{
				tempE = tempEdge;
				tempEdge = e[e[tempEdge].nextEdge].sibling;

				vert2 = e[tempE].vertex;
				vPoint2 = vertexArray[vert2].xyz;

				linAlg::calculateVec(vPoint2, vPoint1, tempVec1);

				edgeLength = linAlg::vecLength(tempVec1);

				//check if edge is too long
				if (edgeLength > MAX_LENGTH) {
					//check if edge should be flipped
					vert1 = e[e[e[tempE].nextEdge].nextEdge].vertex;
					vert2 = e[e[e[e[tempE].sibling].nextEdge].nextEdge].vertex;

					vPoint3 = vertexArray[vert1].xyz;

					vPoint4 = vertexArray[vert2].xyz;
					linAlg::calculateVec(vPoint3, vPoint4, tempNorm1);
					edgeLength2 = linAlg::vecLength(tempNorm1);

					if (edgeLength2 < MAX_LENGTH)
					{
						edgeFlip(tempE);
						if (edgeLength2 < MIN_LENGTH)
						{
							edgeCollapse(false, tempE);
						}
					}
					//---------------------
					//should not be flipped, should be split
					else
					{
						edgeSplit(vPoint1, tempVec1, tempE);
					}
					//----------------------
				}
				//----------------------
				// check if edge is to short
				else if (edgeLength < MIN_LENGTH) {
					edgeCollapse(false, tempE);
				}
			}
			else
				tempEdge = e[e[tempEdge].nextEdge].sibling;

		} while (tempEdge != vInfoArray[vert3].edgePtr);

		//now do it for vertexEPtr aswell---------------------------
		tempE = tempEdge;

		vert2 = e[tempE].vertex;
		vPoint2 = vertexArray[vert2].xyz;

		linAlg::calculateVec(vPoint2, vPoint1, tempVec1);

		edgeLength = linAlg::vecLength(tempVec1);

		//check if edge is too long
		if (edgeLength > MAX_LENGTH) {
			//check if edge should be flipped
			vert1 = e[e[e[tempE].nextEdge].nextEdge].vertex;
			vert2 = e[e[e[e[tempE].sibling].nextEdge].nextEdge].vertex;

			vPoint3 = vertexArray[vert1].xyz;

			vPoint4 = vertexArray[vert2].xyz;
			linAlg::calculateVec(vPoint3, vPoint4, tempNorm1);
			edgeLength2 = linAlg::vecLength(tempNorm1);

			if (edgeLength2 < MAX_LENGTH)
			{
				edgeFlip(tempE);
				if (edgeLength2 < MIN_LENGTH)
				{
					edgeCollapse(false, tempE);
				}
			}
			//---------------------
			//should not be flipped, should be split
			else
			{
				edgeSplit(vPoint1, tempVec1, tempE);
			}
			//----------------------
		}
		//----------------------
		// check if edge is too short
		else if (edgeLength < MIN_LENGTH) {
			edgeCollapse(false, tempE);
		}
	//>-----------------------
       
		tempEdge = vInfoArray[vert3].edgePtr;
		// Update normal /////////////////////////////////////////////////////////////////////////////
		do {
			vert1 = e[tempEdge].vertex;
			vert2 = e[e[e[tempEdge].nextEdge].nextEdge].vertex;

			vPoint2 = vertexArray[vert1].xyz;
			vPoint3 = vertexArray[vert2].xyz;

			linAlg::calculateVec(vPoint2, vPoint1, tempVec1);
			linAlg::calculateVec(vPoint3, vPoint1, tempVec2 );

			linAlg::crossProd(tempNorm1, tempVec2, tempVec1);

			linAlg::normVec(tempNorm1);

			tempNorm2[0] += tempNorm1[0];
			tempNorm2[1] += tempNorm1[1];
			tempNorm2[2] += tempNorm1[2];


			tempEdge = e[e[tempEdge].nextEdge].sibling;
		} while (tempEdge != vInfoArray[vert3].edgePtr);

		//edgeLength = linAlg::vecLength(tempNorm2);

		//tempNorm2[0] = tempNorm2[0] / edgeLength;
		//tempNorm2[1] = tempNorm2[1] / edgeLength;
		//tempNorm2[2] = tempNorm2[2] / edgeLength;

		linAlg::normVec(tempNorm2);

        vertexArray[vert3].nxyz[0] = tempNorm2[0];
		vertexArray[vert3].nxyz[1] = tempNorm2[1];
		vertexArray[vert3].nxyz[2] = tempNorm2[2];
		
		
		//sIt = sIt->next;
	}
}
void DynamicMesh::updateNormals(int* changeList, int listSize) {

	static float* vPoint1; static float* vPoint2; static float* vPoint3; static float* vPoint4;
	static float tempVec1[3], tempVec2[3], tempVec3[3];
	static float tempNorm1[3] = { 0.0f, 0.0f, 0.0f };
	static float tempNorm2[3] = { 0.0f, 0.0f, 0.0f };
	int tempEdge; int tempE;
	static int vert1, vert2, vert3;
	static float edgeLength, edgeLength2;

	//sIt = sHead->next;
	for (int i = 0; i < listSize; i++)
	{
		vert3 = changeList[i];
		vInfoArray[vert3].selected = vInfoArray[vert3].selected - floor(vInfoArray[vert3].selected);

		//>-----------------------
		vPoint1 = vertexArray[vert3].xyz;
		tempEdge = vInfoArray[vert3].edgePtr;
		// Update normal /////////////////////////////////////////////////////////////////////////////
		do {
			vert1 = e[tempEdge].vertex;
			vert2 = e[e[e[tempEdge].nextEdge].nextEdge].vertex;

			vPoint2 = vertexArray[vert1].xyz;
			vPoint3 = vertexArray[vert2].xyz;

			linAlg::calculateVec(vPoint2, vPoint1, tempVec1);
			linAlg::calculateVec(vPoint3, vPoint1, tempVec2);

			linAlg::crossProd(tempNorm1, tempVec2, tempVec1);

			linAlg::normVec(tempNorm1);

			tempNorm2[0] += tempNorm1[0];
			tempNorm2[1] += tempNorm1[1];
			tempNorm2[2] += tempNorm1[2];


			tempEdge = e[e[tempEdge].nextEdge].sibling;
		} while (tempEdge != vInfoArray[vert3].edgePtr);

		edgeLength = linAlg::vecLength(tempNorm2);

		tempNorm2[0] = tempNorm2[0] / edgeLength;
		tempNorm2[1] = tempNorm2[1] / edgeLength;
		tempNorm2[2] = tempNorm2[2] / edgeLength;

		//linAlg::normVec(tempNorm2);

		vertexArray[vert3].nxyz[0] = tempNorm2[0];
		vertexArray[vert3].nxyz[1] = tempNorm2[1];
		vertexArray[vert3].nxyz[2] = tempNorm2[2];


		//sIt = sIt->next;
	}
}

void DynamicMesh::edgeSplit(float* vPoint, float* vec, int &edge) {

	int tempV;
	int tempT1; int tempT2;
	int tempE; int tempE2;
	static int vert1, vert2, vert3, vert4;
	//static float temp[3], temp2[3], temp3[3];
	static GLfloat* vPoint1; static GLfloat* vPoint2; static GLfloat* vPoint3;
	static float tempNorm1[3], tempNorm2[3];
	static float tempVec1[3], tempVec2[3];

	// create new vertex point
	vert1 = e[edge].vertex;
	vert2 = e[e[edge].sibling].vertex;
	//vert3 = edge->nextEdge->nextEdge->vertex;
	//vert4 = edge->sibling->nextEdge->nextEdge->vertex;

   	tempV = -emptyV;
	vertexCap = max(vertexCap, tempV);

	emptyV = vInfoArray[tempV].edgePtr;
	vertexArray[tempV].xyz[0] = (vertexArray[vert1].xyz[0] + vertexArray[vert2].xyz[0]) / 2.0f;
	vertexArray[tempV].xyz[1] = (vertexArray[vert1].xyz[1] + vertexArray[vert2].xyz[1]) / 2.0f;
	vertexArray[tempV].xyz[2] = (vertexArray[vert1].xyz[2] + vertexArray[vert2].xyz[2]) / 2.0f;
	//TODO: TEST
	//vInfoArray[tempV].selected = (vInfoArray[vert1].selected + vInfoArray[vert2].selected) / 2.0f;

	//copy triangles
	tempT1 = -emptyT;
	emptyT = triEPtr[tempT1];
	triangleArray[tempT1].index[0] = triangleArray[e[edge].triangle].index[0];
	triangleArray[tempT1].index[1] = triangleArray[e[edge].triangle].index[1];
	triangleArray[tempT1].index[2] = triangleArray[e[edge].triangle].index[2];

    tempT2 = -emptyT;
	triangleCap = max(triangleCap, tempT2);
	emptyT = triEPtr[tempT2];
	triangleArray[tempT2].index[0] = triangleArray[e[e[edge].sibling].triangle].index[0];
	triangleArray[tempT2].index[1] = triangleArray[e[e[edge].sibling].triangle].index[1];
	triangleArray[tempT2].index[2] = triangleArray[e[e[edge].sibling].triangle].index[2];

	// rebind old triangles
	for (int i = 0; i < 3; i++) {
		if (triangleArray[e[edge].triangle].index[i] == e[edge].vertex)
			triangleArray[e[edge].triangle].index[i] = tempV;

		if (triangleArray[e[e[edge].sibling].triangle].index[i] == e[edge].vertex)
			triangleArray[e[e[edge].sibling].triangle].index[i] = tempV;
	}

	// rebind new triangles
	for (int i = 0; i < 3; i++) {
		if (triangleArray[tempT1].index[i] == e[e[edge].sibling].vertex)
			triangleArray[tempT1].index[i] = tempV;

		if (triangleArray[tempT2].index[i] == e[e[edge].sibling].vertex)
			triangleArray[tempT2].index[i] = tempV;
	}
	
	/////////////////////////////////////////////////////////
	/*nextEdge*/tempE2 = tempE = -emptyE;
	emptyE = e[tempE].nextEdge;

	triEPtr[tempT1] = tempE;
	vInfoArray[tempV].edgePtr = tempE;

	e[tempE].vertex = e[edge].vertex;
	e[tempE].triangle = tempT1;
	/*nextEdge*/tempE = e[tempE].nextEdge = -emptyE;
	emptyE = e[tempE].nextEdge;

	e[tempE].vertex = tempV;
	e[tempE].triangle = tempT1;
	e[tempE].sibling = e[e[edge].nextEdge].nextEdge;
	/*nextEdge*/tempE = e[tempE].nextEdge = -emptyE;
	emptyE = e[tempE].nextEdge;

	e[tempE].vertex = e[e[e[edge].nextEdge].nextEdge].vertex;
	e[tempE].triangle = tempT1;
	e[tempE].nextEdge = tempE2;
	e[tempE].sibling = e[e[e[edge].nextEdge].nextEdge].sibling;
	//rebind sibling of existing triangle
	e[e[e[e[edge].nextEdge].nextEdge].sibling].sibling = e[e[tempE2].nextEdge].nextEdge;
	//rebind sibling of old triangle
	e[e[e[edge].nextEdge].nextEdge].sibling = e[tempE2].nextEdge;

	/*nextEdge*/tempE2 = tempE = -emptyE;
	emptyE = e[tempE].nextEdge;

	triEPtr[tempT2] = tempE;

	e[tempE].sibling = triEPtr[tempT1];
	//bind sibling of first newEdge
	e[triEPtr[tempT1]].sibling = tempE;
	//continue
	e[tempE].vertex = tempV;
	e[tempE].triangle = tempT2;
	/*nextedge*/tempE = e[tempE].nextEdge = -emptyE;
	emptyE = e[tempE].nextEdge;

	e[tempE].vertex = e[edge].vertex;
	e[tempE].triangle = tempT2;
	e[tempE].sibling = e[e[e[edge].sibling].nextEdge].sibling;
	//rebind sibling of existing triangle
	e[e[e[e[edge].sibling].nextEdge].sibling].sibling = e[tempE2].nextEdge;
	/*nextedge*/tempE = e[tempE].nextEdge = -emptyE;
	emptyE = e[tempE].nextEdge;

	e[tempE].vertex = e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex;
	e[tempE].triangle = tempT2;
	e[tempE].sibling = e[e[edge].sibling].nextEdge;
	//rebind sibling of old triangle
	e[e[e[edge].sibling].nextEdge].sibling = tempE;
	e[tempE].nextEdge = tempE2;
	edgeCap = max(edgeCap, tempE);

	// rebind old edge vertex
	vInfoArray[e[edge].vertex].edgePtr = tempE2;

	e[edge].vertex = tempV;
	e[e[e[edge].sibling].nextEdge].vertex = tempV;

	// Update normal /////////////////////////////////////////////////////////////////////////////
	tempE = vInfoArray[tempV].edgePtr;
	vPoint1 = vertexArray[e[e[tempE].sibling].vertex].xyz;
	// loop through the edges
	do {
		vert1 = e[tempE].vertex;
		vert2 = e[e[e[tempE].nextEdge].nextEdge].vertex;

		vPoint2 = vertexArray[vert1].xyz;
		vPoint3 = vertexArray[vert2].xyz;
		
		linAlg::calculateVec(vPoint2, vPoint1, tempVec1);
		linAlg::calculateVec(vPoint3, vPoint1, tempVec2);

		linAlg::crossProd(tempNorm1, tempVec2, tempVec1);

		linAlg::normVec(tempNorm1);

		tempNorm2[0] += tempNorm1[0];
		tempNorm2[1] += tempNorm1[1];
		tempNorm2[2] += tempNorm1[2];

		tempE = e[e[tempE].nextEdge].sibling;
	} while (tempE != vInfoArray[tempV].edgePtr);

	//static float vecLenght = linAlg::vecLength(tempNorm2);

	//tempNorm2[0] = tempNorm2[0] / vecLenght;
	//tempNorm2[1] = tempNorm2[1] / vecLenght;
	//tempNorm2[2] = tempNorm2[2] / vecLenght;
	linAlg::normVec(tempNorm2);

	//linAlg::normVec(tempNorm2);
	vertexArray[tempV].nxyz[0] = tempNorm2[0];
	vertexArray[tempV].nxyz[1] = tempNorm2[1];
	vertexArray[tempV].nxyz[2] = tempNorm2[2];
	nrofVerts++;
	nrofTris = nrofTris + 2;
	
}

void DynamicMesh::edgeFlip(int &edge)
{
	int tempE; int tempE2;

	int vert1 = e[e[e[edge].nextEdge].nextEdge].vertex;
	int vert2 = e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex;

	tempE2 = e[edge].nextEdge;
	tempE = e[e[tempE2].nextEdge].sibling;
	while (tempE != tempE2)
	{
		if (e[tempE].vertex == vert2){
			//test----------------
			/*vert1 = e[edge].vertex;
			vert2 = e[e[edge].sibling].vertex;
			float tempVec[3] = { vertexArray[vert1].x - vertexArray[vert2].x, vertexArray[vert1].y - vertexArray[vert2].y, vertexArray[vert1].z - vertexArray[vert2].z };
			linAlg::normVec(tempVec);
			vertexArray[vert1].x -= tempVec[0] * (MIN_LENGTH / 2.0f); vertexArray[vert1].y -= tempVec[1] * (MIN_LENGTH / 2.0f); vertexArray[vert1].z -= tempVec[2] * (MIN_LENGTH / 2.0f);
			vertexArray[vert2].x += tempVec[0] * (MIN_LENGTH / 2.0f); vertexArray[vert2].y += tempVec[1] * (MIN_LENGTH / 2.0f); vertexArray[vert2].z += tempVec[2] * (MIN_LENGTH / 2.0f);*/
			//------------------
			return;
		}
		tempE = e[e[tempE].nextEdge].sibling;
	}
	//--------------------
	//perform flip
	for (int i = 0; i < 3; i++) {
		if (triangleArray[e[edge].triangle].index[i] == e[edge].vertex)
			triangleArray[e[edge].triangle].index[i] = vert2;

		if (triangleArray[e[e[edge].sibling].triangle].index[i] == e[e[edge].sibling].vertex)
			triangleArray[e[e[edge].sibling].triangle].index[i] = vert1;
	}
	// rebind triangles
	e[e[e[e[edge].sibling].nextEdge].nextEdge].triangle = e[edge].triangle;
	e[e[e[edge].nextEdge].nextEdge].triangle = e[e[edge].sibling].triangle;
	////
	e[e[e[e[edge].sibling].nextEdge].nextEdge].nextEdge = e[edge].nextEdge;
	e[e[e[edge].nextEdge].nextEdge].nextEdge = e[e[edge].sibling].nextEdge;
	////

	tempE = e[e[edge].nextEdge].nextEdge;
	e[e[edge].nextEdge].nextEdge = edge;

	tempE2 = e[e[e[edge].sibling].nextEdge].nextEdge;
	e[e[e[edge].sibling].nextEdge].nextEdge = e[edge].sibling;

	e[e[edge].sibling].nextEdge = tempE;
	e[edge].nextEdge = tempE2;

	e[edge].vertex = e[e[e[edge].sibling].nextEdge].vertex;
	e[e[edge].sibling].vertex = e[e[edge].nextEdge].vertex;

	vInfoArray[e[e[e[edge].nextEdge].nextEdge].vertex].edgePtr = e[edge].nextEdge;
	vInfoArray[e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex].edgePtr = e[e[edge].sibling].nextEdge;
	//std::cout << "flip" << std::endl;
	//pass on
	//edge = e[edge].nextEdge;
}

void DynamicMesh::edgeCollapse(bool recursive, int &edge) {

	int tempE;
	int tempE2;
	static int currVert; 
	static int nVert;
	static int ndVert;
	int ctr = 0;

	currVert = e[e[edge].sibling].vertex;
   	nVert = e[edge].vertex;
	
	//check for special cases
	tempE = e[e[edge].nextEdge].sibling;
	tempE2 = e[e[e[edge].sibling].nextEdge].sibling;
	do{
		do{
			if (e[tempE2].vertex == e[tempE].vertex)
				ctr++;

			tempE2 = e[e[tempE2].nextEdge].sibling;
		} while (tempE2 != e[edge].sibling);

		tempE2 = e[e[e[edge].sibling].nextEdge].sibling;
		tempE = e[e[tempE].nextEdge].sibling;
	} while (tempE != edge);

	if (ctr != 2)
	{
		//test-----------------
		//float tempVec[3] = { vertexArray[nVert].x - vertexArray[currVert].x, vertexArray[nVert].y - vertexArray[currVert].y, vertexArray[nVert].z - vertexArray[currVert].z };
		//linAlg::normVec(tempVec);
		//vertexArray[nVert].x += tempVec[0] * (MIN_LENGTH / 2.0f); vertexArray[nVert].y += tempVec[0] * (MIN_LENGTH / 2.0f); vertexArray[nVert].z += tempVec[0] * (MIN_LENGTH / 2.0f);
		//vertexArray[currVert].x -= tempVec[0] * (MIN_LENGTH / 2.0f); vertexArray[currVert].y -= tempVec[0] * (MIN_LENGTH / 2.0f); vertexArray[currVert].z -= tempVec[0] * (MIN_LENGTH / 2.0f);
		//---------------------
		return;
	}
		
	// rebind edges that point to nVert
	tempE = e[e[e[edge].nextEdge].nextEdge].sibling;
	while (tempE != e[e[edge].sibling].nextEdge)
	{
		e[tempE].vertex = currVert;

		// rebind the triangles containing nVert as index
		for (int i = 0; i < 3; i++) {
			if (triangleArray[e[tempE].triangle].index[i] == nVert) {
				triangleArray[e[tempE].triangle].index[i] = currVert;
				break;
			}
		}
		tempE = e[e[e[tempE].nextEdge].nextEdge].sibling;
	}

	tempE = e[e[edge].nextEdge].sibling;
	

	// rebind edges
	e[e[e[edge].nextEdge].sibling].sibling = e[e[e[edge].nextEdge].nextEdge].sibling;
	e[e[e[e[edge].nextEdge].nextEdge].sibling].sibling = e[e[edge].nextEdge].sibling;

	e[e[e[e[edge].sibling].nextEdge].sibling].sibling = e[e[e[e[edge].sibling].nextEdge].nextEdge].sibling;
	e[e[e[e[e[edge].sibling].nextEdge].nextEdge].sibling].sibling = e[e[e[edge].sibling].nextEdge].sibling;

	// rebind vertex pointer
	vInfoArray[currVert].edgePtr = e[e[edge].nextEdge].sibling;
	vInfoArray[e[e[e[edge].nextEdge].nextEdge].vertex].edgePtr = e[e[e[edge].nextEdge].nextEdge].sibling;
	vInfoArray[e[e[e[e[edge].sibling].nextEdge].nextEdge].vertex].edgePtr = e[e[e[e[edge].sibling].nextEdge].nextEdge].sibling;

	//reset the removed triangles
	tempE2 = e[edge].triangle;
	
	triangleArray[tempE2].index[0] = 0;
	triangleArray[tempE2].index[1] = 0;
	triangleArray[tempE2].index[2] = 0;
	tempE2 = e[e[edge].sibling].triangle;
	triangleArray[tempE2].index[0] = 0;
	triangleArray[tempE2].index[1] = 0;
	triangleArray[tempE2].index[2] = 0;
	nrofTris = nrofTris - 2;
	                                                                                                                
	// reset the removed vertex
	vertexArray[nVert].xyz[0] = 100.0f;
	vertexArray[nVert].xyz[1] = 100.0f;
	vertexArray[nVert].xyz[2] = 100.0f;
	vertexArray[nVert].nxyz[0] = 0;
	vertexArray[nVert].nxyz[1] = 0;
	vertexArray[nVert].nxyz[2] = 0;
	vInfoArray[nVert].selected = 0;
	nrofVerts--;

	// reset edge pointers
	vInfoArray[nVert].edgePtr = emptyV;
	emptyV = -nVert;
	//
	triEPtr[e[edge].triangle] = emptyT;
	triEPtr[e[e[edge].sibling].triangle] = -e[edge].triangle;
	emptyT = -e[e[edge].sibling].triangle;

	// delete the removed edges
	e[e[e[e[edge].sibling].nextEdge].nextEdge].nextEdge = emptyE;
	e[e[e[edge].sibling].nextEdge].nextEdge = -e[e[e[edge].sibling].nextEdge].nextEdge;
	e[e[edge].sibling].nextEdge = -e[e[edge].sibling].nextEdge;
	e[e[e[edge].nextEdge].nextEdge].nextEdge = -e[edge].sibling;
	e[e[edge].nextEdge].nextEdge = -e[e[edge].nextEdge].nextEdge;
	e[edge].nextEdge = -e[edge].nextEdge;

	emptyE = -edge;
}
