#include "Octree.h"


Octree::Octree(float _dim)
{
	halfDim = _dim/2.0f;
	root = new Octant(0, 0.0f, 0.0f, 0.0f, 1.0f);

	float tmpF = root->halfDim / 2.0f;
	
	root->child[0] = new Octant(1, -tmpF, -tmpF, tmpF, tmpF);
	root->child[1] = new Octant(1, tmpF, -tmpF, tmpF, tmpF);
	root->child[2] = new Octant(1, -tmpF, -tmpF, -tmpF, tmpF);
	root->child[3] = new Octant(1, tmpF, -tmpF, -tmpF, tmpF);
	root->child[4] = new Octant(1, -tmpF, tmpF, tmpF, tmpF);
	root->child[5] = new Octant(1, tmpF, tmpF, tmpF, tmpF);
	root->child[6] = new Octant(1, -tmpF, tmpF, -tmpF, tmpF);
	root->child[7] = new Octant(1, tmpF, tmpF, -tmpF, tmpF);
}


Octree::~Octree()
{

}
