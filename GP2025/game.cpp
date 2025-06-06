// COMP710 GP Framework 2022
// This include:
#include "game.h"

// Library includes:
#include "renderer.h"
#include "logmanager.h"
#include "sprite.h"
#include <iostream>
#include "scenecheckerboards.h"
#include "scenemain.h"
#include "imgui/imgui_impl_sdl2.h"
#include "inputsystem.h"
#include "xboxcontroller.h"
#include "fmod.hpp"

// Scenes
#include "scenetitlescreen.h"
#include "SceneSplashScreenAUT.h"
#include "SceneSplashScreenFMOD.h"

// Static Members:
Game* Game::sm_pInstance = 0;
FMOD::System* m_pFMODSystem = nullptr;

Game& Game::GetInstance()
{
	if (sm_pInstance == 0)
	{
		sm_pInstance = new Game();
	}

	return (*sm_pInstance);
}

void Game::DestroyInstance()
{
	delete sm_pInstance;
	sm_pInstance = 0;
}

Game::Game() 
	: m_pRenderer(0)
	, m_bLooping(true)
	, m_iLastTime(0)
	, m_fExecutionTime(0.0f)
	, m_fElapsedSeconds(0.0f)
	, m_iFrameCount(0)
	, m_iFPS(0)
	, m_iCurrentScene(0)
	, m_pInputSystem(nullptr)
	, m_bShowDebugWindow(false)
	, m_pFMODSystem(nullptr)
	, m_pSound(nullptr)
	, m_pChannel(nullptr)
	, bbWidth(0.0f)
	, bbHeight(0.0f)
{

}

Game::~Game()
{
	for (Scene*& scene : m_scenes)
	{
		delete scene;
		scene = nullptr;
	}
	m_scenes.clear();

	delete m_pInputSystem;
	m_pInputSystem = nullptr;

	delete m_pRenderer;
	m_pRenderer = nullptr;
}

void Game::Quit()
{
	m_bLooping = false;
}

bool Game::Initialise()
{
	int bbWidth = 0;
	int bbHeight = 0;

	m_pRenderer = new Renderer();
	if (!m_pRenderer->Initialise(false, bbWidth, bbHeight))
	{
		LogManager::GetInstance().Log("Renderer failed to initialise!");
		return false;
	}

	bbWidth = m_pRenderer->GetWidth();
	bbHeight = m_pRenderer->GetHeight();

	m_iLastTime = SDL_GetPerformanceCounter();
	m_pRenderer->SetClearColour(0, 0, 0);

	// Initialise FMOD
	FMOD::System_Create(&m_pFMODSystem);
	m_pFMODSystem->init(512, FMOD_INIT_NORMAL, 0);

	// Initialise Input System
	m_pInputSystem= new InputSystem();
	if (!m_pInputSystem->Initialise())
	{
		LogManager::GetInstance().Log("InputSystem failed to initialise!");
		return false;
	}

	// SPlash screens
	Scene* pSplashSceneAUT = new SceneSplashScreenAUT();
	if (!pSplashSceneAUT->Initialise(*m_pRenderer))
	{
		LogManager::GetInstance().Log("AUT Splash screen failed to load!");
		delete pSplashSceneAUT;
		return false;
	}
	m_scenes.push_back(pSplashSceneAUT);
	
	Scene* pSplashSceneFMOD = new SceneSplashScreenFMOD();
	if (!pSplashSceneFMOD->Initialise(*m_pRenderer))
	{
		LogManager::GetInstance().Log("FMOD Splash screen failed to load!");
		delete pSplashSceneFMOD;
		delete pSplashSceneAUT;
		m_scenes.clear();
		return false;
	}
	m_scenes.push_back(pSplashSceneFMOD);

	// Titlescreen stuffz
	Scene* pTitleScene = new SceneTitlescreen(m_pFMODSystem);
	if (!pTitleScene->Initialise(*m_pRenderer))
	{
		LogManager::GetInstance().Log("Titlescreen fialed to load!!");
		delete pSplashSceneFMOD;
		delete pSplashSceneAUT;
		delete pTitleScene;
		m_scenes.clear();
		return false;
	}
	m_scenes.push_back(pTitleScene);

	Scene* pMainScene = new SceneMain();
	if (!pMainScene->Initialise(*m_pRenderer))
	{
		// Debugging stuff
		LogManager::GetInstance().Log("TItle screne failed to load!");
		delete pSplashSceneFMOD;
		delete pSplashSceneAUT;
		delete pTitleScene;
		delete pMainScene;
		m_scenes.clear();
		return false;
	}
	m_scenes.push_back(pMainScene);

	m_iCurrentScene = 0;
	SetCurrentScene(m_iCurrentScene);

	return true;
}

