cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

# Set DIR
set(MAIN_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/main.h
	${CMAKE_CURRENT_SOURCE_DIR}/Camera.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Camera.h
	${CMAKE_CURRENT_SOURCE_DIR}/Model.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Model.h
	${CMAKE_CURRENT_SOURCE_DIR}/Shader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Shader.h
	${CMAKE_CURRENT_SOURCE_DIR}/Quad.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Quad.h
	${CMAKE_CURRENT_SOURCE_DIR}/Line.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Line.h
	${CMAKE_CURRENT_SOURCE_DIR}/stb_image.h
	${CMAKE_CURRENT_SOURCE_DIR}/tiny_obj_loader.h
	${CMAKE_CURRENT_SOURCE_DIR}/Loader.h)

# MAIN SOURCE
set(BASE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(BASE_SOURCES 
	${BASE_DIR}/DragForceGenerator.cpp
	${BASE_DIR}/DragForceGenerator.h
	${BASE_DIR}/ForceGenerator.h
	${BASE_DIR}/ForceRegistry.cpp
	${BASE_DIR}/ForceRegistry.h
	${BASE_DIR}/GravityForceGenerator.cpp
	${BASE_DIR}/GravityForceGenerator.h
	${BASE_DIR}/RenderParticle.cpp
	${BASE_DIR}/RenderParticle.h)

# PHYSICS
set(PHYS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Physics")
set(PHYS_SOURCES 
	${PHYS_DIR}/Particle.cpp
	${PHYS_DIR}/Particle.h
	${PHYS_DIR}/PhysicsWorld.cpp
	${PHYS_DIR}/PhysicsWorld.h)

# SPRINGS
set(SPRINGS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Spring")
set(SPRINGS_SOURCES 
	${SPRINGS_DIR}/AnchoredSpring.cpp
	${SPRINGS_DIR}/AnchoredSpring.h
	${SPRINGS_DIR}/BungeeSpring.cpp
	${SPRINGS_DIR}/BungeeSpring.h
	${SPRINGS_DIR}/ParticleSpring.cpp
	${SPRINGS_DIR}/ParticleSpring.h)

# CONTACT
set(CONTACTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Contact")
set(CONTACTS_SOURCES 
	${CONTACTS_DIR}/ContactResolver.cpp
	${CONTACTS_DIR}/ContactResolver.h
	${CONTACTS_DIR}/ParticleContact.cpp
	${CONTACTS_DIR}/ParticleContact.h)

# LINK
set(LINK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Link")
set(LINK_SOURCES 
	${LINK_DIR}/Chain.cpp
	${LINK_DIR}/Chain.h
	${LINK_DIR}/ParticleLink.cpp
	${LINK_DIR}/ParticleLink.h
	${LINK_DIR}/Rod.cpp
	${LINK_DIR}/Rod.h)

# MISC
set(MISC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Misc")
set(MISC_SOURCES 
	${MISC_DIR}/PhysicsThreadManager.h
	${MISC_DIR}/ThreadScheduler.h)

# CABLE
set(CABLE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Cable")
set(CABLE_SOURCES 
	${CABLE_DIR}/Cable.h
	${CABLE_DIR}/Cable.cpp)

#imgui
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/imgui")
set(IMGUI_SOURCES 
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_demo.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp)

set(IMGUI_BACKEND_SOURCES 
	${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
	${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp)

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
add_executable (Project-SimPhyx ${MAIN_SRC} ${GLAD_SRC} ${BASE_SOURCES} ${PHYS_SOURCES}
				 ${SPRINGS_SOURCES} ${LINK_SOURCES} ${CONTACTS_SOURCES} ${MISC_SOURCES}
				 ${CABLE_SOURCES} ${IMGUI_SOURCES} ${IMGUI_BACKEND_SOURCES})

target_include_directories(Project-SimPhyx PRIVATE
    ${glfw_SOURCE_DIR}/include
    ${glm_SOURCE_DIR}
	${GLAD_INCLUDE_DIR}
	${IMGUI_DIR}
	${IMGUI_DIR}/backends
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