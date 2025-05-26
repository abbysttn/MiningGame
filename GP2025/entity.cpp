// COMP710 GP Framework 2022
#include "entity.h"
#include "sprite.h"
#include "renderer.h"
#include <cmath>

Entity::Entity()
    : m_pSprite(nullptr)
    , m_position(0, 0)
    , m_velocity(0, 0)
    , m_bAlive(true)
{
}

Entity::~Entity()
{

    if (m_bOwnsSprite)
    {
        delete m_pSprite;
    }
    m_pSprite = nullptr;
}

bool Entity::Initialise(Renderer& renderer)
{
    return true;
}

void Entity::Process(float deltaTime)
{

    m_position += m_velocity * deltaTime;
}

void Entity::Draw(Renderer& renderer)
{
    if (m_pSprite != nullptr && m_bAlive)
    {
        m_pSprite->SetX(static_cast<int>(m_position.x));
        m_pSprite->SetY(static_cast<int>(m_position.y));
        m_pSprite->Draw(renderer);
    }
}

void Entity::Rotate(float direction)
{
    if (m_pSprite != nullptr)
    {
        m_pSprite->SetAngle(direction);
    }
}

bool Entity::IsAlive() const
{
    return m_bAlive;
}

void Entity::SetDead()
{
    m_bAlive = false;
}

float Entity::GetRadius()
{
    return (m_pSprite != nullptr) ? m_pSprite->GetWidth() / 2.0f : 0.0f;
}

Vector2 Entity::GetFacingDirection()
{
    // Assuming right-facing by default
    return Vector2(1, 0);
}

Vector2& Entity::GetPosition()
{
    return m_position;

}

Vector2& Entity::GetVelocity()
{
    return m_velocity;
}

bool Entity::IsCollidingWith(Entity& toCheck)
{
    float distance = (m_position - toCheck.GetPosition()).Length();
    float combinedRadius = GetRadius() + toCheck.GetRadius() + 40.0f;
    return distance < combinedRadius;
}