cmake_minimum_required(VERSION 3.10)

include_directories(${PROJECT_SOURCE_DIR}/simplesock)
include_directories(simplesock/)
configure_file(./assets/Carlito-Regular.ttf . COPYONLY)

project(a6)

add_executable(a6 main.cpp ${PROJECT_SOURCE_DIR}/simplesock/socklib_generic.cpp)

target_compile_features(a6 PRIVATE cxx_std_17)

# target_compile_options(a6 PRIVATE -fsanitize=address)
# target_link_options(a6 PRIVATE -fsanitize=address)

set(CMAKE_BUILD_TYPE Debug)

if (UNIX)
	target_sources(a6 PRIVATE ${PROJECT_SOURCE_DIR}/simplesock/socklib_posix.cpp)
	find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
	find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
	# find_package(SDL2_ttf REQUIRED CONFIG REQUIRED COMPONENTS SDL2_ttf)
	add_subdirectory(SDL_ttf EXCLUDE_FROM_ALL)
endif (UNIX)

if (WIN32)
	target_sources(a6 PRIVATE ${PROJECT_SOURCE_DIR}/simplesock/socklib_win32.cpp)
	set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
	add_subdirectory(SDL EXCLUDE_FROM_ALL)
	add_subdirectory(SDL_ttf EXCLUDE_FROM_ALL)
endif (WIN32)

target_sources(a6 PRIVATE ${PROJECT_SOURCE_DIR}/simplesock/pool.cpp)
target_link_libraries(a6 PRIVATE SDL2_ttf::SDL2_ttf)
target_link_libraries(a6 PRIVATE SDL2::SDL2main)
target_link_libraries(a6 PRIVATE SDL2::SDL2)
