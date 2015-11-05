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

	position[0] = 0.0f; position[1] = -0.3f; position[2] = 0.0f;

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

	nrofVerts = 6; vertexCap = 6;
	nrofTris = 8; triangleCap = 8;
	nrofEdges = 24; edgeCap = 24;

	//create queue for vertices
	for (int i = 0; i < MAX_NR_OF_VERTICES; i++) {
		vInfoArray[i].edgePtr = -(i + 1);
	}
	emptyV = -7;
	//create queue for Triangles
	for (int i = 0; i < MAX_NR_OF_TRIANGLES; i++) {
		triEPtr[i] = -(i + 1);
	}
	emptyT = -9;
	//create queue for Edges
	for (int i = 0; i < MAX_NR_OF_EDGES; i++) {
		e[i].nextEdge = -(i + 1);
	}
	emptyE = -25;

	// índex 0 is not used
	vertexArray[0].xyz[0] = -1000.0f;	vertexArray[0].xyz[1] = -1000.0f;	vertexArray[0].xyz[2] = -1000.0f; /*normal*/	vertexArray[0].nxyz[0] = -1000.0f;	vertexArray[0].nxyz[1] = -1000;	vertexArray[0].nxyz[2] = -1000.0f;

	// place vertecies
	// Y 0
	vertexArray[1].xyz[0] = 0.0f;	vertexArray[1].xyz[1] = MAX_LENGTH / 2.0f;	vertexArray[1].xyz[2] = 0.0f; /*normal*/	vertexArray[1].nxyz[0] = 0.0f;	vertexArray[1].nxyz[1] = 1;		vertexArray[1].nxyz[2] = 0.0f;
	vertexArray[2].xyz[0] = 0.0f;	vertexArray[2].xyz[1] = -MAX_LENGTH / 2.0f;	vertexArray[2].xyz[2] = 0.0f; /*normal*/	vertexArray[2].nxyz[0] = 0.0f;	vertexArray[2].nxyz[1] = -1;	vertexArray[2].nxyz[2] = 0.0f;

	// X 2
	vertexArray[3].xyz[0] = MAX_LENGTH / 2.0f;	vertexArray[3].xyz[1] = 0.0f;	vertexArray[3].xyz[2] = 0.0f; /*normal*/	vertexArray[3].nxyz[0] = 1;		vertexArray[3].nxyz[1] = 0.0f;	vertexArray[3].nxyz[2] = 0.0f;
	vertexArray[4].xyz[0] = -MAX_LENGTH / 2.0f;	vertexArray[4].xyz[1] = 0.0f;	vertexArray[4].xyz[2] = 0.0f; /*normal*/	vertexArray[4].nxyz[0] = -1;	vertexArray[4].nxyz[1] = 0.0f;	vertexArray[4].nxyz[2] = 0.0f;

	// Z 4
	vertexArray[5].xyz[0] = 0.0f;	vertexArray[5].xyz[1] = 0.0f;	vertexArray[5].xyz[2] = -MAX_LENGTH / 2.0f; /*normal*/	vertexArray[5].nxyz[0] = 0.0f;	vertexArray[5].nxyz[1] = 0.0f;	vertexArray[5].nxyz[2] = -1;
	vertexArray[6].xyz[0] = 0.0f;	vertexArray[6].xyz[1] = 0.0f;	vertexArray[6].xyz[2] = MAX_LENGTH / 2.0f; /*normal*/	vertexArray[6].nxyz[0] = 0.0f;	vertexArray[6].nxyz[1] = 0.0f;	vertexArray[6].nxyz[2] = 1;

	// bind triangles
	triangleArray[1].index[0] = 1;		triangleArray[1].index[1] = 4;		triangleArray[1].index[2] = 6;
	triangleArray[2].index[0] = 1;		triangleArray[2].index[1] = 6;		triangleArray[2].index[2] = 3;
	triangleArray[3].index[0] = 1;		triangleArray[3].index[1] = 3;		triangleArray[3].index[2] = 5;
	triangleArray[4].index[0] = 1;		triangleArray[4].index[1] = 5;		triangleArray[4].index[2] = 4;
	triangleArray[5].index[0] = 2;		triangleArray[5].index[1] = 6;		triangleArray[5].index[2] = 4;
	triangleArray[6].index[0] = 2;		triangleArray[6].index[1] = 3;		triangleArray[6].index[2] = 6;
	triangleArray[7].index[0] = 2;		triangleArray[7].index[1] = 5;		triangleArray[7].index[2] = 3;
	triangleArray[8].index[0] = 2;		triangleArray[8].index[1] = 4;		triangleArray[8].index[2] = 5;

	// Bind halfEdges
	//TOP///////////////////////
	//first tri-----------------
	e[1].vertex = 1; e[1].triangle = 1;
	triEPtr[1] = 1;

	e[1].nextEdge = 2; e[2].vertex = 4; e[2].triangle = 1;
	e[2].nextEdge = 3; e[3].vertex = 6; e[3].triangle = 1;
	e[3].nextEdge = 1;

	vInfoArray[1].edgePtr = 3;

	//second tri--------------
	e[4].vertex = 1; e[4].triangle = 2;
	triEPtr[2] = 4;

	e[4].nextEdge = 5; e[5].vertex = 6; e[5].triangle = 2;
	e[5].nextEdge = 6; e[6].vertex = 3; e[6].triangle = 2;
	e[6].nextEdge = 4;

	vInfoArray[6].edgePtr = 4;

	//third tri----------------
	e[7].vertex = 1; e[7].triangle = 3;
	triEPtr[3] = 7;

	e[7].nextEdge = 8; e[8].vertex = 3; e[8].triangle = 3;
	e[8].nextEdge = 9; e[9].vertex = 5; e[9].triangle = 3;
	e[9].nextEdge = 7;

	vInfoArray[3].edgePtr = 7;

	//fourth tri-----------------
	e[10].vertex = 1; e[10].triangle = 4;
	triEPtr[4] = 10;

	e[10].nextEdge = 11; e[11].vertex = 5; e[11].triangle = 4;
	e[11].nextEdge = 12; e[12].vertex = 4; e[12].triangle = 4;
	e[12].nextEdge = 10;

	vInfoArray[5].edgePtr = 10;

	//BOTTOM///////////////////////////////////
	//fifth tri---------------------------
	e[13].vertex = 2; e[13].triangle = 5;
	triEPtr[5] = 13;

	e[13].nextEdge = 14; e[14].vertex = 6; e[14].triangle = 5;
	e[14].nextEdge = 15; e[15].vertex = 4; e[15].triangle = 5;
	e[15].nextEdge = 13;

	vInfoArray[2].edgePtr = 15;

	//sixth tri-----------------------
	e[16].vertex = 2; e[16].triangle = 6;
	triEPtr[6] = 16;

	e[16].nextEdge = 17; e[17].vertex = 3; e[17].triangle = 6;
	e[17].nextEdge = 18; e[18].vertex = 6; e[18].triangle = 6;
	e[18].nextEdge = 16;

	//seventh tri---------------------
	e[19].vertex = 2; e[19].triangle = 7;
	triEPtr[7] = 19;

	e[19].nextEdge = 20; e[20].vertex = 5; e[20].triangle = 7;
	e[20].nextEdge = 21; e[21].vertex = 3; e[21].triangle = 7;
	e[21].nextEdge = 19;

	//seventh tri
	e[22].vertex = 2; e[22].triangle = 8;
	triEPtr[8] = 22;

	e[22].nextEdge = 23; e[23].vertex = 4; e[23].triangle = 8;
	e[23].nextEdge = 24; e[24].vertex = 5; e[24].triangle = 8;
	e[24].nextEdge = 22;

	vInfoArray[4].edgePtr = 22;

	//TOP SIBLINGS
	e[triEPtr[1]].sibling = e[e[triEPtr[4]].nextEdge].nextEdge;
	e[e[e[triEPtr[4]].nextEdge].nextEdge].sibling = triEPtr[1];

	e[triEPtr[4]].sibling = e[e[triEPtr[3]].nextEdge].nextEdge;
	e[e[e[triEPtr[3]].nextEdge].nextEdge].sibling = triEPtr[4];

	e[triEPtr[3]].sibling = e[e[triEPtr[2]].nextEdge].nextEdge;
	e[e[e[triEPtr[2]].nextEdge].nextEdge].sibling = triEPtr[3];

	e[triEPtr[2]].sibling = e[e[triEPtr[1]].nextEdge].nextEdge;
	e[e[e[triEPtr[1]].nextEdge].nextEdge].sibling = triEPtr[2];

	//BOTTOM SIBLINGS
	e[triEPtr[5]].sibling = e[e[triEPtr[6]].nextEdge].nextEdge;
	e[e[e[triEPtr[6]].nextEdge].nextEdge].sibling = triEPtr[5];

	e[triEPtr[6]].sibling = e[e[triEPtr[7]].nextEdge].nextEdge;
	e[e[e[triEPtr[7]].nextEdge].nextEdge].sibling = triEPtr[6];

	e[triEPtr[7]].sibling = e[e[triEPtr[8]].nextEdge].nextEdge;
	e[e[e[triEPtr[8]].nextEdge].nextEdge].sibling = triEPtr[7];

	e[triEPtr[8]].sibling = e[e[triEPtr[5]].nextEdge].nextEdge;
	e[e[e[triEPtr[5]].nextEdge].nextEdge].sibling = triEPtr[8];

	//MIDDLE SIBLINGS
	e[e[triEPtr[1]].nextEdge].sibling = e[triEPtr[5]].nextEdge;
	e[e[triEPtr[5]].nextEdge].sibling = e[triEPtr[1]].nextEdge;

	e[e[triEPtr[2]].nextEdge].sibling = e[triEPtr[6]].nextEdge;
	e[e[triEPtr[6]].nextEdge].sibling = e[triEPtr[2]].nextEdge;

	e[e[triEPtr[3]].nextEdge].sibling = e[triEPtr[7]].nextEdge;
	e[e[triEPtr[7]].nextEdge].sibling = e[triEPtr[3]].nextEdge;

	e[e[triEPtr[4]].nextEdge].sibling = e[triEPtr[8]].nextEdge;
	e[e[triEPtr[8]].nextEdge].sibling = e[triEPtr[4]].nextEdge;

	while (stepRad < rad) {
		for (int j = 1; j < vertexCap + 1; j++) {
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
		for (int j = 1; j < vertexCap + 1; j++) {
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

void DynamicMesh::generateMC(ScalarField _sf) {

	int cubeIndex;

	float xyz[8][3];
	float vertList[12][3];
	unsigned char val[8];
	unsigned char dVal;

	for (int x = 0; x < _sf.res[0] - 1; x++){
		for (int y = 0; y < _sf.res[1] - 1; y++){
			for (int z = 0; z < _sf.res[2] - 1; z++){
				
				xyz[0][0] = (double)(x - (_sf.res[0] / 2)) / ((double)(_sf.res[0] / 2))*_sf.dim[0];
				xyz[0][1] = (double)(y - (_sf.res[1] / 2)) / ((double)(_sf.res[1] / 2))*_sf.dim[1];
				xyz[0][2] = (double)(z - (_sf.res[2] / 2)) / ((double)(_sf.res[2] / 2))*_sf.dim[2];
				val[0] = _sf.data[x][y][z];

				xyz[1][0] = (double)(x+1 - (_sf.res[0] / 2)) / ((double)(_sf.res[0] / 2))*_sf.dim[0];
				xyz[1][1] = (double)(y - (_sf.res[1] / 2)) / ((double)(_sf.res[1] / 2))*_sf.dim[1];
				xyz[1][2] = (double)(z - (_sf.res[2] / 2)) / ((double)(_sf.res[2] / 2))*_sf.dim[2];
				val[1] = _sf.data[x+1][y][z];

				xyz[2][0] = (double)(x+1 - (_sf.res[0] / 2)) / ((double)(_sf.res[0] / 2))*_sf.dim[0];
				xyz[2][1] = (double)(y - (_sf.res[1] / 2)) / ((double)(_sf.res[1] / 2))*_sf.dim[1];
				xyz[2][2] = (double)(z+1 - (_sf.res[2] / 2)) / ((double)(_sf.res[2] / 2))*_sf.dim[2];
				val[2] = _sf.data[x+1][y][z+1];

				xyz[3][0] = (double)(x - (_sf.res[0] / 2)) / ((double)(_sf.res[0] / 2))*_sf.dim[0];
				xyz[3][1] = (double)(y - (_sf.res[1] / 2)) / ((double)(_sf.res[1] / 2))*_sf.dim[1];
				xyz[3][2] = (double)(z+1 - (_sf.res[2] / 2)) / ((double)(_sf.res[2] / 2))*_sf.dim[2];
				val[3] = _sf.data[x][y][z+1];

				xyz[4][0] = (double)(x - (_sf.res[0] / 2)) / ((double)(_sf.res[0] / 2))*_sf.dim[0];
				xyz[4][1] = (double)(y+1 - (_sf.res[1] / 2)) / ((double)(_sf.res[1] / 2))*_sf.dim[1];
				xyz[4][2] = (double)(z - (_sf.res[2] / 2)) / ((double)(_sf.res[2] / 2))*_sf.dim[2];
				val[4] = _sf.data[x][y+1][z];

				xyz[5][0] = (double)(x+1 - (_sf.res[0] / 2)) / ((double)(_sf.res[0] / 2))*_sf.dim[0];
				xyz[5][1] = (double)(y+1 - (_sf.res[1] / 2)) / ((double)(_sf.res[1] / 2))*_sf.dim[1];
				xyz[5][2] = (double)(z - (_sf.res[2] / 2)) / ((double)(_sf.res[2] / 2))*_sf.dim[2];
				val[5] = _sf.data[x+1][y+1][z];

				xyz[6][0] = (double)(x+1 - (_sf.res[0] / 2)) / ((double)(_sf.res[0] / 2))*_sf.dim[0];
				xyz[6][1] = (double)(y+1 - (_sf.res[1] / 2)) / ((double)(_sf.res[1] / 2))*_sf.dim[1];
				xyz[6][2] = (double)(z+1 - (_sf.res[2] / 2)) / ((double)(_sf.res[2] / 2))*_sf.dim[2];
				val[6] = _sf.data[x+1][y+1][z+1];

				xyz[7][0] = (double)(x - (_sf.res[0] / 2)) / ((double)(_sf.res[0] / 2))*_sf.dim[0];
				xyz[7][1] = (double)(y+1 - (_sf.res[1] / 2)) / ((double)(_sf.res[1] / 2))*_sf.dim[1];
				xyz[7][2] = (double)(z+1 - (_sf.res[2] / 2)) / ((double)(_sf.res[2] / 2))*_sf.dim[2];
				val[7] = _sf.data[x][y+1][z+1];

				cubeIndex = 0;
				if (_sf.data[x][y][z] > _sf.isoValue) cubeIndex += 1;
				if (_sf.data[x+1][y][z] > _sf.isoValue) cubeIndex += 2;
				if (_sf.data[x+1][y][z+1] > _sf.isoValue) cubeIndex += 4;
				if (_sf.data[x][y][z+1] > _sf.isoValue) cubeIndex += 8;

				if (_sf.data[x][y+1][z] > _sf.isoValue) cubeIndex += 16;
				if (_sf.data[x+1][y+1][z] > _sf.isoValue) cubeIndex += 32;
				if (_sf.data[x+1][y+1][z+1] > _sf.isoValue) cubeIndex += 64;
				if (_sf.data[x][y+1][z+1] > _sf.isoValue) cubeIndex += 128;

				/* Cube is entirely in/out of the surface */
				if (_sf.edgeTable[cubeIndex] == 0)
					return;



				/* Find the vertices where the surface intersects the cube */
				if (_sf.edgeTable[cubeIndex] & 1){
					dVal = (_sf.isoValue - val[0]) / (val[1] - val[0]);
					vertList[0][0] = xyz[0][0] + dVal*(xyz[1][0] - xyz[0][0]);
					vertList[0][1] = xyz[0][1] + dVal*(xyz[1][1] - xyz[0][1]);
					vertList[0][2] = xyz[0][2] + dVal*(xyz[1][2] - xyz[0][2]);
						//VertexInterp(_sf.isoValue, grid.p[0], grid.p[1], grid.val[0], grid.val[1]);
				}
					
				if (_sf.edgeTable[cubeIndex] & 2){
					vertList[1] =
						VertexInterp(_sf.isoValue, grid.p[1], grid.p[2], grid.val[1], grid.val[2]);

				}
				if (_sf.edgeTable[cubeIndex] & 4){
					vertList[2] =
						VertexInterp(_sf.isoValue, grid.p[2], grid.p[3], grid.val[2], grid.val[3]);

				}
				if (_sf.edgeTable[cubeIndex] & 8){
					vertList[3] =
						VertexInterp(_sf.isoValue, grid.p[3], grid.p[0], grid.val[3], grid.val[0]);

				}
				if (_sf.edgeTable[cubeIndex] & 16){
					vertList[4] =
						VertexInterp(_sf.isoValue, grid.p[4], grid.p[5], grid.val[4], grid.val[5]);

				}
				if (_sf.edgeTable[cubeIndex] & 32){
					vertList[5] =
						VertexInterp(_sf.isoValue, grid.p[5], grid.p[6], grid.val[5], grid.val[6]);

				}
				if (_sf.edgeTable[cubeIndex] & 64){
					vertList[6] =
						VertexInterp(_sf.isoValue, grid.p[6], grid.p[7], grid.val[6], grid.val[7]);

				}
				if (_sf.edgeTable[cubeIndex] & 128){
					vertList[7] =
						VertexInterp(_sf.isoValue, grid.p[7], grid.p[4], grid.val[7], grid.val[4]);

				}
				if (_sf.edgeTable[cubeIndex] & 256){
					vertList[8] =
						VertexInterp(_sf.isoValue, grid.p[0], grid.p[4], grid.val[0], grid.val[4]);

				}
				if (_sf.edgeTable[cubeIndex] & 512){
					vertList[9] =
						VertexInterp(_sf.isoValue, grid.p[1], grid.p[5], grid.val[1], grid.val[5]);

				}
				if (_sf.edgeTable[cubeIndex] & 1024){
					vertList[10] =
						VertexInterp(_sf.isoValue, grid.p[2], grid.p[6], grid.val[2], grid.val[6]);

				}
				if (_sf.edgeTable[cubeIndex] & 2048){
					vertList[11] =
						VertexInterp(_sf.isoValue, grid.p[3], grid.p[7], grid.val[3], grid.val[7]);

				}
				

				for (int i = 0; _sf.triTable[cubeIndex][i] != -1; i += 3)
				{
					triangleArray[triangleCap].index[0] = 0;
					triangleArray[triangleCap].index[1] = 0;
					triangleArray[triangleCap].index[2] = 0;

					triangleCap++;
				}
				
			}
		}
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

void DynamicMesh::exportToObj() {

	std::cout << "exporting mesh to .obj..." << std::endl;

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

	std::ofstream file;
	file.open("../exportedFiles/" + fileName + ".obj");

	if (file.is_open()) {

		// TODO: clean up the arrays empty slots before exporting

		// write all vertecies
		for (int i = 1; i <= vertexCap; i++) {
			file << "v " << vertexArray[i].xyz[0] << " " << vertexArray[i].xyz[1] << " " << vertexArray[i].xyz[2] << "\n";
			file << "vn " << vertexArray[i].nxyz[0] << " " << vertexArray[i].nxyz[1] << " " << vertexArray[i].nxyz[2] << "\n";
		}
		
		// write triangle indecies
		for (int i = 1; i <= triangleCap; i++) {
			//  only write the valid vertex indecies
			if (triangleArray[i].index[0] != 0) 
				file << "f " << triangleArray[i].index[0] << " " << triangleArray[i].index[1] << " " << triangleArray[i].index[2] << "\n";
		}

		file.close();

		std::cout << "export complete" << std::endl;
	}
	else {
		std::cout << "could not open file for export" << std::endl;
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
	for (int i = 0; i < listSize; i++) {
		vert3 = changeList[i];
		vInfoArray[vert3].selected = 4.0f;

		if (vInfoArray[vert3].edgePtr < 0)
			continue;

		//BUGTEST
		//if (vInfoArray[vert3].edgePtr == 0)
		//	std::cout << vInfoArray[vert3].edgePtr;
		//else
		//	std::cout << vInfoArray[vert3].edgePtr;

		tempEdge = e[e[vInfoArray[vert3].edgePtr].nextEdge].sibling;
		vPoint1 = vertexArray[vert3].xyz;

		do {
			if (vInfoArray[e[tempEdge].vertex].selected < 4.0f) {
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

					if (edgeLength2 < MAX_LENGTH) {
						edgeFlip(tempE);

						if (edgeLength2 < MIN_LENGTH) {
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
			else {
				tempEdge = e[e[tempEdge].nextEdge].sibling;
			}
		} while (tempEdge != vInfoArray[vert3].edgePtr);

		// check if tempE was removed
		if (e[tempE].nextEdge > 0) {
			//now do it for vertexEPtr aswell---------------------------
			int temptemptemp = tempE;
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

				if (edgeLength2 < MAX_LENGTH) {
					edgeFlip(tempE);
					if (edgeLength2 < MIN_LENGTH) {
						edgeCollapse(false, tempE);
					}
				}
				//---------------------
				//should not be flipped, should be split
				else {
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
		}
		
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
	//if (emptyE == 0)//BUGTEST|||||||||||
	//	emptyE = emptyE;

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
	
	//check for special cases, test if two edges have more than 2 vertecies incommon
	tempE = e[e[edge].nextEdge].sibling;
	tempE2 = e[e[e[edge].sibling].nextEdge].sibling;
	do {
		do {
			if (e[tempE2].vertex == e[tempE].vertex)
				ctr++;

			tempE2 = e[e[tempE2].nextEdge].sibling;
		} while (tempE2 != e[edge].sibling);

		tempE2 = e[e[e[edge].sibling].nextEdge].sibling;
		tempE = e[e[tempE].nextEdge].sibling;
	} while (tempE != edge);

	if (ctr != 2) {
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
	while (tempE != e[e[edge].sibling].nextEdge) {
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
