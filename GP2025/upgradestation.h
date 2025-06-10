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

    bool IsPlayerInRange(const Vector2& playerPosition, float bgWidth, float bgHeight, float rendererHeight) const
    {
        if (!m_isActive) return false;

        float leftBoundary = bgWidth * 0.2f;
        float topBoundary = rendererHeight * 0.74f;


        return playerPosition.x <= leftBoundary && playerPosition.y <= topBoundary;

    }
};

#endif // !__UPGRADESTATION_H__