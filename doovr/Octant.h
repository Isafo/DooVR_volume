#pragma once
#include "Mesh.h"
#include "MatrixStack.h"
#include "LineCube.h"

#include <math.h>
#include <vector>

class DynamicMesh;
class Octant
{
public:
	Octant(int _depth, Octant* _parent, float x, float y, float z, float _hafDim);
	~Octant();

	const int MAX_DEPTH = 9;

	void deAllocate(DynamicMesh* _mesh);

	void allocateData();
	void deAllocateData();

	void partition();

	void findNeighbors(Octant* _oNeighbors[7]);

	//LineCube* shape;
	void render(MatrixStack* MVstack, GLint locationMV);

	

	float pos[3];
	float halfDim;

	Octant* child[8];
	Octant* parent;
	int depth;

	unsigned char data;
	bool isoBool;

	int vertices[3];
	
	int tCount;
	int* triangles;

private:



};

