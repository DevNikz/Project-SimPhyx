#include "ForceRegistry.h"

namespace Physics {
	void ForceRegistry::Add(Particle* particle, ForceGenerator* generator)
	{
		ParticleForceRegistry fr;
		fr.particle = particle;
		fr.generator = generator;

		Registry.push_back(fr);
	}
	void ForceRegistry::Remove(Particle* particle, ForceGenerator* generator)
	{
		Registry.remove_if(
			[particle, generator](ParticleForceRegistry reg) {
				return reg.particle == particle && reg.generator == generator;
			}
		);
	}
	void ForceRegistry::Clear()
	{
		Registry.clear();
	}
	void ForceRegistry::UpdateForces(float time)
	{
		for (std::list<ParticleForceRegistry>::iterator i = Registry.begin();
			i != Registry.end();
			i++) {
			i->generator->UpdateForce(i->particle, time);
		}
	}
}