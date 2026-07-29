#include "Rod.h"
using namespace std;
using namespace Physics;

ParticleContact* Rod::GetContact() {
	if (particles.size() < 2 || !particles[0]->isActive || !particles[1]->isActive) {
		return nullptr;
	}

	float currLen = CurrentLength();
	if (currLen == length) {
		return nullptr;
	}

	ParticleContact* ret = new ParticleContact();
	ret->particles[0] = particles[0];
	ret->particles[1] = particles[1];
	glm::vec3 dir = particles[1]->Position - particles[0]->Position;
	dir = glm::normalize(dir);

	if (currLen > length) {
		ret->contactNormal = dir;
		ret->depth = currLen - length;
	}

	else {
		ret->contactNormal = dir * -1.f;
		ret->depth = length - currLen;
	}

	ret->restitution = restitution;
	return ret;
}
