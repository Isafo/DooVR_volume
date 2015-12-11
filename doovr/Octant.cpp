#include "Octant.h"
#include <vector>
#include "DynamicMesh.h"


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

	//shape = new LineCube(0.0f, 0.0f, 0.0f, _halfDim * 2, _halfDim * 2, _halfDim * 2);

	parent = _parent;
	depth = _depth;
	pos[0] = x; pos[1] = y; pos[2] = z;
	halfDim = _halfDim;
	//TODO: maybe do manually
	data = 0;
	isoBool = false;
	//vertices[0] = -1;
	triangles = nullptr;

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
		child[0] = nullptr;
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

void Octant::deAllocate(DynamicMesh* _mesh) {
	for (int i = 0; i < 8; i++) {

		// child has no children 
		if (child[i]->child[0] == nullptr) {
			// data is allocated
			if (child[i]->triangles != nullptr) {
				//delete triangle data
				for (int j = 0; j < child[i]->tCount; j++){
					_mesh->emptyTStack.push_back(child[i]->triangles[j]);
					delete _mesh->triangleArray[child[i]->triangles[j]];
				}
				delete[] triangles;
				
				//delete vertex data
				if (child[i]->vertices[0] != -1){
					_mesh->emptyVStack.push_back(child[i]->vertices[0]);
					delete _mesh->vertexArray[child[i]->vertices[0]];
				}
				if (child[i]->vertices[1] != -1){
					_mesh->emptyVStack.push_back(child[i]->vertices[1]);
					delete _mesh->vertexArray[child[i]->vertices[1]];
				}
				if (child[i]->vertices[2] != -1){
					_mesh->emptyVStack.push_back(child[i]->vertices[2]);
					delete _mesh->vertexArray[child[i]->vertices[2]];
				}
			}
		}
		// child has children
		else {
			child[i]->deAllocate(_mesh);
		}
		// clean up
		delete child[i];	
	}
	child[0] = nullptr;

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
			//shape->render();
		MVstack->pop();
	}
}