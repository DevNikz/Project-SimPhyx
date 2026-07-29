#pragma once
#include "Particle.h"
#include <glm/gtc/quaternion.hpp>

namespace Physics {
	class PhysicsCube : public Particle 
	{
	public:
		glm::vec3 halfExtents;

		PhysicsCube();
		PhysicsCube(glm::vec3 _halfExtents);

	protected:
		float MomentOfInertia() override;
		void UpdateAngularVelocity(float deltaTime) override;

		glm::mat3 GetInertiaTensor();
		glm::mat3 GetWorldInvTensor();
	};
}