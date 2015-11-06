#pragma once
#include "Utilities.h"
#include "linAlg.h"
#include "Mesh.h"
#include "Wand.h"
#include "scalarField.h"
#include <vector>

#ifndef DYNAMICMESH_H
#define DYNAMICMESH_H

struct dBufferData {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat nx;
	GLfloat ny;
	GLfloat nz;
	GLfloat selected;
};

//! HalfEdge data structure. HalfEdge points to the next edge in the triangle counter clockwise.
struct halfEdge {
	int nextEdge;
	int sibling;
	int triangle;
	int vertex;
};

struct sVert {
	int index;
	float vec[3];
	float norm[3];
	float length = 0.0f;
	sVert* next;
};

struct vInfo {
	int edgePtr;
	GLfloat selected = 0;
};

#endif

//! A class representing a modifiable 3D mesh 
class DynamicMesh : public Mesh{
	friend class Smooth;
	friend class Push;
	friend class Draw;
	friend class Drag;
	friend class BuildUp;

  public:
	DynamicMesh();
	DynamicMesh(std::string fileName);
	~DynamicMesh();

	void select(Wand* wand, float rad);
	void deSelect();

	//dilate/erode based modelling

	void render();
	void render(unsigned int PrimID);

	void createBuffers();
	void updateOGLData();

	//! saves the mesh as a binary file with the current date and time as file name (yyyy-mm-dd_hh-mm-ss.bin)
	void sphereSubdivide(float rad);
	void generateMC(ScalarField _sf);

	void save();
	void load(std::string _fileName);
	//! Exports the mesh to an obj file
	/*! The exported obj file contains vertexposition, vertexnormal and triangle indecies. No texture coordinates.*/
	void exportToObj();

	void updateHVerts();

	void cleanBuffer();

  private:

	  //TODO: remove one zero
	const int MAX_NR_OF_VERTICES = 1000000;
	//! negative index of the latest removed vertex
	/*! vInfoArray's edgepointer contains the negative index of an empty slot in the vertexArray*/
	int emptyV;

	const int MAX_NR_OF_TRIANGLES = 2 * MAX_NR_OF_VERTICES;
	//! negative index of the latest removed triangle
	/*! triEPtr contains the negative index of an empty slot in the triangleArray*/
	int emptyT;

	const int MAX_NR_OF_EDGES = 3 * MAX_NR_OF_VERTICES;
	//! negative index of the latest removed edge
	/*! nextEdge contains the negative index of an empty slot in the halfEdge array e*/
	int emptyE;

	//largest index in the vertexArray where values exist 
	int vertexCap;
	//largest index in the indexArray where values exist 
	int triangleCap;

	//! tells the range that needs to be cleaned in the buffer after loading a smaller mesh
	int vertexRange = 0;
	//! tells the range that needs to be cleaned in the buffer after loading a smaller mesh
	int triangleRange = 0;

	//! Array that exists parallell to the vertexArray and contains indices to an edge that is connected to the corresponding triangle
	/*! An empty slot in vInfoArray and vertexArray saves the negative index of the next empty slot in the array in the edgePtr variable.*/
	vInfo* vInfoArray;
	//! Array that exists parallell to the indexArray and contains indices to an edge that is part of the corresponding triangle
	/*! An empty slot in triEPtr and triangleArray saves the negative index of the next empty slot in the array in triEPtr*/
	int* triEPtr;

	//! Array that stores all halfEdges of the mesh
	/*! An empty slot in the array saves the negative index of the next empty slot in the array in the nextEdge variable*/
	halfEdge* e;
	int nrofEdges;
	//! largest index in the edgeArray where values exist 
	int edgeCap;
	
	
	//float midPoint[3];

	const float MAX_LENGTH = 0.025f * 0.2f; // 0.08f*0.1f;
	const float MIN_LENGTH = 0.0124f * 0.2f;

	std::string fileName;

	//! adds a vertex in the middle between the vertexpoints pA and pB.
	/*! pA is the position of currVert, edge is the edge that is to long*/
	void edgeSplit(float* vPoint, float* vec, int &edge);

	void edgeFlip(int &edge);
	//! removes the edge by deleting the vertex the edge points to and moves currVert halfway towards nVert.
	void edgeCollapse(bool recursive, int &edge);

	//! subdivides the surface into a sphere
	void edgeSubdivide(float* pA, float* vecA2B, halfEdge* &edge, bool update, float rad);

	//! updates the changed vertecies normal and checks if retriangulation is needed.
	void updateArea(int* changeList, int listSize);

	//! updates the changed vertecies normal 
	void updateNormals(int* changeList, int listSize);
};