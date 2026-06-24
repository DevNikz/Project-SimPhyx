#pragma once
#include "../Physics/Particle.h"
#include "../Contact/ParticleContact.h"
#include "ParticleLink.h"
#include <vector>

namespace Physics {
	class Chain {
	public:
		std::vector<Particle*> particles;
		float length = 1;
		float restitution = 0;
		std::vector<ParticleContact*> GetContacts() const;
		void AddParticle(Particle* p);
		int SegmentCount() const;

	private:
		float SegmentLength(int i) const;
	};
}