#pragma once
#include "Utilities.h"
#include "Mesh.h"

struct texCoords {
	GLfloat u;
	GLfloat v;
};

class StaticMesh : public Mesh
{
public:
	StaticMesh();
	~StaticMesh();

	void render() = 0;
	void createBuffers() = 0;

protected:
	texCoords* texArray;
};

