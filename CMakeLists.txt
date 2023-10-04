cmake_minimum_required(VERSION 3.7)
set(PROJECT nugem)
project(${PROJECT})


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
	${SOURCEDIR}/mugen/stage.cpp
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
# https://cmake.org/cmake/help/latest/module/FindPkgConfig.html
INCLUDE(FindPkgConfig REQUIRED)
PKG_CHECK_MODULES(SDL2 REQUIRED sdl2)
PKG_CHECK_MODULES(SDL2_IMAGE REQUIRED sdl2_image)
PKG_CHECK_MODULES(GL REQUIRED gl)
PKG_CHECK_MODULES(GLEW REQUIRED glew)

message("SDL2_INCLUDE_DIRS: ${SDL2_INCLUDE_DIRS}
SDL2_IMAGE_INCLUDE_DIRS: ${SDL2_IMAGE_INCLUDE_DIRS}
GL_INCLUDE_DIRS:${GL_INCLUDE_DIRS}
GLEW_INCLUDE_DIRS: ${GLEW_INCLUDE_DIRS}")
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS} ${GL_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS})

message("SDL2_LINK_LIBRARIES: ${SDL2_LINK_LIBRARIES}
SDL2_IMAGE_LINK_LIBRARIES: ${SDL2_IMAGE_LINK_LIBRARIES}
GL_LINK_LIBRARIES:${GL_LINK_LIBRARIES}
GLEW_LINK_LIBRARIES: ${GLEW_LINK_LIBRARIES}")
target_link_libraries(${PROJECT} ${SDL2_LINK_LIBRARIES} ${SDL2_IMAGE_LINK_LIBRARIES} ${GL_LINK_LIBRARIES} ${GLEW_LINK_LIBRARIES})
install(TARGETS ${PROJECT} DESTINATION bin)
