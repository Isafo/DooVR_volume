//! Namespace that handles necassary includes and various functions
#pragma once

#include "GL/glew.h"

#include <Windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL

#ifdef __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#endif
#include "GLFW/glfw3.h"
#include <GLFW/glfw3native.h>

#include <iomanip>      // std::setprecision

namespace Utilities {
	/*
	double displayFPS(GLFWwindow* window);
	*/
}