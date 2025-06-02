#include "particlesystem.h"

void ParticleSystem::Initialise(Sprite* sprite, Player* player, int maxParticles, ParticleType type) {
    m_pSprite = sprite;
    m_particles.clear();
    for (int i = 0; i < maxParticles; ++i) {
        Particle particle;
        particle.Initialise(sprite, type);
        particle.SetPlayer(player);
        m_particles.push_back(particle);
    }
}

void ParticleSystem::ActivateAt(Vector2 position) {
    for (Particle& particle : m_particles) {
        if (!particle.IsAlive()) {
            particle.Activate(position);
        }
    }
}

void ParticleSystem::Update(float deltaTime) {
    for (Particle& particle : m_particles) {
        particle.Update(deltaTime);
    }
}

void ParticleSystem::Draw(Renderer& renderer) {
    for (Particle& particle : m_particles) {
        particle.Draw(renderer);
    }
}

bool ParticleSystem::IsFinished() const {
    for (const Particle& particle : m_particles) {
        if (particle.IsAlive()) return false;
    }
    return true;
}