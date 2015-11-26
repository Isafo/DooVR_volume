#include "Octant.h"
#include <vector>


Octant::Octant(int _depth, Octant* _parent, float x, float y, float z, float _halfDim)
{
	if (_depth != MAX_DEPTH)
	{
		data = new unsigned char**[1];
		data[0] = new unsigned char*[1];
		data[0][0] = new unsigned char[1];
		data[0][0][0] = 0;
	}
	else{

		int scalarNR = std::pow(2, 10 - _depth);

		data = new unsigned char**[scalarNR];
		for (int i = 0; i < scalarNR; i++)
			data[i] = new unsigned char*[32];

		for (int i = 0; i < scalarNR; i++)
			for (int j = 0; j < scalarNR; j++)
				data[i][j] = new unsigned char[scalarNR];

		for (int i = 0; i < scalarNR; i++)
			for (int j = 0; j < scalarNR; j++)
				for (int k = 0; k < scalarNR; k++)
					data[i][j][k] = 0;
	}
	fillCount = 0;

	parent = _parent;
	depth = _depth;
	pos[0] = x; pos[1] = y; pos[2] = z;
	halfDim = _halfDim;

}


Octant::~Octant() {

	if (depth == MAX_DEPTH)
	{
		int scalarNR = std::pow(2, (10 - depth));

		for (int i = 0; i < scalarNR; i++)
			for (int j = 0; j < scalarNR; j++)
				delete data[i][j];

		for (int i = 0; i < scalarNR; i++)
			delete data[i];

		delete data;
	}
	else
	{
		if (child[0] != nullptr)
		{
			delete child[0];
			delete child[1];
			delete child[2];
			delete child[3];
			delete child[4];
			delete child[5];
			delete child[6];
			delete child[7];
		}

		delete data[0][0];
		delete data[0];
		delete data;

	}
	
}


void Octant::partition()
{
	//delete data[0][0];
	//delete data[0];
	//delete data;

	float d = halfDim / 2.0f;
	child[0] = new Octant(depth + 1, this, pos[0] - d, pos[1] - d, pos[2] - d, d);
	child[1] = new Octant(depth + 1, this, pos[0] - d, pos[1] - d, pos[2] + d, d);
	child[2] = new Octant(depth + 1, this, pos[0] - d, pos[1] + d, pos[2] - d, d);
	child[3] = new Octant(depth + 1, this, pos[0] - d, pos[1] + d, pos[2] + d, d);
	child[4] = new Octant(depth + 1, this, pos[0] + d, pos[1] - d, pos[2] - d, d);
	child[5] = new Octant(depth + 1, this, pos[0] + d, pos[1] - d, pos[2] + d, d);
	child[6] = new Octant(depth + 1, this, pos[0] + d, pos[1] + d, pos[2] - d, d);
	child[7] = new Octant(depth + 1, this, pos[0] + d, pos[1] + d, pos[2] + d, d);
}

void Octant::deAllocate() {
	
	delete child[0];
	delete child[1];
	delete child[2];
	delete child[3];
	delete child[4];
	delete child[5];
	delete child[6];
	delete child[7];

	//data = new unsigned char**[1];
	//data[0] = new unsigned char*[1];
	//data[0][0] = new unsigned char[1];
}

void Octant::allocateData5() {
	//unsigned char tmpIso = data[0][0][0];
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
				data[i][j][k] = 0;

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

}

void Octant::allocateData6()
{
	//unsigned char tmpIso = data[0][0][0];
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
				data[i][j][k] = 0;

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

}

