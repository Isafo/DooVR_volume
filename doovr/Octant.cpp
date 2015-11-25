#include "Octant.h"
#include <vector>


Octant::Octant(int _depth, Octant* _parent, float x, float y, float z, float _halfDim)
{
	data = new unsigned char**[1];
	data[0] = new unsigned char*[1];
	data[0][0] = new unsigned char[1];
	data[0][0][0] = 0;
	fillCount = 0;

	depth = _depth;
	pos[0] = x; pos[1] = y; pos[2] = z;
	halfDim = _halfDim;

}


Octant::~Octant() {
	
}


void Octant::partition()
{
	float d = halfDim / 2.0f;
	child[0] = new Octant(depth + 1, this, pos[0] - d, pos[0] - d, pos[0] + d, d);
	child[1] = new Octant(depth + 1, this, pos[0] + d, pos[0] - d, pos[0] + d, d);
	child[2] = new Octant(depth + 1, this, pos[0] - d, pos[0] - d, pos[0] - d, d);
	child[3] = new Octant(depth + 1, this, pos[0] + d, pos[0] - d, pos[0] - d, d);
	child[4] = new Octant(depth + 1, this, pos[0] - d, pos[0] + d, pos[0] + d, d);
	child[5] = new Octant(depth + 1, this, pos[0] + d, pos[0] + d, pos[0] + d, d);
	child[6] = new Octant(depth + 1, this, pos[0] - d, pos[0] + d, pos[0] - d, d);
	child[7] = new Octant(depth + 1, this, pos[0] + d, pos[0] + d, pos[0] - d, d);
}

void Octant::deAllocate() {

	delete this->child[0];

	std::vector<Octant*> childList;
	childList.reserve(256);

	if (this->child[0] != nullptr) { 

		childList.push_back(child[0]);
		childList.push_back(child[1]);
		childList.push_back(child[2]);
		childList.push_back(child[3]);
		childList.push_back(child[4]);
		childList.push_back(child[5]);
		childList.push_back(child[6]);
		childList.push_back(child[7]);
		
		int octCounter = 0;
		while (octCounter < childList.size()) {

			if (childList[octCounter]->child[0] != nullptr)
			{
				childList.push_back(childList[octCounter]->child[0]);
				childList.push_back(childList[octCounter]->child[1]);
				childList.push_back(childList[octCounter]->child[2]);
				childList.push_back(childList[octCounter]->child[3]);
				childList.push_back(childList[octCounter]->child[4]);
				childList.push_back(childList[octCounter]->child[5]);
				childList.push_back(childList[octCounter]->child[6]);
				childList.push_back(childList[octCounter]->child[7]);
			}
			delete childList[octCounter]->data;
			delete childList[octCounter];

			octCounter++;
		}


		
}

void Octant::allocateData5()
{
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

