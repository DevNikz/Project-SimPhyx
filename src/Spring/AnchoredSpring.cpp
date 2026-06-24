#include "AnchoredSpring.h"
using namespace std;
using namespace Physics;

void AnchoredSpring::UpdateForce(Particle* particle, float time)
{
	glm::vec3 pos = particle->Position;
	glm::vec3 force = pos - anchorPoint;
	float mag = glm::length(force);
	float springForce = -springConstant * glm::abs(mag - restLength);
	force = glm::normalize(force);
	force *= springForce;
	particle->ApplyForce(force);
}