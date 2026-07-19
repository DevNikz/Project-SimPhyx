#include "Particle.h"

namespace Physics {
	void Particle::UpdatePosition(float deltaTime)
	{
		this->Position = this->Position + (this->Velocity * deltaTime) +
			((1.0f / 2.0f) * (this->Acceleration * deltaTime * deltaTime));

		//Rotations
		glm::vec3 angularV = AngularVelocity * deltaTime;
		this->Rotation = this->Rotation + angularV;
	}

	void Particle::UpdateVelocity(float deltaTime)
	{
		float d_mass = glm::max(std::numeric_limits<float>::min(), mass);
		this->Acceleration += accumulatedForce * (1 / d_mass);
		this->Velocity = this->Velocity + (this->Acceleration * deltaTime);
		this->Velocity = this->Velocity * powf(damping, deltaTime);

		float mI = MomentOfInertia();
		AngularVelocity += accumulatedTorque * deltaTime * (1.f / mI);
		AngularVelocity = AngularVelocity * powf(AngularDampening, deltaTime);
	}

	float Particle::MomentOfInertia()
	{
		if (shape == Circle) {
			return ((float)2 / 5) * mass * radius * radius;
		}
		else {
			return (mass * (width * width + height * height)) / 12;
		}
	}

	void Particle::Destroy()
	{
		isDestroyed = true;
	}

	bool Particle::IsDestroyed()
	{
		return isDestroyed;
	}

	void Particle::Update(float time)
	{
		this->UpdatePosition(time);
		this->UpdateVelocity(time);
		this->ResetForce();
	}

	void Particle::ApplyForce(glm::vec3 force)
	{
		this->accumulatedForce += force;
	}

	void Particle::ResetForce()
	{
		this->accumulatedForce = glm::vec3(0.f, 0.f, 0.f);
		this->Acceleration = glm::vec3(0.f, 0.f, 0.f);
		this->accumulatedTorque = glm::vec3(0.f, 0.f, 0.f);
	}

	glm::vec3 Particle::GetPosition()
	{
		return this->Position;
	}

	void Particle::AddForceAtPoint(glm::vec3 force, glm::vec3 p)
	{
		this->ApplyForce(force);
		this->accumulatedTorque = glm::cross(p, force);
	}

	void Particle::AddTorqueAtPoint(glm::vec3 force, glm::vec3 p)
	{
		this->accumulatedTorque = glm::cross(p, force);
	}

	Particle::Particle()
	{
		this->Position = glm::vec3(0, 0, 0);
		this->Velocity = glm::vec3(0, 0, 0);
		this->Acceleration = glm::vec3(0, 0, 0); 
		this->Rotation = glm::vec3(0, 0, 0);
		this->Lifespan = 0.f;
	}
}

