#include "Octant.h"


Octant::Octant(int _depth)
{
	data = new unsigned char**[1];
	data[0] = new unsigned char*[1];
	data[0][0] = new unsigned char[1];

	depth = _depth;
}


Octant::~Octant()
{
}
