#pragma once
#include "Physics/Particle.h"

namespace Physics {
	class ForceGenerator {
	public:
		virtual void UpdateForce(Particle* p, float time) {
			p->ApplyForce(glm::vec3(0.f));
		}
	};
}