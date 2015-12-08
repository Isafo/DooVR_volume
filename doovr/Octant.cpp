#include "Octant.h"
#include <vector>


Octant::Octant(int _depth, Octant* _parent, float x, float y, float z, float _halfDim) {
	/*if (_depth != MAX_DEPTH) {
		data = new unsigned char**[1];
		data[0] = new unsigned char*[1];
		data[0][0] = new unsigned char[1];
		data[0][0][0] = 0;
	}
	else {

		int scalarNR = std::pow(2, 10 - _depth);

		data = new unsigned char**[scalarNR];
		for (int i = 0; i < scalarNR; i++)
			data[i] = new unsigned char*[scalarNR];

		for (int i = 0; i < scalarNR; i++)
			for (int j = 0; j < scalarNR; j++)
				data[i][j] = new unsigned char[scalarNR];

		for (int i = 0; i < scalarNR; i++)
			for (int j = 0; j < scalarNR; j++)
				for (int k = 0; k < scalarNR; k++)
					data[i][j][k] = 0;
	}*/

	shape = new LineCube(0.0f, 0.0f, 0.0f, _halfDim * 2, _halfDim * 2, _halfDim * 2);

	parent = _parent;
	depth = _depth;
	pos[0] = x; pos[1] = y; pos[2] = z;
	halfDim = _halfDim;
	//TODO: maybe do manually
	data = 0;
	isoBool = false;
	vertices = -1;
	triangles = -1;

}


Octant::~Octant() {

	/*if (depth == MAX_DEPTH) {
		int scalarNR = std::pow(2, (10 - depth));

		for (int i = 0; i < scalarNR; i++)
			for (int j = 0; j < scalarNR; j++)
				delete data[i][j];

		for (int i = 0; i < scalarNR; i++)
			delete data[i];

		delete data;
	}
	else {
		if (child[0] != nullptr) {
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
	}*/

	if (child[0] != nullptr) {
		delete child[0];
		delete child[1];
		delete child[2];
		delete child[3];
		delete child[4];
		delete child[5];
		delete child[6];
		delete child[7];
	}
}


void Octant::partition() {
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

}

void Octant::allocateData() {
	//unsigned char tmpIso = data[0][0][0];
	/*delete data[0][0];
	delete data[0];
	delete data;

	int scalarNR = std::pow(2, 10 - depth);

	data = new unsigned char**[scalarNR];
	for (int i = 0; i < scalarNR; i++)
		data[i] = new unsigned char*[scalarNR];

	for (int i = 0; i < scalarNR; i++)
		for (int j = 0; j < scalarNR; j++)
			data[i][j] = new unsigned char[scalarNR];

	for (int i = 0; i < scalarNR; i++)
		for (int j = 0; j < scalarNR; j++)
			for (int k = 0; k < scalarNR; k++)
				data[i][j][k] = 0;*/

}

void Octant::deAllocateData() {
	/*int scalarNR = std::pow(2, 10 - depth);

	for (int i = 0; i < scalarNR; i++)
		for (int j = 0; j < scalarNR; j++)
			delete data[i][j];

	for (int i = 0; i < scalarNR; i++)
		delete data[i];
	
	delete data;

	data = new unsigned char**[1];
	data[0] = new unsigned char*[1];
	data[0][0] = new unsigned char[1];*/

}

void Octant::render(MatrixStack* MVstack, GLint locationMV) {
	
	if (child[0] != nullptr){
		child[0]->render(MVstack, locationMV);
		child[1]->render(MVstack, locationMV);
		child[2]->render(MVstack, locationMV);
		child[3]->render(MVstack, locationMV);
		child[4]->render(MVstack, locationMV);
		child[5]->render(MVstack, locationMV);
		child[6]->render(MVstack, locationMV);
		child[7]->render(MVstack, locationMV);
	}
	else {
		MVstack->push();
			MVstack->translate(pos);
			glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack->getCurrentMatrix());
			shape->render();
		MVstack->pop();
	}
}