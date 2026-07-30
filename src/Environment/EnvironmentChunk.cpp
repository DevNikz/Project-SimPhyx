#include "EnvironmentChunk.h"

#include <cmath>

namespace
{
    constexpr float TileSize = 32.0f;
    constexpr float MiddleHeight = TileSize * 10.0f;
    constexpr float BottomHeight = TileSize;
    constexpr float CoinRenderSize = 40.0f;
    constexpr float CoinColliderSize = 34.0f;
    constexpr float CoinSpacing = 52.0f;
    constexpr float SpikeWidth = 96.0f;
    constexpr float SpikeHeight = 72.0f;
    constexpr float PendulumSegmentLength = 70.0f;
}

EnvironmentChunk::EnvironmentChunk(float centerX, float width, float floorY,
    float worldBottomY, Physics::Shader* spriteShader,
    Physics::PhysicsWorld* physicsWorld)
    : m_centerX(centerX),
      m_width(width),
      m_floorY(floorY),
      m_worldBottomY(worldBottomY),
      m_floorSprite(glm::vec2(width / TileSize, 1.0f)),
      m_middleSprite(glm::vec2(width / TileSize, 10.0f)),
      m_bottomSprite(glm::vec2(width / TileSize, 1.0f)),
      m_coinSprite(glm::vec2(1.0f)),
      m_floorCollider(std::make_unique<Physics::Particle>())
{
    m_floorSprite.IsTiled(true);
    m_floorSprite.setShader(spriteShader);
    m_floorSprite.loadTexture("3D/env/ground.png", 1, 1);
    m_floorSprite.setScale(glm::vec2(m_width, TileSize));

    m_middleSprite.IsTiled(true);
    m_middleSprite.setShader(spriteShader);
    m_middleSprite.loadTexture("3D/env/ground1.png", 1, 1);
    m_middleSprite.setScale(glm::vec2(m_width, MiddleHeight));

    m_bottomSprite.IsTiled(true);
    m_bottomSprite.setShader(spriteShader);
    m_bottomSprite.loadTexture("3D/env/ground2.png", 1, 1);
    m_bottomSprite.setScale(glm::vec2(m_width, BottomHeight));

    m_coinSprite.setShader(spriteShader);
    m_coinSprite.setTexture(
        Physics::Quad::LoadTextureCached("3D/env/goldcoin.png"), 1, 1);
    m_coinSprite.setScale(glm::vec2(CoinRenderSize));

    m_floorCollider->mass = 1.0e15f;
    m_floorCollider->restitution = 0.0f;
    m_floorCollider->width = m_width;
    m_floorCollider->height = TileSize;
    m_floorCollider->extents = glm::vec3(m_width, TileSize, 0.0f);
    m_floorCollider->halfExtents = glm::vec3(m_width * 0.5f, TileSize * 0.5f, 0.0f);
    m_floorCollider->SetPrimitive(Rect);
    m_floorCollider->useGravity = false;
    m_floorCollider->collisionLayer = Physics::CollisionWorld;

    UpdatePositions();
    physicsWorld->AddParticle(m_floorCollider.get());

    for (Coin& coin : m_coins) {
        coin.particle = std::make_unique<Physics::Particle>();
        coin.particle->width = CoinColliderSize;
        coin.particle->height = CoinColliderSize;
        coin.particle->extents = glm::vec3(CoinColliderSize, CoinColliderSize, 0.0f);
        coin.particle->halfExtents = glm::vec3(CoinColliderSize * 0.5f);
        coin.particle->halfExtents.z = 0.0f;
        coin.particle->SetPrimitive(Rect);
        coin.particle->useGravity = false;
        coin.particle->isTrigger = true;
        coin.particle->isActive = false;
        coin.particle->collisionLayer = Physics::CollisionCollectible;
        coin.particle->collisionMask = Physics::CollisionPlayer;
        physicsWorld->AddParticle(coin.particle.get());
    }

    GenerateCoins();
    SetupHazards(spriteShader, physicsWorld);
    GenerateHazard();
}

EnvironmentChunk::~EnvironmentChunk()
{
    m_floorSprite.DeleteBuffers();
    m_middleSprite.DeleteBuffers();
    m_bottomSprite.DeleteBuffers();
    m_coinSprite.DeleteBuffers();
    m_spikeHazard.sprite.DeleteBuffers();
    m_pendulumHazard.chainSprite.DeleteBuffers();
    m_pendulumHazard.spikeBallSprite.DeleteBuffers();
}

