#include "GravityForceGenerator.h"

namespace Physics {
	void GravityForceGenerator::UpdateForce(Particle* p, float time)
	{
		if (p->mass <= 0) return;
		glm::vec3 force = Gravity * p->mass;
		p->ApplyForce(force);
	}
}