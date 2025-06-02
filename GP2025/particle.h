#ifndef PARTICLE_H
#define PARTICLE_H

#include "sprite.h"
#include "vector2.h"
#include "player.h"
#include "entity.h"

class Player;
enum class ParticleType {
    BlockPickup,
    BlockBreak,
    DigDirt
};
class Particle {
public:
    Particle();

    void Initialise(Sprite* sprite, ParticleType type = ParticleType::BlockPickup);
    void Activate(Vector2 position);
    void Update(float deltaTime);
    void Draw(Renderer& renderer);
    void SetTargetPosition(Vector2 target);
    bool IsAlive() const;
    void SetPlayer(Player* player);

private:
    Sprite* m_pSprite = nullptr;
    Vector2 m_position;
    Vector2 m_velocity;
    float m_lifetime = 0.0f;
    float m_maxLifetime = 1.0f;
    bool m_active = false;
    float m_speedMultiplier = 1.0f;
    float m_burstDuration = 2.0f;
    bool m_seekingTarget = false;
    Vector2 m_targetPosition;
    ParticleType m_type;
    Player* m_pPlayer;

    const float m_gravity = 600.0f;

};
#endif // PARTICLE_H