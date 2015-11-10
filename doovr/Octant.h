#pragma once
#include "Mesh.h"
class Octant
{
public:
	Octant(int _depth);
	~Octant();

private:
	Octant* child[8];

	unsigned char*** data;
	vertex* vertices;
	triangle* triangles;

	int depth;
	int vCount;
	int triCount;

};

