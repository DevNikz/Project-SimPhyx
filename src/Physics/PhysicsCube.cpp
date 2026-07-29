#include "PhysicsCube.h"

using namespace std;
using namespace Physics;

PhysicsCube::PhysicsCube() 
{
	halfExtents = glm::vec3(0.5f);
}

PhysicsCube::PhysicsCube(glm::vec3 _halfExtents) 
{
	halfExtents = _halfExtents;
}

float Physics::PhysicsCube::MomentOfInertia()
{
	float l = halfExtents.x * 2.f;
	return (1.f / 6.f) * mass * (l * l);
}

void Physics::PhysicsCube::UpdateAngularVelocity(float deltaTime)
{
	glm::mat3 worldInvTensor = GetWorldInvTensor();
	glm::vec3 angularAccel = worldInvTensor * accumulatedTorque;
	AngularVelocity += angularAccel * deltaTime;
	AngularVelocity = AngularVelocity * powf(AngularDampening, deltaTime);
}

glm::mat3 Physics::PhysicsCube::GetInertiaTensor()
{
	glm::vec3 size = halfExtents * 2.f;
	float w = size.x; //width (w)
	float h = size.y; //height (h)
	float d = size.z; //depth (z)

	float ix = (1.f / 12.f) * mass * (h * h + d * d);
	float iy = (1.f / 12.f) * mass * (w * w + d * d);
	float iz = (1.f / 12.f) * mass * (w * w + h * h);

	return glm::mat3(
		glm::vec3(ix, 0, 0),
		glm::vec3(0, iy, 0),
		glm::vec3(0, 0, iz)
	);
}

glm::mat3 Physics::PhysicsCube::GetWorldInvTensor()
{
	glm::mat3 localTensor = GetInertiaTensor();
	
	glm::mat3 localInverseTensor = glm::mat3(
		glm::vec3(1.f / localTensor[0][0], 0.f, 0.f),
		glm::vec3(0.f, 1.f / localTensor[1][1], 0.f),
		glm::vec3(0.f, 0.f, 1.f / localTensor[2][2])
	);

	glm::quat orient = glm::quat(Particle::Rotation);
	glm::mat3 R = glm::mat3_cast(orient);

	return R * localInverseTensor * glm::transpose(R);
}
