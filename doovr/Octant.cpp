#include "Octant.h"


Octant::Octant()
{
	data = new unsigned char**[1];
	data[0] = new unsigned char*[1];
	data[0][0] = new unsigned char[1];
}


Octant::~Octant()
{
}
