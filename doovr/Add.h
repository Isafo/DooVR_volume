/* Copyright (C) DooVR - All Rights Reserved
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Olle Grahn <ollgr444@student.liu.se> and Isabelle Forsman <isafo268@student.liu.se>, July 2015 - Mars 2016
*/

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