void EnvironmentChunk::SetCenterX(float centerX)
{
    m_centerX = centerX;
    UpdatePositions();
    GenerateCoins();
    GenerateHazard();
}

void EnvironmentChunk::UpdatePositions()
{
    m_floorSprite.setPosition(glm::vec3(m_centerX, m_floorY, 0.0f));
    m_floorCollider->Position = glm::vec3(m_centerX, m_floorY, 0.0f);
    m_floorCollider->Velocity = glm::vec3(0.0f);

    const float middleY = m_floorY - TileSize * 0.5f - MiddleHeight * 0.5f;
    m_middleSprite.setPosition(glm::vec3(m_centerX, middleY, 0.0f));

    const float bottomY = m_worldBottomY + BottomHeight * 0.5f;
    m_bottomSprite.setPosition(glm::vec3(m_centerX, bottomY, 0.0f));
}

void EnvironmentChunk::ActivateCoin(int& nextCoin, const glm::vec2& localPosition)
{
    if (nextCoin >= MaxCoins)
        return;

    Physics::Particle* particle = m_coins[nextCoin].particle.get();
    particle->Position = glm::vec3(
        m_centerX + localPosition.x,
        m_floorY + localPosition.y,
        0.0f);
    particle->Velocity = glm::vec3(0.0f);
    particle->isActive = true;
    ++nextCoin;
}

void EnvironmentChunk::GenerateCoins()
{
    for (Coin& coin : m_coins)
        coin.particle->isActive = false;

    std::uniform_int_distribution<int> patternDistribution(0, 1);
    std::uniform_int_distribution<int> rowSizeDistribution(3, 4);
    std::uniform_real_distribution<float> heightDistribution(90.0f, 260.0f);
    int nextCoin = 0;

    // Generate one bunch in each half of the chunk so they do not overlap.
    for (int bunch = 0; bunch < 2; ++bunch) {
        const float sectionCenter =
            -m_width * 0.25f + bunch * (m_width * 0.5f);
        const float height = heightDistribution(m_random);

        if (patternDistribution(m_random) == 0) {
            // Horizontal row of three or four coins.
            const int coinCount = rowSizeDistribution(m_random);
            const float rowWidth = (coinCount - 1) * CoinSpacing;
            for (int i = 0; i < coinCount; ++i) {
                ActivateCoin(nextCoin, glm::vec2(
                    sectionCenter - rowWidth * 0.5f + i * CoinSpacing,
                    height));
            }
        }
        else {
            // Two-by-two square.
            ActivateCoin(nextCoin, glm::vec2(sectionCenter - CoinSpacing * 0.5f, height));
            ActivateCoin(nextCoin, glm::vec2(sectionCenter + CoinSpacing * 0.5f, height));
            ActivateCoin(nextCoin, glm::vec2(sectionCenter - CoinSpacing * 0.5f, height + CoinSpacing));
            ActivateCoin(nextCoin, glm::vec2(sectionCenter + CoinSpacing * 0.5f, height + CoinSpacing));
        }
    }
}

bool EnvironmentChunk::CollectCoin(Physics::Particle* particle)
{
    for (Coin& coin : m_coins) {
        if (coin.particle.get() == particle && coin.particle->isActive) {
            coin.particle->isActive = false;
            return true;
        }
    }

    return false;
}

