#pragma once
#include "../Physics/Particle.h"
#include "../Contact/ParticleContact.h"
#include "ParticleLink.h"

namespace Physics {
	class Rod : public ParticleLink
	{
	public:
		float length = 1;
		float restitution = 0;
		ParticleContact* GetContact() override;
	};
}