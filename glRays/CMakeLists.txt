# CMakeList.txt : CMake project for glRays, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (glRays
	glRays.cpp 
	"shader.cpp" "shader.h" "camera.h" "vertex.glsl" "fragment.glsl" "compute.glsl" "scene.h" "options.h")

# Copy over shader files so program can read & compile them
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/compute.glsl
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_CURRENT_SOURCE_DIR}/compute.glsl
	${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/compute.glsl)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/fragment.glsl
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_CURRENT_SOURCE_DIR}/fragment.glsl
	${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/fragment.glsl)

add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/vertex.glsl
	COMMAND ${CMAKE_COMMAND} -E copy
	${CMAKE_CURRENT_SOURCE_DIR}/vertex.glsl
	${CMAKE_CURRENT_BINARY_DIR}
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/vertex.glsl)

add_custom_target(copy_compute_shader DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/compute.glsl)
add_custom_target(copy_fragment_shader DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/fragment.glsl)
add_custom_target(copy_vertex_shader DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/vertex.glsl)

add_dependencies(glRays copy_compute_shader)
add_dependencies(glRays copy_fragment_shader)
add_dependencies(glRays copy_vertex_shader)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET glRays PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
