#pragma once
#include "Octant.h"
class Octree
{
public:
	Octree(double _dim[3]);

	~Octree();

private:
	Octant* root;

	//const int MAX_DEPTH = 8;
	double dim[3];
	//int scalarRes[3];

};