void EnvironmentChunk::SetupHazards(
    Physics::Shader* spriteShader,
    Physics::PhysicsWorld* physicsWorld)
{
    m_spikeHazard.sprite.setShader(spriteShader);
    m_spikeHazard.sprite.setTexture(
        Physics::Quad::LoadTextureCached("3D/env/spikes.png"), 1, 1);
    m_spikeHazard.sprite.setScale(glm::vec2(SpikeWidth, SpikeHeight)); //spike WIDTH x HEIGHT

    m_spikeHazard.leftParticle = std::make_unique<Physics::Particle>();
    m_spikeHazard.rightParticle = std::make_unique<Physics::Particle>();
    for (Physics::Particle* particle : {
        m_spikeHazard.leftParticle.get(),
        m_spikeHazard.rightParticle.get() }) {
        particle->width = SpikeWidth * 0.5f;
        particle->height = 60.0f;
        particle->extents = glm::vec3(particle->width, particle->height, 0.0f);
        particle->halfExtents = glm::vec3(particle->width * 0.1f, particle->height * 0.2f, 0.0f);
        particle->SetPrimitive(Rect);
        particle->useGravity = false;
        particle->isTrigger = true;
        particle->isActive = false;
        particle->collisionLayer = Physics::CollisionHazard;
        particle->collisionMask = Physics::CollisionPlayer;
        physicsWorld->AddParticle(particle);
    }

    m_spikeHazard.rod = std::make_unique<Physics::Rod>();
    m_spikeHazard.rod->particles.push_back(m_spikeHazard.leftParticle.get());
    m_spikeHazard.rod->particles.push_back(m_spikeHazard.rightParticle.get());
    m_spikeHazard.rod->length = SpikeWidth * 0.5f;
    m_spikeHazard.rod->restitution = 0.0f;
    physicsWorld->Links.push_back(m_spikeHazard.rod.get());

    m_pendulumHazard.chainSprite.setShader(spriteShader);
    m_pendulumHazard.chainSprite.setTexture(
        Physics::Quad::LoadTextureCached("3D/env/chain.png"), 1, 1);

    m_pendulumHazard.spikeBallSprite.setShader(spriteShader);
    m_pendulumHazard.spikeBallSprite.setTexture(
        Physics::Quad::LoadTextureCached("3D/env/spikeball.png"), 1, 1);
    m_pendulumHazard.spikeBallSprite.setScale(glm::vec2(72.0f));

    m_pendulumHazard.anchor = std::make_unique<Physics::Particle>();
    m_pendulumHazard.movingLink = std::make_unique<Physics::Particle>();
    m_pendulumHazard.spikeBall = std::make_unique<Physics::Particle>();

    m_pendulumHazard.anchor->mass = 1.0e15f;
    m_pendulumHazard.anchor->useGravity = false;
    m_pendulumHazard.anchor->isActive = false;
    m_pendulumHazard.anchor->collisionMask = Physics::CollisionNone;

    m_pendulumHazard.movingLink->mass = 1.0f;
    m_pendulumHazard.movingLink->useGravity = true;
    m_pendulumHazard.movingLink->damping = 0.995f;
    m_pendulumHazard.movingLink->isActive = false;
    m_pendulumHazard.movingLink->collisionMask = Physics::CollisionNone;

    m_pendulumHazard.spikeBall->mass = 2.5f;
    m_pendulumHazard.spikeBall->useGravity = true;
    m_pendulumHazard.spikeBall->damping = 0.995f;
    m_pendulumHazard.spikeBall->width = 60.0f;
    m_pendulumHazard.spikeBall->height = 60.0f;
    m_pendulumHazard.spikeBall->extents = glm::vec3(60.0f, 60.0f, 0.0f);
    m_pendulumHazard.spikeBall->halfExtents = glm::vec3(30.0f, 30.0f, 0.0f);
    m_pendulumHazard.spikeBall->SetPrimitive(Rect);
    m_pendulumHazard.spikeBall->isTrigger = true;
    m_pendulumHazard.spikeBall->isActive = false;
    m_pendulumHazard.spikeBall->collisionLayer = Physics::CollisionHazard;
    m_pendulumHazard.spikeBall->collisionMask = Physics::CollisionPlayer;

    physicsWorld->AddParticle(m_pendulumHazard.anchor.get());
    physicsWorld->AddParticle(m_pendulumHazard.movingLink.get());
    physicsWorld->AddParticle(m_pendulumHazard.spikeBall.get());

    m_pendulumHazard.chain = std::make_unique<Physics::Chain>();
    m_pendulumHazard.chain->AddParticle(m_pendulumHazard.anchor.get());
    m_pendulumHazard.chain->AddParticle(m_pendulumHazard.movingLink.get());
    m_pendulumHazard.chain->AddParticle(m_pendulumHazard.spikeBall.get());
    m_pendulumHazard.chain->length = PendulumSegmentLength;
    m_pendulumHazard.chain->restitution = 0.0f;
    physicsWorld->Chains.push_back(m_pendulumHazard.chain.get());
}

