set(PROJECT nugem)
project(${PROJECT})
cmake_minimum_required(VERSION 2.8)


# Add global definitions
add_definitions("-Wall")
set(SOURCEDIR src)
set(SOURCE_FILES
	${SOURCEDIR}/mugen/mugenutils.cpp
	${SOURCEDIR}/mugen/state.cpp
	${SOURCEDIR}/mugen/cmd.cpp
	${SOURCEDIR}/spritehandler.cpp
	${SOURCEDIR}/mugen/sffv1.cpp
	${SOURCEDIR}/mugen/sffv2.cpp
	${SOURCEDIR}/input.cpp
    ${SOURCEDIR}/character.cpp
    ${SOURCEDIR}/player.cpp
    ${SOURCEDIR}/scenefight.cpp
    ${SOURCEDIR}/scenemenu.cpp
    ${SOURCEDIR}/scene.cpp
    ${SOURCEDIR}/game.cpp
    ${SOURCEDIR}/main.cpp
)

add_executable(${PROJECT} ${SOURCE_FILES})
# Using C++11
set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD_REQUIRED 11)

# Find SDL2
INCLUDE(FindPkgConfig REQUIRED)
PKG_CHECK_MODULES(SDL2 REQUIRED sdl2)
include_directories(${SDL2_INCLUDE_DIRS})

target_link_libraries(${PROJECT} ${SDL2_LIBRARIES})
install(TARGETS ${PROJECT} DESTINATION bin)
