/*
CHAIN

- This is a chain particle class where it's basically like a rod, but instead
it makes use of particles connected with rigid segments.

- For every N particles, there are N-1 segments.

- The GetContacts() functions checks whether each segment of the chain is:
(1) at rest, (2) stretched, or (3) compressed

- The contact normal basically tells the physics engine which direction to 
push the particles to restore the correct segment length, while the depth tells
how much correction is needed.

*/


#include "Chain.h"
#include <iostream>
using namespace std;
using namespace Physics;

void Chain::AddParticle(Particle* p) {
	particles.push_back(p);
}

int Chain::SegmentCount() const {
	return static_cast<int>(particles.size()) - 1;
}

vector<ParticleContact*> Chain::GetContacts() const {
	vector<ParticleContact*> contacts;
	if (particles.size() < 2) return contacts; // no contacts since its 0

	//cout << "Segments: " << SegmentCount() << endl;

	for (int i = 0; i < SegmentCount(); i++) {

		Particle* a = particles[i];
		Particle* b = particles[i+1];

		float currLen = SegmentLength(i);
		if (currLen == length) continue;

		ParticleContact* contact = new ParticleContact();
		contact->particles[0] = a;
		contact->particles[1] = b;
		contact->restitution = restitution;
		
		glm::vec3 dir = b->Position - a->Position;
		dir = glm::normalize(dir);

		if (currLen > length) {
			//Segment is stretched
			contact->contactNormal = dir;
			contact->depth = currLen - length;
		}
		else {
			//Segment is compressed
			contact->contactNormal = -dir;
			contact->depth = length - currLen;
		}

		contacts.push_back(contact);
	}
	return contacts;
}

float Chain::SegmentLength(int i) const {
	glm::vec3 delta = particles[i]->Position - particles[i+1]->Position;
	return glm::length(delta);
}