#pragma once
#include <list>
#include "Particle.h"
#include "../ForceRegistry.h"
#include "../GravityForceGenerator.h"
#include "../Link/ParticleLink.h"
#include "../Link/Chain.h"
#include "../Contact/ParticleContact.h"
#include "../Contact/ContactResolver.h"
#include "../Cable/Cable.h"

namespace Physics {
	class PhysicsWorld
	{
		public:
			ForceRegistry forceRegistry;
			std::list<Particle*> Particles;
			std::list<ParticleLink*> Links;
			std::vector<ParticleContact*> Contacts;
			std::list<Chain*> Chains;
			std::list<Cable*> Cables;

			void AddParticle(Particle* p);
			void AddContact(Particle* p1, Particle* p2, float restitution, glm::vec3 contactNormal, float depth);
			void Update(float time);
			void ModifyGravity(float force);

		private:
			void UpdateParticleList();
			void GenerateContacts();
			void GetOverlaps();
			GravityForceGenerator EnableGravity = GravityForceGenerator(glm::vec3(0.f, -9.8f, 0.f));
			GravityForceGenerator DisableGravity = GravityForceGenerator(glm::vec3(0.f, 0.f, 0.f));
			ContactResolver contactResolver = ContactResolver(20);
	};
}