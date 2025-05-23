// COMP710 GP Framework 2022

#ifndef SCENECHECKERBOARDS_H
#define SCENECHECKERBOARDS_H

// Local includes:
#include "scene.h"
#include "animatedsprite.h"
#include "fmod.hpp"

// Forward declarations:
class Renderer;
class Sprite;

// Class declaration:
class SceneCheckerboards : public Scene
{
// Member methods:
public:
SceneCheckerboards(FMOD::System* pFMODSystem);
virtual ~SceneCheckerboards();

virtual bool Initialise(Renderer& renderer);
virtual void Draw(Renderer& renderer);
virtual void DebugDraw();
virtual void Process(float deltaTime, InputSystem& inputSystem); 

protected:

private:
SceneCheckerboards(const SceneCheckerboards& sceneCheckerboards) = delete;
SceneCheckerboards& operator=(const SceneCheckerboards& sceneCheckerboards) = delete;

// Member data:
public:

protected:
Sprite* m_pCorners[4];
Sprite* m_pCentre;

float m_angle;
float m_rotationSpeed;

private:
AnimatedSprite* m_pAnimatedSprite;

FMOD::System* m_pFMODSystem;
FMOD::Sound* m_pSound;
FMOD::Channel* m_pChannel;

};

#endif // SCENECHECKERBOARDS_H