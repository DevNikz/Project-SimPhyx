// PlayerController.h
#pragma once
#include "../Physics/Particle.h"
#include "PlayerInput.h"

class PlayerController {
public:
    Physics::Particle* particle;

    float moveSpeed = 150.0f;
    float jumpVelocity = 350.0f;   // desired instantaneous upward speed on jump
    bool isGrounded = false;

    void HandleInput(const PlayerInput& input, float deltaTime) {
        // --- Horizontal: unchanged, direct velocity set ---
        particle->Velocity.x = input.moveDir.x * moveSpeed;


        // --- Jump: force-based ---
        if (input.jumpPressed && isGrounded) {
            Jump(deltaTime);
        }
    }

private:
    void Jump(float deltaTime) {
        // ApplyForce() feeds accumulatedForce -> Acceleration -> Velocity over ONE deltaTime,
        // then ResetForce() wipes it. So to land on an exact jumpVelocity (frame-rate independent),
        // solve F = mass * desiredVelocityChange / deltaTime.
        float force = particle->mass * jumpVelocity / deltaTime;
        particle->ApplyForce(glm::vec3(0.0f, force, 0.0f));
        isGrounded = false;
    }
};

inline bool CheckGrounded(Physics::Particle* player, std::vector<Physics::ParticleContact*> contacts,
    float groundNormalThreshold)
{
    for (Physics::ParticleContact* contact : contacts) {
        glm::vec3 normal = contact->contactNormal;
        bool playerIsA = (contact->particles[0] == player);
        bool playerIsB = (contact->particles[1] == player);

        if (!playerIsA && !playerIsB) continue;

        // contactNormal points from B to A (see AddContact: p1->Position - p2->Position style).
        // Flip it so it always points from "other" toward player.
        if (playerIsB) normal = -normal;

        // Strong upward component toward the player = something below them = grounded.
        if (normal.y > groundNormalThreshold) {
            return true;
        }
    }
    return false;
}