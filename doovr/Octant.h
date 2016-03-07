#pragma once
#include "Mesh.h"
#include "MatrixStack.h"
#include "LineCube.h"

#include <math.h>
#include <vector>

class DynamicMesh;

class Octant;

struct octantStackElement {
	Octant* octant;
	int index{ 0 };
	unsigned char deallocationBool{ 0 }; // 0 nothing has happened, 1 octant has children and no check will be done, 2 deallocation has been done no children exists
};

class Octant
{
public:
	Octant();
	Octant(int _depth, Octant* _parent, float x, float y, float z, float _hafDim, bool _fill);
	~Octant();

	void deAllocate(DynamicMesh* _mesh);

	static const int MAX_DEPTH = 8;

	void allocateData();
	void deAllocateData();

	void partition();

	void collisionCheck();

	void checkHomogeneity();
	void checkHomogeneity(std::vector<octantStackElement>& octStack);


	void findNeighbors(Octant* _oNeighbors[7]);

	//LineCube* shape;
	void render(MatrixStack* MVstack, GLint locationMV);

	float pos[3];
	float halfDim;

	Octant* child[8];
	Octant* parent;
	int depth;

	unsigned char scalarValue;
	bool isoBool;

	int vertices[3];
	
	int tCount;
	int* triangles;

private:



};



