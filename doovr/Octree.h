#pragma once
#include "Octant.h"
class Octree
{
public:
	Octree(int _res[3]);

	~Octree();

private:
	Octant* root;

	const int MAX_DEPTH = 8;
	//int scalarRes[3];

};

