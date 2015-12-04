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


	const int MAX_DEPTH = 6;

	void deAllocate();

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

	unsigned char*** data;
	int fillCount;

	int* vertices;
	int* triangles;

	int vRowCount;
	int tRowCount;


	//const int V_ROW_MAX = std::pow(2.0,10.0);
	//static const int T_ROW_MAX;

private:



};

