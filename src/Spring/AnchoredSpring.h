#include "../ForceGenerator.h"
#include "../Physics/Particle.h"

namespace Physics {
	class AnchoredSpring : public ForceGenerator 
	{
		private:
			glm::vec3 anchorPoint;
			float springConstant;
			float restLength;

	public:
		AnchoredSpring(glm::vec3 pos, float _springConst, float _restLen) : 
			anchorPoint(pos), springConstant(_springConst), restLength(_restLen) {}

		void UpdateForce(Particle* particle, float time) override;
	};
}