cmake_minimum_required(VERSION 2.6)

project(Perabird)

file(
	GLOB_RECURSE
	source_files
	src/*
)

file(
	GLOB_RECURSE
	data_files
	data/*
)

Find_Package(SDL REQUIRED)
Find_Package(SDL_image REQUIRED)
Find_Package(SDL_mixer REQUIRED)
Find_Package(SDL_net REQUIRED)
Find_Package(OpenSSL REQUIRED)
# OpenSSL is only used for hashing...
# I may copy their algorythm directly here maybe, would remove a dependency
Find_Package(OpenGL REQUIRED)

link_libraries (
   ${SDL_LIBRARY}
   ${SDLIMAGE_LIBRARY}
   ${SDLMIXER_LIBRARY}
   ${SDLNET_LIBRARY}
   ${OPENSSL_LIBRARIES}
   ${OPENGL_LIBRARY}
)

add_executable(
	perabird
	${source_files}
)

add_definitions(-Wall -Wextra -pedantic -g)

