#pragma once
#include "Tool.h"
#include "Line.h"
#include "Circle.h"

class BuildUp : public Tool
{
public:
	BuildUp(DynamicMesh* mesh, Wand* wand);
	~BuildUp();

	void render(MatrixStack* MVstack, GLint locationMV);
	void renderIntersection(MatrixStack* MVstack, GLint locationMV);

	void firstSelect(DynamicMesh* mesh, Wand* wand);
	void moveVertices(DynamicMesh* mesh, Wand* wand, float dT);
	void deSelect();

	void setRadius(float rad) { radius = rad; };
	void setStrength(float str) { strength = str; };
private:
	float radius;
	float strength;

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
};

