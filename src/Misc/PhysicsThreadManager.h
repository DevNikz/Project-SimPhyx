#pragma once
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <chrono>
#include <functional>
#include <glm/glm.hpp>
#include "../Physics/Particle.h"
using namespace std;
using namespace Physics;

struct ParticleSnapshot {
	glm::vec3 position = glm::vec3(0.f);
	glm::vec3 velocity = glm::vec3(0.f);
};

class PhysicsThreadManager {
public:
	using UpdateFn = std::function<void(float deltaTime)>;

	explicit PhysicsThreadManager(UpdateFn _updateFn, int physicsHz = 60) :
		updateFn(move(_updateFn)),
		timestep(chrono::nanoseconds(static_cast<long long>(1e9 / physicsHz))) { }

	void Track(Particle* p) {
		particles.push_back(p);
		snapshot.push_back({});
	}

	void Start() {
		if (running.load()) return;
		running.store(true);
		Thread = std::thread(&PhysicsThreadManager::Loop, this);
	}

	void Stop() {
		running.store(false);
		if (Thread.joinable()) Thread.join();
	}

	std::vector<ParticleSnapshot> GetSnapshot() const {
		std::lock_guard<std::mutex> lock(mutex);
		return snapshot;
	}

	size_t Count() const { return particles.size(); }
	~PhysicsThreadManager() { Stop(); }

	PhysicsThreadManager(const PhysicsThreadManager&) = delete;
	PhysicsThreadManager& operator=(const PhysicsThreadManager&) = delete;

private:
	void Loop() {
		using clock = std::chrono::high_resolution_clock;
		auto prev = clock::now();
		std::chrono::nanoseconds acc(0);

		while (running.load()) {
			auto now = clock::now();
			acc += chrono::duration_cast<chrono::nanoseconds>(now - prev);
			prev = now;

			if (acc >= timestep) {
				const float deltaTime = static_cast<float>(acc.count()) / 1e9f;
				acc = std::chrono::nanoseconds(0);
				updateFn(deltaTime);

				std::lock_guard<std::mutex> lock(mutex);
				for (size_t i = 0; i < particles.size(); ++i) {
					snapshot[i].position = particles[i]->Position;
					snapshot[i].velocity = particles[i]->Velocity;
				}
			}

			this_thread::sleep_for(chrono::milliseconds(1));
		}
	}

	UpdateFn updateFn;
	std::chrono::nanoseconds timestep;
	vector<Particle*> particles;
	mutable mutex mutex;
	vector<ParticleSnapshot> snapshot;
	atomic<bool> running{ false };
	thread Thread;
};