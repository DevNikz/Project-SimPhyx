#pragma once

#include <glm/glm.hpp>


namespace Physics{
	class Particle 
	{
		protected:
			bool isDestroyed = false;

			void UpdatePosition(float deltaTime);
			void UpdateVelocity(float deltaTime);

			glm::vec3 accumulatedForce = glm::vec3(0.f, 0.f, 0.f);
			glm::vec3 gravity = glm::vec3(0.0f, -9.81f, 0.0f);
	
		public:
			void Destroy();
			bool IsDestroyed();

			glm::vec3 Position;
			glm::vec3 Velocity;
			glm::vec3 Acceleration;
			float damping = 0.9f;
			float mass = 1.f; //kg
			bool useGravity = false;

			void Update(float time);
			void ApplyForce(glm::vec3 force);
			void ResetForce();
			glm::vec3 GetPosition();

			Particle();
	};
}