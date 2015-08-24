#pragma once
#include "Tool.h"
#include "Line.h"
#include "Circle.h"

class Draw : public Tool
{
public:
	Draw(DynamicMesh* mesh, Wand* wand);
	~Draw();

	void render(MatrixStack* MVstack, GLint locationMV);
	void renderIntersection(MatrixStack* MVstack, GLint locationMV);

	void firstSelect(DynamicMesh* mesh, Wand* wand);
	void moveVertices(DynamicMesh* mesh, Wand* wand, float dT);
	void deSelect();

	void setRadius(float rad) { radius = rad; };
	void setStrength(float str){};

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
};

