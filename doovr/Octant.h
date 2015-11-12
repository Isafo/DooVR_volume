#pragma once
#include "Mesh.h"
class Octant
{
public:
	Octant(int _depth, float x, float y, float z, float _hafDim);
	~Octant();

	void allocateData5();
	void deAllocateData5();

	void allocateData6();
	void deAllocateData6();

	float pos[3];
	float halfDim;

	Octant* child[8];
	int depth;

	unsigned char*** data;
	bool detailed;
	int fillCount;

private:

	vertex* vertices;
	triangle* triangles;
	
	int vCount;
	int triCount;

};

