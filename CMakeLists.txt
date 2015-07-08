# project name is not mandatory but should be used
#------------------------------------------------------------------

SET(APP_NAME DooVR)
project(${APP_NAME})

# states the minimum version required
#------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8)

# include the directory itself as a path to include directories
# a directory is a file system cataloging structure which
# contains references to other computer files
#------------------------------------------------------------------
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# set the output path to /bin
#------------------------------------------------------------------
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#set up searchpath
SET( LIBRARY_SEARCH_PATH
	 ${PROJECT_SOURCE_DIR}/lib )

SET( INCLUDE_SEARCH_PATH
	 ${PROJECT_SOURCE_DIR}/include )

# create a variable called .._SOURCES containing all .cpp files
#------------------------------------------------------------------
#set (DooVR_SOURCES
#	doovr/*.cpp
#	doovr/*.h
#	doovr/*.glsl
#)

file(GLOB DooVR_SOURCES doovr/*.cpp doovr/*.h doovr/*.glsl)

# create an executable file from sources, create it first,
# then link the libraries
#------------------------------------------------------------------
add_executable(${APP_NAME} ${DooVR_SOURCES})

# for a large number of source files it can be created using
# file(GLOB DooVR_SOURCES *.cpp)

# find packages that is required
# pkg-config is a helper tool that helps you inser the correct
# compiler options
#------------------------------------------------------------------
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

# find GLFW
FIND_PATH( GLFW_INCLUDE_DIR
			glfw3.h glfw3native.h
			${INCLUDE_SEARCH_PATH}/GLFW)

FIND_LIBRARY( GLFW_LIBRARY 
			  glfw3
			  ${LIBRARY_SEARCH_PATH} )
			  
FIND_LIBRARY( GLFW_DLL_LIBRARY
			  glfw3dll
			  ${LIBRARY_SEARCH_PATH} )
			  
SET( GLFW_LIBRARIES ${GLFW_LIBRARY} ${GLFW_DLL_LIBRARY}) 
			
#find wand3D
FIND_PATH( W3D_INCLUDE_DIR 
		   wand3d.h wand3d_exception.h wand3d_observer.h wand3d_utils.h 
		   ${INCLUDE_SEARCH_PATH}/wand3d )

FIND_PATH( W3D_DIR
		   ${INCLUDE_SEARCH_PATH} )
		   
FIND_LIBRARY( W3D_LIBRARY 
			  wand3d
			  ${LIBRARY_SEARCH_PATH} )
			  
FIND_LIBRARY( W3D_UTILS_LIBRARY
			  wand3d_utils
			  ${LIBRARY_SEARCH_PATH} )

SET (WAND3D_LIBRARIES ${W3D_LIBRARY} ${W3D_UTILS_LIBRARY})
SET (WAND3D_INCLUDE_DIRS ${W3D_INCLUDE_DIR} ${W3D_DIR})	
	
#find libOVR
find_path( LIBOVR_INCLUDE_DIR 
		    OVR.h 
		   ${INCLUDE_SEARCH_PATH}/OVR )

find_path( LIBOVR_SRC_DIR 
		   OVR_CAPI.h OVR_CAPI_GL.h
		   ${INCLUDE_SEARCH_PATH}/Src )
		   
FIND_LIBRARY( LIBOVR_LIBRARY 
			   libovr64 REQUIRED
			  ${LIBRARY_SEARCH_PATH} )

FIND_LIBRARY( LIBOVR_DEBUG_LIBRARY 
			  libovr64d REQUIRED
			  ${LIBRARY_SEARCH_PATH} )
			  
SET (LIBOVR_LIBRARIES ${LIBOVR_LIBRARY} ${LIBOVR_DEBUG_LIBRARY})	  
SET (LIBOVR_INCLUDE_DIRS ${LIBOVR_INCLUDE_DIR} ${LIBOVR_SRC_DIR})
	
# find Armadillo
FIND_LIBRARY( BLAS_WIN64_LIBRARY
			  blas_win64_MT REQUIRED
			  ${LIBRARY_SEARCH_PATH} )
			  
FIND_LIBRARY( LAPACK_WIN64_LIBRARY
			  lapack_win64_MT REQUIRED
			  ${LIBRARY_SEARCH_PATH} )
	
FIND_PATH( ARMADILLO_INCLUDE_DIRS
		   armadillo_bits/arma_version.hpp
		   $(INCLUDE_SEARCH_PATH)/armadillo )
		
SET (LIBS
		${OPENGL_LIBRARY}
		${GLEW_LIBRARIES} 
		${LIBOVR_LIBRARIES}
		${WAND3D_LIBRARIES}
		${CMAKE_THREAD_LIBS_INIT}
		${RT_LIBRARY}
		${GLFW_LIBRARIES}
		winmm.lib
		ws2_32.lib
		Dbghelp.lib
		${LAPACK_WIN64_LIBRARY}
		${BLAS_WIN64_LIBRARY}
)

SET( INCLUDE_PATHS
	 ${OPENGL_INCLUDE_DIR}
	 ${GLEW_INCLUDE_DIRS}
	 ${GLFW_INCLUDE_DIR}
	 ${LIBOVR_INCLUDE_DIRS}
	 ${WAND3D_INCLUDE_DIRS} 
	 ${ARMADILLO_INCLUDE_DIRS}
)

# get variable GLFW_INCLUDE_DIRS when searching module it contains
# paths to directories to the header files we want to include
#------------------------------------------------------------------
include_directories(${INCLUDE_PATHS})
include_directories(${INCLUDE_SEARCH_PATH})

# GLFW_STATIC_LIBRARIES is also retrived when running search module
# it contains all the external libraries that are needed
#------------------------------------------------------------------
target_link_libraries(${APP_NAME} ${LIBS} )