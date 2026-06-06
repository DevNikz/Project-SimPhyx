#include "DragForceGenerator.h"
using namespace Physics;

void DragForceGenerator::UpdateForce(Particle* particle, float time)
{
	glm::vec3 force = glm::vec3(0.f);
	glm::vec3 currV = particle->Velocity;

	float mag = glm::length(currV);
	if (mag <= 0) return;

	float dragF = (k1 * mag) + (k2 * mag);
	glm::vec3 dir = glm::normalize(currV);

	particle->ApplyForce(dir * -dragF);
}