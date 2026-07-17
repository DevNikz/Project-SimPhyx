#include "RenderParticle.h"

using namespace Physics;

void RenderParticle::Draw()
{
	if (PhysicsParticle->IsDestroyed()) return; 
	RenderModel->Color(Color);
	RenderModel->Position(PhysicsParticle->Position);
	RenderModel->Scale(Scale);
	RenderModel->Rotation(glm::vec3(1.f));
	RenderModel->DrawModel();
}

void RenderParticle::Draw(glm::vec3 rot)
{
	if (PhysicsParticle->IsDestroyed()) return;
	RenderModel->Color(Color);
	RenderModel->Position(PhysicsParticle->Position);
	RenderModel->Scale(Scale);
	RenderModel->Rotation(rot);
	RenderModel->DrawModel();
}