#pragma once
#include "Physics/Particle.h"
#include "ForceGenerator.h"
#include <list>

namespace Physics {
	class ForceRegistry {
	protected:

		struct ParticleForceRegistry
		{
			Particle* particle;
			ForceGenerator* generator;
		};

		std::list<ParticleForceRegistry> Registry;

	public:
		void Add(Particle* particle, ForceGenerator* generator);
		void Remove(Particle* particle, ForceGenerator* generator);
		void Clear();
		void UpdateForces(float time);


	};
}