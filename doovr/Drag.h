#pragma once
#include "Tool.h"
#include "Line.h"
#include "Circle.h"

class Drag : public Tool
{
public:
	Drag(DynamicMesh* mesh, Wand* wand);
	~Drag();

	void render(MatrixStack* MVstack, GLint locationMV);
	void renderIntersection(MatrixStack* MVstack, GLint locationMV);

	void findIntersection(DynamicMesh* mesh, Wand* wand, int triIndex){};
	void firstSelect(DynamicMesh* mesh, Wand* wand);
	void moveVertices(DynamicMesh* mesh, Wand* wand, float dT);
	void deSelect();

	void setRadius(float rad) { radius = rad; };
	void setStrength(float str){};

	void getIntersection(float* intersectionP, float* intersectionN){ intersectionP[0] = intersection.xyz[0]; 
																	intersectionP[1] = intersection.xyz[1]; 
																	intersectionP[2] = intersection.xyz[2];
																	intersectionN[0] = intersection.nxyz[0];
																	intersectionN[1] = intersection.nxyz[1];
																	intersectionN[2] = intersection.nxyz[2]; };

private:
	float radius;

	Line* pointer;
	float lineOffset[3];

	Circle* iCircle;

	vertex intersection;

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