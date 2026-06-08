#include "PhysicsWorld.h"

namespace Physics {
	void PhysicsWorld::AddParticle(Particle* p) {
		Particles.push_back(p);
		forceRegistry.Add(p, &Gravity);
	}

	void PhysicsWorld::Update(float time) {
		UpdateParticleList();

		

		for (std::list<Particle*>::iterator p = Particles.begin(); p != Particles.end(); p++) {
			(*p)->Update(time);
		}

		//call this before velocity and pos
		forceRegistry.UpdateForces(time);

	}

	void PhysicsWorld::UpdateParticleList() {
		Particles.remove_if([](Particle* p) { return p->IsDestroyed(); });
	}
}