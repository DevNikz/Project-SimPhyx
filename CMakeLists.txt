cmake_minimum_required(VERSION 3.15)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

#Create a project with this name
project ("Project-SimPhyx")

include(FetchContent)

# Set directory of third party libraries
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/thirdparty CACHE PATH "Base Directory of libs" FORCE)
set(GLAD_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glad/include")
set(GLAD_SRC "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/glad/src/glad.c")

#Get GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw
  GIT_TAG        3.4
  GIT_SUBMODULES "" 
)

# Disable building GLFW's extra tools to save time
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

#Set GLFW for this project
FetchContent_MakeAvailable(glfw)

#Get GLM
FetchContent_Declare(
	glm
	GIT_REPOSITORY	https://github.com/g-truc/glm.git
	GIT_TAG 1.0.1
	GIT_SUBMODULES ""
)

FetchContent_MakeAvailable(glm)

#OpenGL should be preinstalled
find_package(OpenGL REQUIRED)

# Add source to this project's executable.
# Change these to your files
add_executable (Project-SimPhyx "main.cpp" "main.h" "tiny_obj_loader.h" "stb_image.h" ${GLAD_SRC} "src/Physics/Particle.h" "src/Physics/Particle.cpp" "Shader.h" "Camera.h" "Camera.cpp" "Shader.cpp" "Model.h" "Model.cpp" "src/RenderParticle.h" "src/RenderParticle.cpp"  "src/Physics/PhysicsWorld.h" "src/Physics/PhysicsWorld.cpp")

target_include_directories(Project-SimPhyx PRIVATE
    ${glfw_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
	${GLAD_INCLUDE_DIR}
)

#Link the required libraries
target_link_libraries(Project-SimPhyx PRIVATE 
						glfw
						OpenGL::GL
						opengl32
						glm::glm
)

add_custom_command(
	TARGET Project-SimPhyx
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	"${CMAKE_CURRENT_SOURCE_DIR}/Shaders"
		"$<TARGET_FILE_DIR:Project-SimPhyx>/Shaders"
	COMMENT "Copying assets to build dir"
)

add_custom_command(
	TARGET Project-SimPhyx
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/3D"
		"$<TARGET_FILE_DIR:Project-SimPhyx>/3D"
	COMMENT "Copying assets to build dir"
)

add_custom_command(
	TARGET Project-SimPhyx
	POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
		"${CMAKE_CURRENT_SOURCE_DIR}/Skybox"
		"$<TARGET_FILE_DIR:Project-SimPhyx>/Skybox"
	COMMENT "Copying assets to build dir"
)