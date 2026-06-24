#pragma once
#include "../Physics/Particle.h"
#include <list>

namespace Physics {
	class ParticleContact {
		public:
			float depth;
			Particle* particles[2];

			float restitution;
			glm::vec3 contactNormal;

			void Resolve(float time);
			float GetSeparatingSpeed();
		protected:
			
			void ResolveVelocity(float time);
			void ResolveInterpenetration(float time);
	};
}