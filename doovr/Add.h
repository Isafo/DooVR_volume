#pragma once
#include "ScalarTool.h"
class Add : public ScalarTool
{
public:
	Add();
	~Add();

	void render(MatrixStack* MVstack, GLint locationMV);

	void setRadius(float rad){ radius = rad; };
	void setStrength(float str){ strength = str; };

	void changeScalarData(DynamicMesh* _mesh, Wand* _wand, Octree* _ot );
};

