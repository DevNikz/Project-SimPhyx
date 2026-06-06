#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
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

using namespace std;
using namespace Physics;