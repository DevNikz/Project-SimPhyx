#pragma once
#include "../ForceGenerator.h"
#include "../Physics/Particle.h"
#include "../Contact/ParticleContact.h"

namespace Physics {
	class Cable 
	{
		private:
			glm::vec3 anchorPoint;
			Particle* particle;

		public:
			float cableLength = 1.f;
			float restitution = 0.f;

			Cable(Particle* _p, glm::vec3 _pos, float _len, float _r = 0.f) :
				particle(_p), anchorPoint(_pos), cableLength(_len), restitution(_r) {
			};

			ParticleContact* GetContact();
			float CurrentLength();
	};
}
