#pragma once
#include "../Physics/Particle.h"
#include "../Contact/ParticleContact.h"
#include <vector>
#include <glm/glm.hpp>

namespace Physics {
	class ParticleLink 
	{
	public:
		std::vector<Particle*> particles;
		std::vector<glm::vec3> positions;
		//Particle* particles[2];
		virtual ParticleContact* GetContact() { return nullptr; };

	protected:
		float CurrentLength();
	};
}