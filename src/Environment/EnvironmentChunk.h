#pragma once

#include <array>
#include <memory>
#include <random>

#include "../../Quad.h"
#include "../../Shader.h"
#include "../Physics/Particle.h"
#include "../Physics/PhysicsWorld.h"
#include "../Link/Chain.h"
#include "../Link/Rod.h"

class EnvironmentChunk
{
public:
    EnvironmentChunk(float centerX, float width, float floorY,
        float worldBottomY, Physics::Shader* spriteShader,
        Physics::PhysicsWorld* physicsWorld);
    ~EnvironmentChunk();

    EnvironmentChunk(const EnvironmentChunk&) = delete;
    EnvironmentChunk& operator=(const EnvironmentChunk&) = delete;

    float GetCenterX() const { return m_centerX; }
    float GetWidth() const { return m_width; }

    void SetCenterX(float centerX);
    bool CollectCoin(Physics::Particle* particle);
    void Draw();

private:
    static constexpr int MaxCoins = 8;

    struct Coin
    {
        std::unique_ptr<Physics::Particle> particle;
    };

    struct SpikeRodHazard
    {
        std::unique_ptr<Physics::Particle> leftParticle;
        std::unique_ptr<Physics::Particle> rightParticle;
        std::unique_ptr<Physics::Rod> rod;
        Physics::Quad sprite{ glm::vec2(1.0f) };
    };

    struct PendulumHazard
    {
        std::unique_ptr<Physics::Particle> anchor;
        std::unique_ptr<Physics::Particle> movingLink;
        std::unique_ptr<Physics::Particle> spikeBall;
        std::unique_ptr<Physics::Chain> chain;
        Physics::Quad chainSprite{ glm::vec2(1.0f) };
        Physics::Quad spikeBallSprite{ glm::vec2(1.0f) };
    };

    void UpdatePositions();
    void GenerateCoins();
    void ActivateCoin(int& nextCoin, const glm::vec2& localPosition);
    void SetupHazards(Physics::Shader* spriteShader, Physics::PhysicsWorld* physicsWorld);
    void GenerateHazard();
    void DisableHazards();
    void ActivateSpikeRod(float centerX);
    void ActivatePendulum(float centerX);
    void DrawHazards();

    float m_centerX;
    float m_width;
    float m_floorY;
    float m_worldBottomY;

    Physics::Quad m_floorSprite;
    Physics::Quad m_middleSprite;
    Physics::Quad m_bottomSprite;
    Physics::Quad m_coinSprite;
    std::unique_ptr<Physics::Particle> m_floorCollider;
    std::array<Coin, MaxCoins> m_coins;
    SpikeRodHazard m_spikeHazard;
    PendulumHazard m_pendulumHazard;
    std::mt19937 m_random{ std::random_device{}() };
};
