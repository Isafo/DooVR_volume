#pragma once
#include "VertexTool.h"
#include "Line.h"
#include "Circle.h"

class Drag : public VertexTool
{
public:
	Drag(DynamicMesh* mesh, Wand* wand);
	~Drag();

	void render(MatrixStack* MVstack, GLint locationMV) override;
	void renderIntersection(MatrixStack* MVstack, GLint locationMV) override;

	void findIntersection(DynamicMesh* mesh, Wand* wand, int triIndex) override {};
	void firstSelect(DynamicMesh* mesh, Wand* wand) override;
	void moveVertices(DynamicMesh* mesh, Wand* wand, float dT) override;
	void deSelect() override;

	void setRadius(float rad) override { radius = rad; };
	void setStrength(float str) override {};

	void getIntersection(float* intersectionP, float* intersectionN) override {
		intersectionP[0] = intersection.xyz[0];
		intersectionP[1] = intersection.xyz[1];
		intersectionP[2] = intersection.xyz[2];
		intersectionN[0] = intersection.nxyz[0];
		intersectionN[1] = intersection.nxyz[1];
		intersectionN[2] = intersection.nxyz[2];
	};

private:

	Line* pointer;
	float lineOffset[3];

	Circle* iCircle;

	int* previouslySelectedVertices;
	int previouslySelectedSize;

	vertex* mVertexArray;
	halfEdge* mEdgeArray;
	vInfo* mVInfoArray;
	float* mPosition;
	float* mOrientation;

	float* wPosition;
	float* wDirection;
	float* wOrientation;

	float mMAX_LENGTH;

	float tempVec[3];
	float zVec[3];
	float iTransform[16];

	float midPoint[3];
	float midLength;
	int mIndex;

	float wPoint[4]; float newWPoint[4]; float Dirr[4]; float newDirr[4]; float vel[4];
	float spherePos[4];
};