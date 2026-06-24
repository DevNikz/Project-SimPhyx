#include "PhysicsWorld.h"
#include <iostream>
using namespace std;
using namespace Physics;

namespace Physics {
	void PhysicsWorld::AddParticle(Particle* p) {
		Particles.push_back(p);
		if (p->useGravity) forceRegistry.Add(p, &EnableGravity);
		else forceRegistry.Add(p, &DisableGravity);
	}

	void PhysicsWorld::AddContact(Particle* p1, Particle* p2, float restitution, glm::vec3 contactNormal, float depth)
	{
		ParticleContact* toAdd = new ParticleContact();

		toAdd->particles[0] = p1;
		toAdd->particles[1] = p2;
		toAdd->restitution = restitution;
		toAdd->contactNormal = contactNormal;
		toAdd->depth = depth;

		Contacts.push_back(toAdd);
	}

	void PhysicsWorld::Update(float time) {
		UpdateParticleList();
		forceRegistry.UpdateForces(time);
		
		for (std::list<Particle*>::iterator p = Particles.begin();
			p != Particles.end();
			p++)
		{
			(*p)->Update(time);
		}

		GenerateContacts();
		GetOverlaps();

		if (Contacts.size() > 0) {
			contactResolver.ResolveContacts(Contacts, time);
		}

	}

	void PhysicsWorld::UpdateParticleList() {
		Particles.remove_if([](Particle* p) { return p->IsDestroyed(); });
	}

	void PhysicsWorld::GenerateContacts()
	{
		Contacts.clear();

		//Link
		for (std::list<ParticleLink*>::iterator i = Links.begin();
			i != Links.end();
			i++)
		{
			ParticleContact* contact = (*i)->GetContact();
			if (contact != nullptr) {
				Contacts.push_back(contact);
			}

		}

		//Chain
		for (Chain* chain : Chains) {
			std::vector<ParticleContact*> chainContacts = chain->GetContacts();
			for (ParticleContact* c : chainContacts) {
				Contacts.push_back(c);
			}
		}
	}
	void PhysicsWorld::GetOverlaps()
	{
		//Checking Pairs
		for (int i = 0; i < Particles.size() - 1; i++)
		{
			//Access particle at index i (First Particle)
			list<Particle*>::iterator a = next(Particles.begin(), i);
			for (int j = i + 1; j < Particles.size(); j++)
			{
				//Access particle at index h (Second Particle)
				list<Particle*>::iterator b = next(Particles.begin(), j);

				glm::vec3 mag2Vec = (*a)->Position - (*b)->Position;
				float mag2 = glm::dot(mag2Vec, mag2Vec);
				float rad = (*a)->radius + (*b)->radius;
				float rad2 = rad * rad;

				//Check Collision
				if (mag2 <= rad2)
				{
					glm::vec3 dir = glm::normalize(mag2Vec);
					float r = rad2 - mag2;
					if (r <= 0) continue;
					float depth = sqrt(r);
					float restitution = fmin((*a)->restitution, (*b)->restitution);

					AddContact(*a, *b, restitution, dir, depth);
				}
			}
		}
	}
}