#pragma once
#include "ScalarTool.h"



class Add : public ScalarTool
{
public:
	Add();
	~Add();

	void render(MatrixStack* MVstack, GLint locationMV) override;

	void setRadius(float rad) override { radius = rad; };
	void setStrength(float str) override { strength = str; };

	void changeScalarData(DynamicMesh* _mesh, Wand* _wand, Octree* _ot ) override;


};

