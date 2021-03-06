#pragma once
#include "Utilities.h"
#include "Mesh.h"

struct texCoords {
	GLfloat u;
	GLfloat v;
};

struct sBufferData {
	GLfloat x;
	GLfloat y;
	GLfloat z;
	GLfloat nx;
	GLfloat ny;
	GLfloat nz;
	GLfloat u;
	GLfloat v;
};

class StaticMesh : public Mesh
{
public:
	StaticMesh();
	~StaticMesh();

	void render() override;
	void createBuffers() override;

	void load(std::string fileName);

private:
	texCoords* texArray;
	void clean();
};

