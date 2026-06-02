#pragma once
#include "Particle.h"
#include <list>
namespace Physics {
	class PhysicsWorld {
	public:
		std::list<Particle*> PhysicsParticles;
		
		void AddParticle(Particle* p);
		void Update(float time);

	private:
		void UpdateParticleList();
	};
}