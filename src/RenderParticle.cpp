#include "RenderParticle.h"

using namespace Physics;

void RenderParticle::Draw()
{
	if (PhysicsParticle->IsDestroyed()) return; 
	RenderModel->Color(Color);
	RenderModel->Position(PhysicsParticle->Position);
	RenderModel->DrawModel();

}