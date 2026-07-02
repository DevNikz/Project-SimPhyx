#include "ParticleContact.h"
using namespace std;
using namespace Physics;

void Physics::ParticleContact::Resolve(float time)
{
	ResolveVelocity(time);
	ResolveInterpenetration(time);
}

float Physics::ParticleContact::GetSeparatingSpeed()
{
	if (particles[1] != nullptr) {
		glm::vec3 velocity = particles[0]->Velocity;
		if (particles[1]) velocity -= particles[1]->Velocity;
		return glm::dot(velocity, contactNormal);
	}
	else {
		glm::vec3 velocity = particles[0]->Velocity;
		return glm::dot(velocity, contactNormal);
	}
}

void Physics::ParticleContact::ResolveVelocity(float time)
{
	if (particles[1] != nullptr) {
		float separatingSpeed = GetSeparatingSpeed();
		if (separatingSpeed > 0) return;

		float newSS = -restitution * separatingSpeed;
		float deltaSpeed = newSS - separatingSpeed;

		float totalMass = 1.f / particles[0]->mass;
		if (particles[1]) totalMass += 1.f / particles[1]->mass;

		if (totalMass <= 0) return;

		float impulse_mag = deltaSpeed / totalMass;
		glm::vec3 Impulse = contactNormal * impulse_mag;

		glm::vec3 V_a = Impulse * (1.f / particles[0]->mass);
		particles[0]->Velocity = particles[0]->Velocity + V_a;

		if (particles[1]) {
			glm::vec3 V_b = Impulse * (-1.f / particles[1]->mass);
			particles[1]->Velocity = particles[1]->Velocity + V_b;
		}
	}
	else {
		float separatingSpeed = GetSeparatingSpeed();
		if (separatingSpeed > 0) return;

		float newSS = -restitution * separatingSpeed;
		float deltaSpeed = newSS - separatingSpeed;

		float totalMass = 1.f / particles[0]->mass;

		if (totalMass <= 0) return;

		float impulse_mag = deltaSpeed / totalMass;
		glm::vec3 Impulse = contactNormal * impulse_mag;

		glm::vec3 V_a = Impulse * (1.f / particles[0]->mass);
		particles[0]->Velocity = particles[0]->Velocity + V_a;
	}
}

void ParticleContact::ResolveInterpenetration(float time) {\
	if (particles[1] != nullptr) {
		if (depth <= 0) return;

		float totalMass = (float)1 / particles[0]->mass;
		if (particles[1]) totalMass += 1.f / particles[1]->mass;

		if (totalMass <= 0) return;

		float totalMoveByMass = depth / totalMass;

		glm::vec3 moveByMass = contactNormal * totalMoveByMass;

		glm::vec3 P_a = moveByMass * (1.f / particles[0]->mass);
		particles[0]->Position += P_a;

		if (particles[1]) {
			glm::vec3 P_b = moveByMass * (-1.f / particles[1]->mass);
			particles[1]->Position += P_b;
		}

		depth = 0;
	}
	else {
		if (depth <= 0) return;

		float totalMass = 1.f / particles[0]->mass;

		if (totalMass <= 0) return;

		float totalMoveByMass = depth / totalMass;

		glm::vec3 moveByMass = contactNormal * totalMoveByMass;

		glm::vec3 P_a = moveByMass * (1.f / particles[0]->mass);
		particles[0]->Position += P_a;

		depth = 0;
	}
		
}