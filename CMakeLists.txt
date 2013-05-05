cmake_minimum_required(VERSION 2.6)

project(Perabird)

SET(CMAKE_MODULE_PATH "${Perabird_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(AGuiSy REQUIRED)

link_libraries(
	${OPENGL_LIBRARY}
	${GLEW_LIBRARY}
	${SDL_LIBRARY}
	${SDL_IMAGE_LIBRARY}
	${AGUISY_LIBRARY}
)

if(WIN32)
	link_libraries(ws2_32.lib)
endif()

file(
	GLOB_RECURSE
	source_files
	src/*.cpp
	src/*.h
)

add_executable(
	perabird
	${source_files}
)

