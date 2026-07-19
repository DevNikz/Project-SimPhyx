#pragma once

#include <glm/glm.hpp>

enum PrimitiveType {
	Circle,
	Rect
};

namespace Physics{
	class Particle 
	{
		protected:
			bool isDestroyed = false;

			void UpdatePosition(float deltaTime);
			void UpdateVelocity(float deltaTime);

			glm::vec3 accumulatedForce = glm::vec3(0.f, 0.f, 0.f);
			glm::vec3 accumulatedTorque = glm::vec3(0.f, 0.f, 0.f);
			virtual float MomentOfInertia();

			PrimitiveType shape = Circle;
	
		public:
			void SetPrimitive(PrimitiveType type) { shape = type; }
			PrimitiveType GetPrimitiveType() { return this->shape; }

			void Destroy();
			bool IsDestroyed();
			glm::vec3 Position;
			glm::vec3 Velocity;
			glm::vec3 Acceleration;
			glm::vec3 Rotation = glm::vec3(0.f, 0.f, 0.f);
			float Lifespan;
			float damping = 0.9f;
			float mass = 1.f; //kg
			float radius = 1.f;
			float restitution = 0.f;
			bool useGravity = true;
			glm::vec3 halfExtents;
			float width = 0.f;
			float height = 0.f;
			glm::vec3 extents;

			void Update(float time);
			void ApplyForce(glm::vec3 force);
			void ResetForce();
			glm::vec3 GetPosition();

			//Rotations
			glm::vec3 AngularVelocity = glm::vec3(0.f, 0.f, 0.f);
			float AngularDampening = 0.9f;
			void AddForceAtPoint(glm::vec3 force, glm::vec3 p);
			void AddTorqueAtPoint(glm::vec3 force, glm::vec3 p);
			PrimitiveType GetShape() { return this->shape; }
			glm::vec3 GetHalfExtents() const { return glm::vec3(width * 0.5f, height * 0.5f, 0.0f); }

			Particle();
	};
}