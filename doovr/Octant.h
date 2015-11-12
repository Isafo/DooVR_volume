#pragma once
#include "Mesh.h"
class Octant
{
public:
	Octant(int _depth, float x, float y, float z, float _hafDim);
	~Octant();

	float pos[3];
	float halfDim;
	Octant* child[8];
	int depth;

private:


	unsigned char*** data;
	vertex* vertices;
	triangle* triangles;
	
	int vCount;
	int triCount;

};

