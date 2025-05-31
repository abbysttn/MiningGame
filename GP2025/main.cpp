// COMP710 GP Framework 2022

// Library includes:
#include <SDL.h>
#include <ctime>
#include <cstdlib>
#include <crtdbg.h>

// Local includes:
#include "game.h"
#include "logmanager.h"



int main(int argc, char* argv[])
{
	srand(static_cast<unsigned>(time(0)));
	Game& gameInstance = Game::GetInstance();

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(275);

	if (!gameInstance.Initialise())
	{
		LogManager::GetInstance().Log("Game initialise failed!");

		return 1;
	}

	while (gameInstance.DoGameLoop())
	{
		// No body.
	}

	Game::DestroyInstance();
	LogManager::DestroyInstance();

	return 0;
}