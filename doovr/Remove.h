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

private:

	struct octantStackElement {
		Octant* octant;
		int index{ 0 };
		unsigned char deallocationBool{ 0 }; // 0 nothing has happened, 1 octant has children and no check will be done, 2 deallocation has been done no children exists
	};
};

