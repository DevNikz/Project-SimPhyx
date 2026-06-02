#include "PhysicsWorld.h"

namespace Physics {
	void PhysicsWorld::AddParticle(Particle* p) {
		PhysicsParticles.push_back(p);
	}

	void PhysicsWorld::Update(float time) {
		UpdateParticleList();

		for (std::list<Particle*>::iterator p = PhysicsParticles.begin(); p != PhysicsParticles.end(); p++) {
			(*p)->Update(time);
		}
	}

	void PhysicsWorld::UpdateParticleList() {
		PhysicsParticles.remove_if([](Particle* p) { return p->IsDestroyed(); });
	}
}