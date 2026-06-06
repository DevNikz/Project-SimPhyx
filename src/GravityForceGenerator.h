#pragma once
#include "Physics/Particle.h"
#include "ForceGenerator.h"

namespace Physics {
	class GravityForceGenerator : public ForceGenerator {
	private:
		glm::vec3 Gravity = glm::vec3(0.f, -9.8f, 0.f);

	public:
		GravityForceGenerator(const glm::vec3 gravity) : Gravity(gravity) {};
		void UpdateForce(Particle* p, float time) override;
	};
}