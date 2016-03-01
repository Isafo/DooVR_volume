#pragma once
#include "Tool.h"
#include "Octree.h"
#include "Octant.h"
#include "DynamicMesh.h"




class ScalarTool :
	public Tool
{
	friend class Octant;

public:
	ScalarTool();
	virtual ~ScalarTool();

	virtual void render(MatrixStack* MVstack, GLint locationMV) = 0;

	virtual void setRadius(float rad) = 0;
	virtual void setStrength(float str) = 0;

	virtual void changeScalarData( DynamicMesh* _mesh, Wand* _wand, Octree* _ot) = 0;

protected:

};

