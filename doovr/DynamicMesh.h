#include "Utilities.h"
#include "linAlg.h"
#include "Mesh.h"
#include "Wand.h"
#include <vector>

struct dBufferData {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat nx;
	GLfloat ny;
	GLfloat nz;
	GLfloat selected;
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

struct vInfo {
	int edgePtr;
	GLfloat selected = 0;
};

//! A class representing a modifiable 3D mesh 
class DynamicMesh : public Mesh{
  public:
	DynamicMesh(float rad);
	DynamicMesh(std::string fileName);
	~DynamicMesh();

	void select(Wand* wand, float rad);

	//dilate/erode based modelling
	void pull(Wand* wand, float rad);
	void push(Wand* wand, float rad);
	void drag(Wand* wand, float rad);
	void markUp(Wand* wand, float rad);

	void render();
	void render(unsigned int PrimID);

	void createBuffers();
	void updateOGLData();

	//! saves the mesh as a binary file with the current date and time as file name (yyyy-mm-dd_hh-mm-ss.bin)
	void save();

	void load(std::string _fileName);

  private:

	const int MAX_NR_OF_VERTICES = 1000000;
	int emptyV;

	const int MAX_NR_OF_TRIANGLES = 2 * MAX_NR_OF_VERTICES;
	int emptyT;

	const int MAX_NR_OF_EDGES = 3 * MAX_NR_OF_VERTICES;
	int emptyE;

	//largest index in the vertexArray where values exist 
	int vertexCap;
	//largest index in the indexArray where values exist 
	int triangleCap;

	//Array that exists parallell to the vertexArray and contains indices to an edge that is connected to the corresponding triangle 
	vInfo* vInfoArray;

	//Array that exists parallell to the indexArray and contains indices to an edge that is part of the corresponding triangle 
	int* triEPtr;

	//Array that stores all halfEdges of the mesh
	halfEdge* e;
	int nrofEdges;
	//largest index in the edgeArray where values exist 
	int edgeCap;
	
	sVert sVerts[10000];
	int sVertsNR = 0;

	const float MAX_LENGTH = 0.025f  * 0.5f; // 0.08f*0.1f;
	const float MIN_LENGTH = 0.0124f * 0.5f;

	std::string fileName;

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

	//! updates the changed vertecies normal and checks if retriangulation is needed.
	void updateArea(sVert* changeList, int listSize);

};