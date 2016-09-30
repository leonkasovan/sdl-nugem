set(PROJECT nugem)
project(${PROJECT})
cmake_minimum_required(VERSION 2.8)


# Add global definitions
add_definitions("-Wall")
set(SOURCEDIR src)
set(SOURCE_FILES
	${SOURCEDIR}/mugen/mugenutils.cpp
	${SOURCEDIR}/mugen/air.cpp
	${SOURCEDIR}/mugen/def.cpp
	${SOURCEDIR}/mugen/state.cpp
	${SOURCEDIR}/mugen/cmd.cpp
	${SOURCEDIR}/mugen/sprites.cpp
	${SOURCEDIR}/mugen/sffv1.cpp
	${SOURCEDIR}/mugen/sffv2.cpp
	${SOURCEDIR}/script/script.cpp
	${SOURCEDIR}/input.cpp
	${SOURCEDIR}/window.cpp
	${SOURCEDIR}/glgraphics.cpp
	${SOURCEDIR}/glsprite.cpp
	${SOURCEDIR}/character.cpp
    ${SOURCEDIR}/player.cpp
    ${SOURCEDIR}/fight/fight.cpp
    ${SOURCEDIR}/fight/fightcharacter.cpp
    ${SOURCEDIR}/scenemenu.cpp
    ${SOURCEDIR}/sceneloader.cpp
	${SOURCEDIR}/eventhandler.cpp
    ${SOURCEDIR}/game.cpp
    ${SOURCEDIR}/main.cpp
)

add_executable(${PROJECT} ${SOURCE_FILES})
# Using C++14
set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT} PROPERTY CXX_STANDARD_REQUIRED 14)

# Find SDL2
INCLUDE(FindPkgConfig REQUIRED)
PKG_CHECK_MODULES(SDL2 REQUIRED sdl2)
PKG_CHECK_MODULES(GL REQUIRED gl)
PKG_CHECK_MODULES(GLEW REQUIRED glew)
include_directories(${SDL2_INCLUDE_DIRS} ${GL_INCLUDE_DIRS})

target_link_libraries(${PROJECT} SDL2_image ${SDL2_LIBRARIES} ${GL_LIBRARIES} pthread ${GLEW_LIBRARIES})
install(TARGETS ${PROJECT} DESTINATION bin)
