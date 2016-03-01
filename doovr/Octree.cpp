#include "Octree.h"


Octree::Octree(float _dim)
{
	halfDim = _dim/2.0f;
	root = new Octant(0, nullptr, 0.0f, 0.0f, 0.0f, halfDim, false);

	root->partition();
}


Octree::~Octree()
{

}
