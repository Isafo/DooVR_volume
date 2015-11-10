#pragma once
#include "Tool.h"
#include "Octree.h"
#include "DynamicMesh.h"
class ScalarTool :
	public Tool
{
public:
	ScalarTool();
	~ScalarTool();

	virtual void render(MatrixStack* MVstack, GLint locationMV) = 0;

	virtual void setRadius(float rad) = 0;
	virtual void setStrength(float str) = 0;

	virtual void changeScalarData(Wand* _wand, Octree* _ot, DynamicMesh* _mesh) = 0;

protected:
};

