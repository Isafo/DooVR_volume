#pragma once
#include "ScalarTool.h"



class Remove : public ScalarTool
{
public:
	Remove();
	~Remove();

	void render(MatrixStack* MVstack, GLint locationMV) override;

	void setRadius(float rad) override { radius = rad; };
	void setStrength(float str) override { strength = str; };

	void changeScalarData(DynamicMesh* _mesh, Wand* _wand, Octree* _ot) override;

	std::vector<Octant*> octList;

};

