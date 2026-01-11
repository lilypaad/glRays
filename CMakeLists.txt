# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 4.1)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project (glRays)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Compile imgui
set(IMGUI_PATH ${CMAKE_CURRENT_LIST_DIR}/lib/imgui-docking)
file(GLOB IMGUI_SOURCES 
	${IMGUI_PATH}/*.cpp 
	${IMGUI_PATH}/backends/imgui_impl_glfw.cpp 
	${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC 
	${IMGUI_PATH} 
	${IMGUI_PATH}/backends
	${CMAKE_CURRENT_LIST_DIR}/lib/glfw-3.4/include)

add_subdirectory(lib/glad)
add_subdirectory(lib/glfw-3.4)
add_subdirectory(lib/glm-master)
add_subdirectory(glRays)

target_link_libraries(glRays glad glfw glm::glm imgui)