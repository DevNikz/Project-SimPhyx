#pragma once
#include "../Physics/Particle.h"
#include "../Contact/ParticleContact.h"
#include <vector>

namespace Physics {
	class ParticleLink 
	{
	public:
		std::vector<Particle*> particles;
		//Particle* particles[2];
		virtual ParticleContact* GetContact() { return nullptr; };

	protected:
		float CurrentLength();
	};
}