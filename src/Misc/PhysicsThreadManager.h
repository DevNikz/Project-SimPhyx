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
	using SpawnAgeCallback = std::function<void(float /*deltaTime*/)>;

	explicit PhysicsThreadManager(PhysicsWorld* _world,
								  chrono::nanoseconds _timestep) 
		: world(_world)
		, timestep(_timestep)
		, paused(true)
		, running(false)
	{}

	// Non-copyable
	PhysicsThreadManager(const PhysicsThreadManager&) = delete;
	PhysicsThreadManager& operator=(const PhysicsThreadManager&) = delete;

	~PhysicsThreadManager() { Stop(); }

	void SetSpawnAgeCallback(SpawnAgeCallback cb) { spawnAgeCB = std::move(cb); }

	void Start()
	{
		running = true;
		physicsThread = std::thread(&PhysicsThreadManager::PhysicsLoop, this);
		renderThread = std::thread(&PhysicsThreadManager::RenderLoop, this);
	}

	void Stop()
	{
		if (!running) return;
		running = false;

		// Wake the thread if it is sleeping on the condition variable.
		{
			std::lock_guard<std::mutex> lk(mutex);
			paused = false;           // unblock the wait
		}
		cv.notify_all();

		if (physicsThread.joinable())  physicsThread.join();
		if (renderThread.joinable()) renderThread.join();
	}

	void TogglePaused()
	{
		{
			std::lock_guard<std::mutex> lk(mutex);
			paused = !paused;
		}
		cv.notify_all();
	}

	void SetPaused(bool pause)
	{
		{
			std::lock_guard<std::mutex> lk(mutex);
			paused = pause;
		}
		cv.notify_all();
	}
	
	bool IsPaused() const { return paused.load(); }
	std::mutex& ParticleMutex() { return particleMutex; }

private:
	void PhysicsLoop()
	{
		using clock = std::chrono::high_resolution_clock;
		const float dt = timestep.count() / 1'000'000'000.f;
		std::chrono::nanoseconds accumulator(0);

		while (running)
		{
			{
				std::unique_lock<std::mutex> lk(mutex);
				cv.wait(lk, [this] { return !paused || !running; });
			}
			if (!running) break;

			// Reset clock on resume — prevents catch-up spiral
			auto prev = clock::now();

			while (running && !paused)
			{
				auto now = clock::now();
				accumulator += std::chrono::duration_cast<std::chrono::nanoseconds>(now - prev);
				prev = now;

				while (accumulator >= timestep)
				{
					accumulator -= timestep;
					world->Update(dt);
				}
			}

			accumulator = std::chrono::nanoseconds(0); // clear on pause
		}
	}

	void RenderLoop()
	{
		using clock = std::chrono::high_resolution_clock;

		while (running)
		{
			{
				std::unique_lock<std::mutex> lk(mutex);
				cv.wait(lk, [this] { return !paused || !running; });
			}
			if (!running) break;

			auto prev = clock::now();

			while (running && !paused)
			{
				auto now = clock::now();
				float frameDt = std::chrono::duration_cast<std::chrono::nanoseconds>(now - prev).count()
					/ 1'000'000'000.f;
				prev = now;

				if (spawnAgeCB)
				{
					// Lock particle containers for the duration of the callback
					std::lock_guard<std::mutex> lk(particleMutex);
					spawnAgeCB(frameDt);
				}
			}
		}
	}

	PhysicsWorld* world;
	std::chrono::nanoseconds timestep;
	SpawnAgeCallback spawnAgeCB;

	std::atomic<bool> paused;
	std::atomic<bool> running;

	std::mutex mutex;
	std::condition_variable cv;

	std::mutex particleMutex;
	
	std::thread physicsThread;
	std::thread renderThread;
};