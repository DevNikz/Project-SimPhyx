#pragma once
#include <list>
#include "Particle.h"
#include "../ForceRegistry.h"
#include "../GravityForceGenerator.h"

namespace Physics {
	class PhysicsWorld
	{
	public:
		ForceRegistry forceRegistry;
		std::list<Particle*> Particles;

		void AddParticle(Particle* p);
		void Update(float time);

	private:
		void UpdateParticleList();
		GravityForceGenerator Gravity = GravityForceGenerator(glm::vec3(0.f, -9.8f, 0.f));
	};
}