#include "Octree.h"


Octree::Octree(double _dim[3])
{
	dim[0] = _dim[0]; dim[1] = _dim[1]; dim[2] = _dim[2];
	root = new Octant(0);
}


Octree::~Octree()
{

}
