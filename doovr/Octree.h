#pragma once
#include "Octant.h"
class Octree
{
public:
	Octree(float _dim);

	~Octree();

	Octant* root;

	const int MAX_DEPTH = 1000;
	float halfDim;
	//int scalarRes[3];

private:


};

