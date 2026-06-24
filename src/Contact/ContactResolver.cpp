#include "ContactResolver.h"
using namespace std;
using namespace Physics;

void Physics::ContactResolver::ResolveContacts(std::vector<ParticleContact*> contacts, float time)
{
	current_iterations = 0;
	while (true) {
		ParticleContact* leastContact = nullptr;
		float lowestSpeed = std::numeric_limits<float>::max();


		for (unsigned i = 0; i < contacts.size(); i++) {
			float speed = contacts[i]->GetSeparatingSpeed();
			if (speed < lowestSpeed) {
				lowestSpeed = speed;
				leastContact = contacts[i];
			}
		}

		/*
		for (ParticleContact* contact : contacts) {
			float speed = contact->GetSeparatingSpeed();
			if (speed < lowestSpeed) {
				lowestSpeed = speed;
				leastContact = contact;
			}
		}
		*/

		if (leastContact == nullptr || lowestSpeed >= 0.0f) break;

		leastContact->Resolve(time);

		++current_iterations;

		if (current_iterations >= max_iterations) break;
	}
}