bool Game::DoGameLoop()
{
	const float stepSize = 1.0f / 60.0f;

	// Process input 
	m_pInputSystem->ProcessInput();

	if (m_bLooping)
	{
		Uint64 current = SDL_GetPerformanceCounter();
		float deltaTime = (current - m_iLastTime) / static_cast<float>(SDL_GetPerformanceFrequency());

		m_iLastTime = current;

		m_fExecutionTime += deltaTime;

		Process(deltaTime);

#ifdef USE_LAG
		m_fLag += deltaTime;

		int innerLag = 0;

		while (m_fLag >= stepSize)
		{
			Process(stepSize);

			m_fLag -= stepSize;

			++m_iUpdateCount;
			++innerLag;
		}
#endif //USE_LAG

		Draw(*m_pRenderer);
		//frame limiting
		Uint64 frameEnd = SDL_GetPerformanceCounter();
		float actualFrameTime = (frameEnd - current) / static_cast<float>(SDL_GetPerformanceFrequency());
		float sleepTime = targetFrameTime - actualFrameTime;

		if (sleepTime > 0.0f)
		{
			SDL_Delay(static_cast<Uint32>(sleepTime * 1000.0f));
		}
	}

	return m_bLooping;
}

void Game::Process(float deltaTime)
{
	ProcessFrameCounting(deltaTime);

	m_scenes[m_iCurrentScene]->Process(deltaTime, *m_pInputSystem);

	// Toggle Debug Window
	ButtonState backspaceState = m_pInputSystem->GetKeyState(SDL_SCANCODE_BACKSPACE);
	if (backspaceState == BS_PRESSED)
	{
		std::cout << "Backspace pressed" << std::endl;
		ToggleDebugWindow();
	}

	/*
	* Scenes Order
	* AUT Splash = 0
	* FMOD Splash = 1
	* Title screen = 2
	* (Instructions scene)
	* (Loading maybe) 
	* Main Scene = 3
	*/

	if (m_iCurrentScene == 0)
	{
		SceneSplashScreenAUT* autSplash = dynamic_cast<SceneSplashScreenAUT*>(m_scenes[m_iCurrentScene]);
		if (autSplash && autSplash->IsFinished())
		{
			SetCurrentScene(1); // Move to FMOD splash screen
		}
	}

	else if (m_iCurrentScene == 1)
	{
		SceneSplashScreenFMOD* fmodSplash = dynamic_cast<SceneSplashScreenFMOD*>(m_scenes[m_iCurrentScene]);
		if (fmodSplash && fmodSplash->IsFinished())
		{
			SetCurrentScene(2); // Move to title screen
		}
	}

	// loading screen?

}

void Game::Draw(Renderer& renderer)
{

	++m_iFrameCount;

	renderer.Clear();

	//Draw Current Scene
	m_scenes[m_iCurrentScene]->Draw(renderer);

	DebugDraw();

	renderer.Present();
}

void
Game::ProcessFrameCounting(float deltaTime)
{
	// Count total simulation time elapsed:
	m_fElapsedSeconds += deltaTime;

	// Frame Counter:
	if (m_fElapsedSeconds > 1.0f)
	{
		m_fElapsedSeconds -= 1.0f;
		m_iFPS = m_iFrameCount;
		m_iFrameCount = 0;
	}
}

void Game::DebugDraw
()
{
	if (m_bShowDebugWindow) {
		bool open = true;

		ImGui::Begin("Debug Window", &open, ImGuiWindowFlags_MenuBar);

		ImGui::Text("COMP710 GP Framework (%s)", "2025, S1");

		if (ImGui::Button("Quit"))
		{
			Quit();
		}

		LogManager::GetInstance().DebugDraw();

		ImGui::SliderInt("Active scene", &m_iCurrentScene, 0, m_scenes.size() - 1, "%d");

		m_scenes[m_iCurrentScene]->DebugDraw();

		ImGui::End();
	}
}

void Game::ToggleDebugWindow
()
{
	std::cout << "Toggle Debug Window" << std::endl;
	m_bShowDebugWindow = !m_bShowDebugWindow;
	m_pInputSystem->ShowMouseCursor(m_bShowDebugWindow);
}

void Game::SetCurrentScene(int sceneIndex)
{
	if (sceneIndex >= 0 && sceneIndex < static_cast<int>(m_scenes.size()))
	{
		m_scenes[m_iCurrentScene]->OnExit();
		m_iCurrentScene = sceneIndex;
		m_scenes[m_iCurrentScene]->OnEnter();


		SceneMain* mainScene = dynamic_cast<SceneMain*>(m_scenes[m_iCurrentScene]);
		m_pRenderer->SetSceneMain(mainScene);

		if (m_pInputSystem)
		{
			if (m_iCurrentScene == 2)
			{
				m_pInputSystem->ShowMouseCursor(true);
				m_pInputSystem->SetRelativeMode(false);
			}
			else
			{
				m_pInputSystem->ShowMouseCursor(m_bShowDebugWindow);
				m_pInputSystem->SetRelativeMode(false);
			}
		}
	}
}
