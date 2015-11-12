#include "Octant.h"


Octant::Octant(int _depth, float x, float y, float z, float _halfDim)
{
	data = new unsigned char**[1];
	data[0] = new unsigned char*[1];
	data[0][0] = new unsigned char[1];
	data[0][0][0] = 0;
	fillCount = 0;

	depth = _depth;
	pos[0] = x; pos[1] = y; pos[2] = z;
	halfDim = _halfDim;

	detailed = false;
}


Octant::~Octant()
{
}


void Octant::allocateData5()
{
	unsigned char tmpIso = data[0][0][0];
	delete data[0][0];
	delete data[0];
	delete data;

	data = new unsigned char**[32];
	for (int i = 0; i < 32; i++)
		data[i] = new unsigned char*[32];

	for (int i = 0; i < 32; i++)
		for (int j = 0; j < 32; j++)
			data[i][j] = new unsigned char[32];

	for (int i = 0; i < 32; i++)
		for (int j = 0; j < 32; j++)
			for (int k = 0; k < 32; k++)
				data[i][j][k] = tmpIso;

	detailed = true;
}

void Octant::deAllocateData5()
{
	for (int i = 0; i < 32; i++)
		for (int j = 0; j < 32; j++)
			delete data[i][j];

	for (int i = 0; i < 32; i++)
		delete data[i];
	
	delete data;

	data = new unsigned char**[1];
	data[0] = new unsigned char*[1];
	data[0][0] = new unsigned char[1];

	detailed = false;
}

void Octant::allocateData6()
{
	unsigned char tmpIso = data[0][0][0];
	delete data[0][0];
	delete data[0];
	delete data;

	data = new unsigned char**[16];
	for (int i = 0; i < 16; i++)
		data[i] = new unsigned char*[16];

	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 16; j++)
			data[i][j] = new unsigned char[16];

	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 16; j++)
			for (int k = 0; k < 16; k++)
				data[i][j][k] = tmpIso;

	detailed = true;
}

void Octant::deAllocateData6()
{
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 16; j++)
			delete data[i][j];

	for (int i = 0; i < 16; i++)
		delete data[i];

	delete data;

	data = new unsigned char**[1];
	data[0] = new unsigned char*[1];
	data[0][0] = new unsigned char[1];

	detailed = false;
}

