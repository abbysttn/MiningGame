#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include "inputsystem.h"
#include <cmath>

class Player : public Entity {

public:
	Player();
	~Player();

	bool Initialise(Renderer& renderer);
	void Process(float deltaTime, InputSystem& inputSystem);
	void Draw(Renderer& renderer);
	bool IsKeyHeld(InputSystem& input, SDL_Scancode key);

	Vector2 GetPosition() const { return m_position; };


private:
	float m_speed;
	Renderer* m_pRenderer;
	float m_heightMultiple = 3.0f; //The height multiple vs the height of the renderer. e.g. the renderer height is 1080, 3240 = 3*1080. NEEED to make this dynamic

	float m_heightMultiple = 3.0f;

};

#endif // PLAYER_H