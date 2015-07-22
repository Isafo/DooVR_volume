#pragma once
#include "Entity.h"
#include "Utilities.h"
#include <vector>
#include "StaticMesh.h"

class Plane :
	public StaticMesh
{

public:
	Plane()
	{
		vao = 0;
		vertexbuffer = 0;
		indexbuffer = 0;
		nrofTris = 0;
		nrofVerts = 0;
	};

	Plane(float x, float y, float z, float dX, float dZ);
	~Plane(void);

	void render();
	void createBuffers();

private:

};
