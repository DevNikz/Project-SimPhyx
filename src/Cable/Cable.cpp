/*
*/

#include "Cable.h"
using namespace std;
using namespace Physics;

float Cable::CurrentLength() {
	return glm::length(particle->Position - anchorPoint);
}

ParticleContact* Cable::GetContact() {
	float currLen = CurrentLength();
	if (currLen <= cableLength) return nullptr;

	ParticleContact* ret = new ParticleContact();
	ret->particles[0] = particle;
	ret->particles[1] = nullptr;

	glm::vec3 dir = particle->Position - anchorPoint;
	dir = glm::normalize(dir);

	ret->contactNormal = dir * -1.f;
	ret->depth = currLen - cableLength;
	ret->restitution = restitution;

	return ret;
}

/*
float Cable::CurrentLength(Particle* particle) {
	glm::vec3 ret = anchorPoint - particle->Position;
	return glm::length(ret);
}

void Cable::UpdateForce(Particle* particle, float time) {
	float currLen = CurrentLength(particle);
	if (currLen == cableLength) return;

	glm::vec3 pos = particle->Position;
	glm::vec3 dir = pos - anchorPoint;
	float mag = glm::length(dir);

	//If within cable length, no force
	if (mag <= cableLength) {
		cout << "within cable length" << endl;
		return;
	}

	glm::vec3 normDir = glm::normalize(dir);
	float ext = mag - cableLength;

	float springForce = -springConstant * ext;
	particle->ApplyForce(normDir * springForce);

	float vel = glm::dot(particle->Velocity, normDir);
	if (vel > 0.0f) particle->Velocity -= normDir * vel;

	
	/*
	float springForce = -springConstant * glm::abs(mag - cableLength);
	glm::vec3 force = glm::normalize(dir);
	force *= springForce;
	particle->ApplyForce(force);
}
*/

/*
dir /= mag;
	float extension = mag - cableLength;
	float spring = -springConstant * extension;
	float damp = -damping * glm::dot(particle->Velocity, dir);
	glm::vec3 force = dir * (spring + damp);
*/