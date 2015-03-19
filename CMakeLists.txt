# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (restore_the_light)

# CMakeLists modified from: http://www.opengl-tutorial.org/download/

find_package(OpenGL REQUIRED)


if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory ! (and give it a clever name, like bin_Visual2012_64bits/)" )
endif()
if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()
if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause." )
endif()



# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-1c73e35")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)



include_directories(
	external/glfw-3.0.3/include/
	external/
	external/glew-1.9.0/include/
	external/gaussian-blur/
	restore-the-light/headers/
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	GLFW_303
	GLEW_190
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

add_executable(restore-the-light 
	restore-the-light/source/main.cpp

	restore-the-light/source/collision_manager.cpp
	restore-the-light/headers/collision_manager.h

	restore-the-light/source/fbo.cpp
	restore-the-light/headers/fbo.h

	restore-the-light/source/GLSL.cpp
	restore-the-light/headers/GLSL.h

	restore-the-light/source/image.cpp
	restore-the-light/headers/image.h

	restore-the-light/source/light.cpp
	restore-the-light/headers/light.h

	restore-the-light/source/load_manager.cpp
	restore-the-light/headers/load_manager.h

	restore-the-light/source/mesh.cpp
	restore-the-light/headers/mesh.h

	restore-the-light/source/object3d.cpp
	restore-the-light/headers/object3d.h

	restore-the-light/source/render.cpp
	restore-the-light/headers/render.h

	restore-the-light/source/shader.cpp
	restore-the-light/headers/shader.h

	restore-the-light/source/texture.cpp
	restore-the-light/headers/texture.h

	restore-the-light/source/tiny_obj_loader.cc
	restore-the-light/headers/tiny_obj_loader.h

	restore-the-light/source/virtual_camera.cpp
	restore-the-light/headers/virtual_camera.h

	restore-the-light/headers/enemy.h
	restore-the-light/headers/game_map.h
	restore-the-light/headers/GLIncludes.h
	restore-the-light/headers/globals.h
	restore-the-light/headers/hero.h
	restore-the-light/headers/light_friend.h
	restore-the-light/headers/light_shot.h
	restore-the-light/headers/material.h

	restore-the-light/shaders/blur-fragment.glsl
	restore-the-light/shaders/geometry-pass-map-fragment.glsl
	restore-the-light/shaders/geometry-pass-map-vertex.glsl
	restore-the-light/shaders/geometry-pass-map-texture-fragment.glsl
	restore-the-light/shaders/geometry-pass-map-texture-vertex.glsl
	restore-the-light/shaders/light-ambient-fragment.glsl
	restore-the-light/shaders/light-point-fragment.glsl
	restore-the-light/shaders/light-spot-fragment.glsl
	restore-the-light/shaders/light-directional-fragment.glsl
	restore-the-light/shaders/light-vertex.glsl
	restore-the-light/shaders/multiply-two-textures-fragment.glsl
	restore-the-light/shaders/sum-two-textures-fragment.glsl
	restore-the-light/shaders/texture-fragment.glsl
	restore-the-light/shaders/texture-vertex.glsl
)
target_link_libraries(restore-the-light
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(restore-the-light PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/restore-the-light/")
create_target_launcher(restore-the-light WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/restore-the-light/")
create_default_target_launcher(restore-the-light WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/restore-the-light/")



SOURCE_GROUP(Shaders "restore-the-light/shaders/.*glsl$" )


if (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )
#Copying new resources to the build folder
add_custom_command(
   TARGET restore-the-light POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/restore-the-light/shaders" "${CMAKE_CURRENT_BINARY_DIR}/shaders"
)
add_custom_command(
   TARGET restore-the-light POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/restore-the-light/objects" "${CMAKE_CURRENT_BINARY_DIR}/objects"
)
add_custom_command(
   TARGET restore-the-light POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/restore-the-light/textures" "${CMAKE_CURRENT_BINARY_DIR}/textures"
)

#Copying the compiled project to the project folder, out of the build directory
add_custom_command(
   TARGET restore-the-light POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/restore-the-light${CMAKE_EXECUTABLE_SUFFIX}" "${CMAKE_CURRENT_SOURCE_DIR}/restore-the-light/"
)
elseif (${CMAKE_GENERATOR} MATCHES "Xcode" )

endif (NOT ${CMAKE_GENERATOR} MATCHES "Xcode" )

