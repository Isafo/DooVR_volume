#include "Utilities.h"
#include "linAlg.h"
#include "Wand.h"
#include <vector>

typedef struct vertex vertex;

//! Data structure containing the coordinates and normal coordinates of a vertex, aswell as a pointer to an adjacent face
struct vertex {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat nx;
	GLfloat ny;
	GLfloat nz;
	GLfloat selected = 0.0f;
};

//! Data structure containing three indices of the vertexArray that make a certain triangle. Points to one edge in the triangle
struct triangle {
	GLuint index[3];
};

//! Data structure halfEdge pointing to the next edge in the triangle counter clockwise.
struct halfEdge {
	int nextEdge;
	int sibling;
	int triangle;
	int vertex;
};

struct sVert {
	int index;
	float vec[3];
	float length;
};

//! A class representing a modifiable 3D mesh 
class Mesh {
  public:
	Mesh(float rad);
	~Mesh();

	void select(Wand* wand, float rad);

	//dilate/erode based modelling
	void pull(Wand* wand, float rad);
	void push(Wand* wand, float rad);
	void drag(Wand* wand, float rad);
	void markUp(Wand* wand, float rad);

	void render();
	void render(unsigned int PrimID);

	float* getPosition(){ return position; };
	float* getOrientation(){ return orientation; };

	void setPosition(float* p) { position[0] = p[0]; position[1] = p[1]; position[2] = p[2]; }
	void setOrientation(float* o) { std::copy(o, o + 16, orientation); }

	//! updates the changed vertecies normal and checks if retriangulation is needed.
	void updateArea(sVert* changeList, int listSize);

	void updateOGLData();

  private:

	//Array that stores all halfEdges of the mesh
	halfEdge e[3000000];
	int nrofEdges;
	//largest index in the edgeArray where values exist 
	int edgeCap;

	//Array that stores all triangles of the mesh
	triangle indexArray[2000000];
	//Array that exists parallell to the indexArray and contains indices to an edge that is part of the corresponding triangle 
	int triEPtr[2000000];
	int nrofTris;
	//largest index in the indexArray where values exist 
	int triangleCap;

	//array that stres all vertices of the mesh
	vertex vertexArray[1000000];
	//Array that exists parallell to the vertexArray and contains indices to an edge that is connected to the corresponding triangle 
	int vertexEPtr[1000000];
	int nrofVerts;
	//largest index in the vertexArray where values exist 
	int vertexCap;
	
	sVert sVerts[10000];
	int sVertsNR = 0;

	float position[3];
	float orientation[16];

	const float MAX_LENGTH = 0.025f;// *0.6f; // 0.08f*0.1f;
	const float MIN_LENGTH = 0.0124f;// *0.6f;

	GLuint vao;          // Vertex array object, the main handle for geometry
	GLuint vertexbuffer; // Buffer ID to bind to GL_ARRAY_BUFFER
	GLuint indexbuffer;  // Buffer ID to bind to GL_ELEMENT_ARRAY_BUFFER

	//! adds a vertex in the middle between the vertexpoints pA and pB.
	/*! pA is the position of currVert, edge is the edge that is to long*/
	void edgeSplit(float* vPoint, float* vec, int &edge);

	void edgeFlip(int &edge);
	//! removes the vertexpoint nVert and moves currVert halfway towards nVert.
	/*! vPoint is the position of currVert, vec is the vector between the vertecies that are to close to each other,
	and edge is a pointer to the edge that is to short*/
	void edgeCollapse(bool recursive, int &edge);

	//! subdivides the surface into a sphere
	void edgeSubdivide(float* pA, float* vecA2B, halfEdge* &edge, bool update, float rad);

};