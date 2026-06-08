#include "Physics/Particle.h"
#include "../Model.h"

namespace Physics {
	class RenderParticle {
	public:
		Particle* PhysicsParticle;
		Model* RenderModel;
		glm::vec3 Color;
		glm::vec3 Scale;

		RenderParticle(Particle* p, Model* obj) : PhysicsParticle(p), RenderModel(obj)
		{
			Color = glm::vec3(1.0f, 1.0f, 1.0f);
			Scale = glm::vec3(1.0f, 1.0f, 1.0f);
		};
		RenderParticle(Particle* p, Model* obj, glm::vec3 c, glm::vec3 s) : PhysicsParticle(p), RenderModel(obj), Color(c), Scale(s) {};

		void Draw();
	};
}