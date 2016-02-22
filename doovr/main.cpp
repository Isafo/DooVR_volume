/*! \mainpage DooVR
*
* \section intro_sec Introduction
*
* This is the introduction.
*
* \section install_sec Compiling and running
* DooVR is tested to work with the compiler VC12. 
*
* - Get all requierd libraries\ref found in \ref dependencies_sec and install them. 
* - Create a project in Visual studio 2013 and add the files in DooVR
* - Link the required libraries in the project properties
* - Build and run the project
* 
*
* \section dependencies_sec Dependencies
*	- OpenGL
*	- GLFW
*	- GLEW
*	- libOVR
*	- wand3D
*
*/

#include <iostream>
#include <time.h>

#include "Utilities.h"
#include "Oculus.h"

#include "Shader.h"
#include "MatrixStack.h"
#include "Entity.h"
#include "Sphere.h"



//! main 

/*!
Very short function. Simply decides which mode the user wants to use and then 
redirects to two different namespaces depending on the choice. 
*/
int main() {

	char test{ 0 };
	int size = sizeof(test);

	int runSuccess = 0;

	runSuccess = Oculus::runOvr();
	
	return runSuccess;
}
