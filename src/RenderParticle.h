#include "Physics/Particle.h"
#include "../Model.h"

namespace Physics {
	class RenderParticle {
	public:
		Particle* PhysicsParticle;
		Model* RenderModel;
		glm::vec3 Color;

		RenderParticle(Particle* p, Model* obj) : PhysicsParticle(p), RenderModel(obj)
		{
			Color = glm::vec3(1.0f, 1.0f, 1.0f);
		};
		RenderParticle(Particle* p, Model* obj, glm::vec3 c) : PhysicsParticle(p), RenderModel(obj), Color(c) {};

		void Draw();
	};
}