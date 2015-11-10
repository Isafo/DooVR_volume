#pragma once
#include "VertexTool.h"
#include "Line.h"
#include "Circle.h"

class Smooth : public VertexTool
{
public:
	Smooth(DynamicMesh* mesh, Wand* wand);
	~Smooth();

	void render(MatrixStack* MVstack, GLint locationMV);
	void renderIntersection(MatrixStack* MVstack, GLint locationMV);

	void findIntersection(DynamicMesh* mesh, Wand* wand, int triIndex){};
	void firstSelect(DynamicMesh* mesh, Wand* wand);
	void moveVertices(DynamicMesh* mesh, Wand* wand, float dT);
	void deSelect();

	void setRadius(float rad) { radius = rad; };
	void setStrength(float str){ strength = abs(str)/0.075f; };

	void getIntersection(float* intersectionP, float* intersectionN){ intersectionP[0] = intersection.xyz[0]; 
																	intersectionP[1] = intersection.xyz[1]; 
																	intersectionP[2] = intersection.xyz[2];
																	intersectionN[0] = intersection.nxyz[0];
																	intersectionN[1] = intersection.nxyz[1];
																	intersectionN[2] = intersection.nxyz[2]; };

private:

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

	float counter = 0.0f;
};

