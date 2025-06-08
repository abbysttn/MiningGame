#ifndef __UPGRADESTATION_H__
#define __UPGRADESTATION_H__

#include "vector2.h"

class UpgradeStation 
{
public:
    Vector2 m_position;
    float m_interactionRadius;
    bool m_isActive;

    UpgradeStation(float x, float y, float radius)
        : m_interactionRadius(radius), m_isActive(true) 
    {
        m_position.Set(x, y);
    }

    bool IsPlayerInRange(const Vector2& playerPosition) const 
    {
        if (!m_isActive) return false;

        float dx = playerPosition.x - m_position.x;
        float dy = playerPosition.y - m_position.y;
        
        float distSq = (dx * dx) + (dy * dy);
        return distSq <= (m_interactionRadius * m_interactionRadius);
    }
};

#endif // !__UPGRADESTATION_H__