#pragma once
#include "Mesh.h"
class Octant
{
public:
	Octant(int _depth, Octant* _parent, float x, float y, float z, float _hafDim);
	~Octant();

	void deAllocate();

	void allocateData5();
	void deAllocateData5();

	void allocateData6();
	void deAllocateData6();

	void partition();

	float pos[3];
	float halfDim;

	Octant* child[8];
	Octant* parent;
	int depth;

	unsigned char*** data;
	int fillCount;

private:

	vertex* vertices;
	triangle* triangles;
	
	int vCount;
	int triCount;

	static const int MAX_DEPTH = 6;

};

