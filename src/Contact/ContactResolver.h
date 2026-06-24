#pragma once
#include <vector>
#include "ParticleContact.h"

namespace Physics {
	class ContactResolver {
	public:
		unsigned max_iterations;
		ContactResolver(unsigned _maxIterations) : max_iterations(_maxIterations) {}
		void ResolveContacts(std::vector<ParticleContact*> contacts, float time);

	protected:
		unsigned current_iterations = 0;
	};
}