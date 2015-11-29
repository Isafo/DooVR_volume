#pragma once
#include "Mesh.h"
class Octant
{
public:
	Octant(int _depth, Octant* _parent, float x, float y, float z, float _hafDim);
	~Octant();

	void deAllocate();

	void allocateData();
	void deAllocateData();

	void partition();

	float pos[3];
	float halfDim;

	Octant* child[8];
	Octant* parent;
	int depth;

	unsigned char*** data;
	int fillCount;

	vertex** vertices;
	triangle** triangles;

	int vCount;
	int tCount;

	static const int MAX_DEPTH = 6;

private:



};

