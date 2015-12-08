#pragma once
#include "Mesh.h"
#include "MatrixStack.h"
#include <math.h>
#include "LineCube.h"

class Octant
{
public:
	Octant(int _depth, Octant* _parent, float x, float y, float z, float _hafDim);
	~Octant();

	void deAllocate();

	const int MAX_DEPTH = 9;

	void allocateData();
	void deAllocateData();

	void partition();

	LineCube* shape;
	void render(MatrixStack* MVstack, GLint locationMV);

	float pos[3];
	float halfDim;

	Octant* child[8];
	Octant* parent;
	int depth;

	unsigned char data;
	bool isoBool;

	int vertices;
	int triangles;

private:



};

