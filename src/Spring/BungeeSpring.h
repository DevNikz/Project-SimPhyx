/*
This class is an example of a bungee spring.
It's similar to an anchored spring, except it doesn't
push the particle.

The cord / spring doesn't exert force when it is within the range of its restLength.
It will only exert force once the spring goes past its restLength.
*/

#pragma once
#include "../Physics/Particle.h"
#include "../ForceGenerator.h"

namespace Physics {
	class BungeeSpring : public ForceGenerator 
	{
	private:
		glm::vec3 anchorPoint;
		float springConstant;
		float restLength;

	public:
		BungeeSpring(const glm::vec3& _anchorPoint, float _springConst, float _restLength) : 
			anchorPoint(_anchorPoint), springConstant(_springConst), restLength(_restLength) {}

		void UpdateForce(Particle* particle, float time) override;
	};
}