#include "../ForceGenerator.h"
#include "../Physics/Particle.h"

namespace Physics {
	class ParticleSpring : public ForceGenerator 
	{
	private:
		Particle* otherParticle;
		float springConstant;
		float restLength;

	public:
		ParticleSpring(Particle* particle, float _springConst, float _restLen) :
			otherParticle(particle), springConstant(_springConst), restLength(_restLen) {
		}

		void UpdateForce(Particle* particle, float time) override;
	};
}
