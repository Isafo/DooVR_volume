#include "Octant.h"


Octant::Octant(int _depth, float x, float y, float z, float _halfDim)
{
	data = new unsigned char**[1];
	data[0] = new unsigned char*[1];
	data[0][0] = new unsigned char[1];

	depth = _depth;
	pos[0] = x; pos[1] = y; pos[2] = z;
	halfDim = _halfDim;
}


Octant::~Octant()
{
}
