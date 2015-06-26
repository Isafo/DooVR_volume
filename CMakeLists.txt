cmake_minimum_required (VERSION 2.8)

SET(APP_NAME DooVR)

PROJECT(${APP_NAME})

ADD_SUBDIRECTORY (doovr)

#set up searchpath
set( LIBRARY_SEARCH_PATH
	 ${PROJECT_SOURCE_DIR}/lib )

set( INCLUDE_SEARCH_PATH
	 ${PROJECT_SOURCE_DIR}/include )

# GLFW
find_path( GLFW_INCLUDE_DIR 
		   glfw3.h glfw3native.h
		   ${INCLUDE_SEARCH_PATH}/GLFW )

find_library( GLFW_LIBRARY 
			  glfw3 glfw3dll 
			  ${LIBRARY_SEARCH_PATH} )

#find wand3D
find_path( W3D_INCLUDE_DIR 
		   wand3d.h wand3d_exception.h wand3d_observer.h wand3d_utils.h 
		   ${INCLUDE_SEARCH_PATH}/wand3d )

find_library( W3D_LIBRARY 
			  wand3d wand3d_utils 
			  ${LIBRARY_SEARCH_PATH} )

#find libOVR
find_path( LIBOVR_INCLUDE_DIR 
		   wand3d.h wand3d_exception.h wand3d_observer.h wand3d_utils.h 
		   ${INCLUDE_SEARCH_PATH}/wand3d )

find_library( LIBOVR_LIBRARY 
			   libovr64 libovr64d
			  ${LIBRARY_SEARCH_PATH} )


find_package(OpenGL REQUIRED)

find_path( GLEW_INCLUDE_DIR 
		   ${PROJECT_SOURCE_DIR}/include/GL
		   REQUIRED)

find_library( GLEW_LIBRARY
			  GLEW glew32 glew glew32s 
			  ${LIBRARY_SEARCH_PATH} )

SET( LIBS
		${OPENGL_LIBRARY}
		${GLEW_LIBRARY}
		${GLFW_LIBRARY}
		${LIBOVR_LIBRARY}
		${W3D_LIBRARY} )

SET( INCLUDE_PATH
	 ${OPENGL_INCLUDE_DIR}
	 ${GLEW_INCLUDE_DIR}
	 ${GLFW_INCLUDE_DIR}
	 ${LIBOVR_INCLUDE_DIR}
	 ${W3D_INCLUDE_DIR} )

include_directories(INCLUDE_PATH)

# Libraries to be linked
target_link_libraries (${APP_NAME} ${LIBS})