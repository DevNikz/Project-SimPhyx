#include "BungeeSpring.h"
using namespace std;
using namespace Physics;

void BungeeSpring::UpdateForce(Particle* particle, float time) {
	glm::vec3 pos = particle->Position;
	glm::vec3 force = pos - anchorPoint;
	float mag = glm::length(force);
	//float mag = glm::clamp(mag, 0.f, 0.f);

	if (mag <= restLength) return; //the cord doesn't exert force

	float springForce = -springConstant * (mag - restLength); // only pulls
	force = glm::normalize(force);
	force *= springForce;
	particle->ApplyForce(force);
}