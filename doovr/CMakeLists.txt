cmake_minimum_required (VERSION 2.8)

SET(APP_NAME DooVR)

PROJECT(${APP_NAME})

file(GLOB SRCFILES
    "*.h"
    "*.cpp"
	"*.glsl"
)

add_executable(${APP_NAME} ${SRCFILES})