#pragma once
#include "Physics/Particle.h"
#include "ForceGenerator.h"

namespace Physics {
	class DragForceGenerator : public ForceGenerator
	{
	private:
		float k1 = 0.74;
		float k2 = 0.57;

	public:
		DragForceGenerator() {}
		DragForceGenerator(float _k1, float _k2) : k1(_k1), k2(_k2) {}
		void UpdateForce(Particle* particle, float time);
	};
}	