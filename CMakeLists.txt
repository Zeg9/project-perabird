cmake_minimum_required(VERSION 2.6)

project(Perabird)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)

link_libraries(
	${OPENGL_LIBRARY}
	${GLEW_LIBRARY}
	${SDL_LIBRARY}
	${SDL_IMAGE_LIBRARY}
)

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

