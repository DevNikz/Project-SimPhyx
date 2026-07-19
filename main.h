#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <list>
#include <iomanip>
#include "Camera.h"
#include "src/Physics/Particle.h"
#include "src/Physics/PhysicsWorld.h"
#include "Shader.h"
#include "Model.h"
#include "src/RenderParticle.h"
#include "src/DragForceGenerator.h"
#include "src/Misc/PhysicsThreadManager.h"

#include "src/Misc/ThreadScheduler.h"
#include "src/Cable/Cable.h"
#include "thirdparty/imgui/imgui.h"
#include "thirdparty/imgui/backends/imgui_impl_opengl3.h"
#include "Quad.h"
#include "Line.h"
#include "src/Input/PlayerController.h"
#include "src/Input/PlayerInput.h"

using namespace std;
using namespace Physics;

/*
std::vector<FountainParticle> fountainParticles;
    std::uniform_real_distribution<float> colorGen(0.1f, 1.f);
    std::uniform_real_distribution<float> massGen(2.5f, 5.f);
    std::uniform_real_distribution<float> dampGen(0.3f, 0.9f);
    std::uniform_real_distribution<float> forceXGen(-50000.f, 50000.f);
    std::uniform_real_distribution<float> forceYGen(100000.f, 200000.f);
    std::uniform_real_distribution<float> lifespanGen(1.f, 10.f);
    std::uniform_real_distribution<float> scaleGen(2.f, 10.f);
*/

/*
* Force Gen
* Particle Links
* Rotations
* Collisions
*/