project(negum)
cmake_minimum_required(VERSION 2.8)

# Point to our own cmake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules)

# Find SDL2
find_package(SDL2 REQUIRED)

# Add global definitions
add_definitions("-Wall")
include_directories(${SDL2_INCLUDE_DIR})


set(SOURCE_FILES
	mugen/sffv1.cpp
	mugen/sffv2.cpp
    character.cpp
    scenefight.cpp
    scenemenu.cpp
    scene.cpp
    game.cpp
    main.cpp
)

add_executable(negum ${SOURCE_FILES})
# Using C++11
set_property(TARGET negum PROPERTY CXX_STANDARD 11)
set_property(TARGET negum PROPERTY CXX_STANDARD_REQUIRED 11)
install(TARGETS negum DESTINATION bin)
target_link_libraries(negum ${SDL2_LIBRARIES})