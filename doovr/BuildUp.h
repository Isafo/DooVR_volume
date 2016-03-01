#pragma once
#include "VertexTool.h"
#include "Line.h"
#include "Circle.h"

class BuildUp : public VertexTool
{
public:
	BuildUp(DynamicMesh* mesh, Wand* wand);
	~BuildUp();

	void render(MatrixStack* MVstack, GLint locationMV) override;
	void renderIntersection(MatrixStack* MVstack, GLint locationMV) override;

	void findIntersection(DynamicMesh* mesh, Wand* wand, int triIndex) override;
	void firstSelect(DynamicMesh* mesh, Wand* wand) override;
	void moveVertices(DynamicMesh* mesh, Wand* wand, float dT) override;
	void deSelect() override;

	void setRadius(float rad) override { radius = rad; };
	void setStrength(float str) override { strength = str; };

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
};