void EnvironmentChunk::DisableHazards()
{
    for (Physics::Particle* particle : {
        m_spikeHazard.leftParticle.get(),
        m_spikeHazard.rightParticle.get(),
        m_pendulumHazard.anchor.get(),
        m_pendulumHazard.movingLink.get(),
        m_pendulumHazard.spikeBall.get() }) {
        particle->isActive = false;
        particle->Velocity = glm::vec3(0.0f);
        particle->Acceleration = glm::vec3(0.0f);
        particle->ResetForce();
    }
}

void EnvironmentChunk::ActivateSpikeRod(float centerX)
{
    const float floorTop = m_floorY + TileSize * 0.5f;
    const float centerY = floorTop + SpikeHeight * 0.5f;

    m_spikeHazard.leftParticle->Position =
        glm::vec3(centerX - SpikeWidth * 0.25f, centerY, 0.0f);
    m_spikeHazard.rightParticle->Position =
        glm::vec3(centerX + SpikeWidth * 0.25f, centerY, 0.0f);
    m_spikeHazard.leftParticle->isActive = true;
    m_spikeHazard.rightParticle->isActive = true;
}

void EnvironmentChunk::ActivatePendulum(float centerX)
{
    const float anchorY = 100.0f;
    const float horizontalOffset = 35.0f;
    const float verticalOffset = std::sqrt(
        PendulumSegmentLength * PendulumSegmentLength -
        horizontalOffset * horizontalOffset);

    m_pendulumHazard.anchor->Position = glm::vec3(centerX, anchorY, 0.0f);
    m_pendulumHazard.movingLink->Position = glm::vec3(
        centerX + horizontalOffset,
        anchorY - verticalOffset,
        0.0f);
    m_pendulumHazard.spikeBall->Position = glm::vec3(
        centerX + horizontalOffset * 2.0f,
        anchorY - verticalOffset * 2.0f,
        0.0f);

    m_pendulumHazard.anchor->isActive = true;
    m_pendulumHazard.movingLink->isActive = true;
    m_pendulumHazard.spikeBall->isActive = true;
}

void EnvironmentChunk::GenerateHazard()
{
    DisableHazards();

    std::uniform_int_distribution<int> hazardDistribution(0, 9);
    const int selection = hazardDistribution(m_random);
    if (selection <= 2)
        return;

    if (selection <= 6)
        ActivateSpikeRod(m_centerX);
    else
        ActivatePendulum(m_centerX);
}

void EnvironmentChunk::DrawHazards()
{
    if (m_spikeHazard.leftParticle->isActive) {
        const glm::vec3 midpoint =
            (m_spikeHazard.leftParticle->Position +
             m_spikeHazard.rightParticle->Position) * 0.5f;
        m_spikeHazard.sprite.setPosition(midpoint);
        m_spikeHazard.sprite.setRotationDegrees(0.0f);
        m_spikeHazard.sprite.draw();
    }

    if (!m_pendulumHazard.anchor->isActive)
        return;

    const std::array<Physics::Particle*, 3> particles = {
        m_pendulumHazard.anchor.get(),
        m_pendulumHazard.movingLink.get(),
        m_pendulumHazard.spikeBall.get()
    };

    for (size_t i = 0; i + 1 < particles.size(); ++i) {
        const glm::vec3 difference = particles[i + 1]->Position - particles[i]->Position;
        const glm::vec3 midpoint = (particles[i]->Position + particles[i + 1]->Position) * 0.5f;
        const float length = glm::length(glm::vec2(difference));
        const float angle = glm::degrees(std::atan2(difference.y, difference.x)) - 90.0f;

        m_pendulumHazard.chainSprite.setPosition(midpoint);
        m_pendulumHazard.chainSprite.setScale(glm::vec2(20.0f, length + 4.0f));
        m_pendulumHazard.chainSprite.setRotationDegrees(angle);
        m_pendulumHazard.chainSprite.draw();
    }

    m_pendulumHazard.spikeBallSprite.setPosition(m_pendulumHazard.spikeBall->Position);
    m_pendulumHazard.spikeBallSprite.setRotationDegrees(0.0f);
    m_pendulumHazard.spikeBallSprite.draw();
}

void EnvironmentChunk::Draw()
{
    m_bottomSprite.draw();
    m_middleSprite.draw();
    m_floorSprite.draw();
    DrawHazards();

    for (const Coin& coin : m_coins) {
        if (!coin.particle->isActive)
            continue;

        m_coinSprite.setPosition(coin.particle->Position);
        m_coinSprite.draw();
    }
}
