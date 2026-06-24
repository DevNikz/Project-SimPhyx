#include "ParticleLink.h"
using namespace std;
using namespace Physics;

//assuming it gets the first and last particle (only 2 particles)
float ParticleLink::CurrentLength() {
	glm::vec3 ret = particles[0]->Position - particles[1]->Position;
	return glm::length(ret);
}