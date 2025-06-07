#include "particle.h"

#include "gridstate.h"
#include "quadtree.h"


#include <fmod.hpp>
#include <fmod_errors.h>

#include <cstdlib>
#include <iostream>

Particle::Particle() {}

void Particle::Initialise(Sprite* sprite, ParticleType type) {
    m_pSprite = sprite;
    m_type = type;
}

void Particle::Activate(Vector2 position) {

    if (m_active) return;


    m_position = position;
    m_lifetime = 0.0f;
    m_active = true;
    m_seekingTarget = false;
    m_maxLifetime = 1.0f;

    switch (m_type) {
    case ParticleType::BlockPickup: //receive items/materials from block
        m_speedMultiplier = 15.0f;
        m_velocity = Vector2(
            (((rand() % 200) - 100) / 100.0f) * m_speedMultiplier,
            (((rand() % 200) - 100) / 100.0f) * m_speedMultiplier
        );
        m_maxLifetime = 3.2f;
        break;

    case ParticleType::DigDirt:
        if (m_type == ParticleType::DigDirt && m_pSprite) {
            float scale = 5.0f + static_cast<float>(rand()) / RAND_MAX * 3.0f;
            float colourVariation = 0.4f + static_cast<float>(rand()) / RAND_MAX * 0.25f;
            m_pSprite->SetScale(7.0f);
        }
        m_speedMultiplier = 200.0f;
        m_velocity = Vector2(
            (((rand() % 200) - 100) / 100.0f) * m_speedMultiplier,
            -std::abs(((rand() % 100) / 100.0f) * m_speedMultiplier)
        );
        m_maxLifetime = 4.0f;
        break;

    case ParticleType::BlockBreak:
        if (m_type == ParticleType::BlockBreak && m_pSprite) {
            float scale = 8.0f + static_cast<float>(rand()) / RAND_MAX * 5.0f;
            float colourVariation = 0.5f + static_cast<float>(rand()) / RAND_MAX * 0.35f;
            m_pSprite->SetScale(scale);
        }
        m_speedMultiplier = 400.0f;
        m_velocity = Vector2(
            (((rand() % 200) - 100) / 100.0f) * m_speedMultiplier,
            (((rand() % 200) - 100) / 100.0f) * m_speedMultiplier * 1.2f
        );
        m_maxLifetime = 4.0f;
        break;

    case ParticleType::WaterDrop:
        if (m_type == ParticleType::WaterDrop && m_pSprite) {
            float scale = 8.0f + static_cast<float>(rand()) / RAND_MAX * 5.0f;
            m_pSprite->SetScale(scale);
            m_pSprite->SetRedTint(0.3f);
            m_pSprite->SetGreenTint(0.5f);
            m_pSprite->SetBlueTint(1.0f);
            m_pSprite->SetAlpha(0.35f);
        }
        m_speedMultiplier = 400.0f;
        m_velocity = Vector2(0.0f, 0.1f);
        m_maxLifetime = 4.0f;
        break;
    }
}



void Particle::Update(float deltaTime) {
    if (!m_active) return;

    m_lifetime += deltaTime;

    if (m_pPlayer) {
        m_targetPosition = m_pPlayer->GetPosition();
    }

    if (m_type == ParticleType::BlockPickup) {
        if (m_lifetime < m_burstDuration) {
            //initial burst movement
            m_position += m_velocity;
            m_velocity *= 0.92f; //friction
        }
        else { //converge on player
            if (!m_seekingTarget) {
                m_seekingTarget = true;
            }

            //calculate direction to target
            Vector2 toTarget = m_targetPosition - m_position;


            float length = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

            //deactivate if close enough
            float deactivateDistance = 150.0f;
            if (length < deactivateDistance) {
                m_active = false;
                return;
            }

            if (length > 0.0f) {
                Vector2 direction;
                direction.x = toTarget.x / length;
                direction.y = toTarget.y / length;

                float minDistance = 0.1f;
                float distanceFactor = std::max(length, minDistance);
                float acceleration = 3000.0f;
                m_velocity.x += direction.x * acceleration * deltaTime;
                m_velocity.y += direction.y * acceleration * deltaTime;


                m_position.x += m_velocity.x * deltaTime;
                m_position.y += m_velocity.y * deltaTime;
            }


            if (m_lifetime > m_maxLifetime) {
                m_active = false;
            }
        }
    }
    else if (m_type == ParticleType::DigDirt) {
        m_velocity.y += m_gravity * deltaTime;
        m_velocity.x *= 0.99f;
        Box pticle(m_position.x + GridState::GetInstance().GetTileSize() / 2, m_position.y, 1.0f, GridState::GetInstance().GetTileSize() / 2);

        if (!GridState::GetInstance().CheckCollision(pticle)) {
            m_position += m_velocity * deltaTime;
        }


        if (m_lifetime > m_maxLifetime) {
            m_active = false;
        }
    }    
    else if (m_type == ParticleType::BlockBreak) {
        m_velocity.y += m_gravity * deltaTime;
        m_velocity.x *= 0.992f;
        Box pticle(m_position.x + GridState::GetInstance().GetTileSize() / 2, m_position.y, 1.0f, GridState::GetInstance().GetTileSize() / 2);

        if (!GridState::GetInstance().CheckCollision(pticle)) {
            m_position += m_velocity * deltaTime;
        }



        if (m_lifetime > m_maxLifetime) {
            m_active = false;
        }
    }
    else if (m_type == ParticleType::WaterDrop) {
        m_velocity.y += m_gravity * deltaTime * 3.5f;
        m_velocity.x *= 0.992f;

        Box pticle(m_position.x + GridState::GetInstance().GetTileSize() / 2, m_position.y, 1.0f, GridState::GetInstance().GetTileSize() / 2);

        if (!GridState::GetInstance().CheckCollision(pticle)) {
            m_position += m_velocity * deltaTime;
        }
        else {
            m_velocity.y = 0.0f;
            m_pSprite->SetAlpha(m_pSprite->GetAlpha() * 0.995f);
        }

        if (m_lifetime > m_maxLifetime) {
            m_active = false;
        }
    }
    else {
        m_position += m_velocity * deltaTime;
        if (m_lifetime > m_maxLifetime) {
            m_active = false;
        }
    }

}

void Particle::Draw(Renderer& renderer) {
    if (!m_active || !m_pSprite) return;
    m_pSprite->SetX(static_cast<int>(m_position.x));
    m_pSprite->SetY(static_cast<int>(m_position.y));
    m_pSprite->Draw(renderer);
}

bool Particle::IsAlive() const {
    return m_active;
}

void Particle::SetTargetPosition(Vector2 target) {
    m_targetPosition = target;
}
void Particle::SetPlayer(Player* player) {
    m_pPlayer = player;
}