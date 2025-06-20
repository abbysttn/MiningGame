// COMP710 GP Framework 2022

// This include:
#include "renderer.h"

// Local includes:
#include "texturemanager.h"
#include "logmanager.h"
#include "shader.h"
#include "vertexarray.h"
#include "sprite.h"
#include "matrix4.h"
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
#include "animatedsprite.h"
#include "scenemain.h"

#include "texture.h"

// Library includes:
#include <SDL.h>
#include <SDL_image.h>
#include <glew.h>
#include <cassert>
#include <cmath>

Renderer::Renderer()
	: m_pTextureManager(0)
	, m_pSpriteShader(0)
	, m_pSpriteVertexData(0)
	, m_glContext(0)
	, m_iWidth(0)
	, m_iHeight(0)
	, m_fClearRed(0.0f)
	, m_fClearGreen(0.0f)
	, m_fClearBlue(0.0f)
{

}

Renderer::~Renderer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	delete m_pSpriteShader;
	m_pSpriteShader = 0;

	delete m_pSpriteVertexData;
	m_pSpriteVertexData = 0;

	delete m_pTextureManager;
	m_pTextureManager = 0;

	delete m_pWhiteTexture;
	m_pWhiteTexture = nullptr;

	if (m_pSDLRenderer)
	{
		SDL_DestroyRenderer(m_pSDLRenderer);
		m_pSDLRenderer = nullptr;
	}

	SDL_DestroyWindow(m_pWindow);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

bool Renderer::Initialise(bool windowed, int width, int height)
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LogSdlError();
		return false;
	}
	TTF_Init();

	if (!windowed)
	{
		// Go fullscreen, with current resolution!
		int numDisplays = SDL_GetNumVideoDisplays();
		SDL_DisplayMode* currentDisplayMode = new SDL_DisplayMode[numDisplays];

		for (int k = 0; k < numDisplays; ++k)
		{
			int result = SDL_GetCurrentDisplayMode(k, &currentDisplayMode[k]);
		}

		// Use the widest display?
		int widest = 0;
		int andItsHeight = 0;

		for (int k = 0; k < numDisplays; ++k)
		{
			if (currentDisplayMode[k].w > widest)
			{
				widest = currentDisplayMode[k].w;
				andItsHeight = currentDisplayMode[k].h;
			}
		}

		delete[] currentDisplayMode;
		currentDisplayMode = 0;

		width = widest;
		height = andItsHeight;
	}

	bool initialised = InitialiseOpenGL(width, height);

	SetFullscreen(!windowed);

	if (initialised)
	{
		m_pTextureManager = new TextureManager();
		assert(m_pTextureManager);
		initialised = m_pTextureManager->Initialise();
	}

	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(m_pWindow, m_glContext);
	ImGui_ImplOpenGL3_Init();
	ImGui::SetMouseCursor(ImGuiMouseCursor_None);

	// Creating SDL Renderer for Text
	m_pSDLRenderer = SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (!m_pSDLRenderer)
	{
		LogSdlError();
		return false;
	}

	// Creating plain white pixel for rectangle functions
	m_pWhiteTexture = new Texture();
	m_pWhiteTexture->SetID(CreateWhiteTexture());

	return initialised;
}

bool Renderer::InitialiseOpenGL(int screenWidth, int screenHeight)
{
	m_iWidth = screenWidth;
	m_iHeight = screenHeight;

	m_pWindow = SDL_CreateWindow("COMP710 GP Framework 2025", SDL_WINDOWPOS_UNDEFINED,
						     	  SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);

	m_glContext = SDL_GL_CreateContext(m_pWindow);

	GLenum glewResult = glewInit();

	if (glewResult != GLEW_OK)
	{
		return false;
	}

	// Disable VSYNC
	SDL_GL_SetSwapInterval(0);

	bool shadersLoaded = SetupSpriteShader();

	return shadersLoaded;
}

void Renderer::Clear()
{
	glClearColor(m_fClearRed, m_fClearGreen, m_fClearBlue, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void Renderer::Present()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_GL_SwapWindow(m_pWindow);
}

void Renderer::SetFullscreen(bool fullscreen)
{
	if (fullscreen)
	{
		// SDL_SetWindowFullscreen(m_pWindow, SDL_WINDOW_FULLSCREEN | SDL_WINDOW_ALWAYS_ON_TOP);
		SDL_SetWindowFullscreen(m_pWindow, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_ALWAYS_ON_TOP);
		SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
		SDL_SetWindowSize(m_pWindow, m_iWidth, m_iHeight);
	}

	else
	{
		SDL_SetWindowFullscreen(m_pWindow, 0);
	}
}

void Renderer::SetClearColour(unsigned char r, unsigned char g, unsigned char b)
{
	m_fClearRed = r / 255.0f;
	m_fClearGreen = g / 255.0f;
	m_fClearBlue = b / 255.0f;
}

void Renderer::GetClearColour(unsigned char& r, unsigned char& g, unsigned char& b)
{
	r = static_cast<unsigned char>(m_fClearRed * 255.0f);
	g = static_cast<unsigned char>(m_fClearGreen * 255.0f);
	b = static_cast<unsigned char>(m_fClearBlue * 255.0f);
}

int Renderer::GetWidth() const
{
	return m_iWidth;
}

int Renderer::GetHeight() const
{
	return m_iHeight;
}
void Renderer::SetSceneMain(SceneMain* scene)
{
	m_sceneMain = scene;
}
int Renderer::GetWorldHeight() {
	return m_sceneMain->GetBackgroundHeight();

}

Sprite* Renderer::CreateSprite(const char* pcFilename)
{
	assert(m_pTextureManager);

	Texture* pTexture = m_pTextureManager->GetTexture(pcFilename);

	Sprite* pSprite = new Sprite();
	if (!pSprite->Initialise(*pTexture))
	{
		LogManager::GetInstance().Log("Sprite Failed to Create!");
	}

	return (pSprite);
}

Sprite* Renderer::CreateTextSprite(Texture* pTexture)
{
	if (!pTexture)
	{
		LogManager::GetInstance().Log("CreateTextSprite: Null texture passed");
		return nullptr;
	}

	Sprite* pSprite = new Sprite();
	if (!pSprite->Initialise(*pTexture))
	{
		LogManager::GetInstance().Log("CreateTextSprite: Failed to initialize sprite with texture");
		delete pSprite;
		return nullptr;
	}

	return pSprite;
}

void Renderer::LogSdlError()
{
	LogManager::GetInstance().Log(SDL_GetError());
}

bool Renderer::SetupSpriteShader()
{
	m_pSpriteShader = new Shader();

	bool loaded = m_pSpriteShader->Load("shaders\\sprite.vert", "shaders\\sprite.frag");

	m_pSpriteShader->SetActive();

	float vertices[] =
	{
	-0.5f, 0.5f, 0.0f, 0.0f, 0.0f, // Top left
	0.5f, 0.5f, 0.0f, 1.0f, 0.0f, // Top right
	0.5f, -0.5f, 0.0f, 1.0f, 1.0f, // Bottom right
	-0.5f, -0.5f, 0.0f, 0.0f, 1.0f // Bottom left
	};

	unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

	m_pSpriteVertexData = new VertexArray(vertices, 4, indices, 6);

	return loaded;
}

void Renderer::DrawSprite(Sprite& sprite)
{
	m_pSpriteShader->SetActive();
	m_pSpriteVertexData->SetActive();

	float angleInDegrees = sprite.GetAngle();
	float sizeX = static_cast<float>(sprite.GetWidth());
	float sizeY = static_cast<float>(sprite.GetHeight());
	const float PI = 3.14159f;
	float angleInRadians = (angleInDegrees * PI) / 180.0f;

	Matrix4 world;
	SetIdentity(world);
	world.m[0][0] = cosf(angleInRadians) * sizeX;
	world.m[0][1] = sinf(angleInRadians) * -sizeY;
	world.m[1][0] = sinf(angleInRadians) * sizeX;
	world.m[1][1] = cosf(angleInRadians) * -sizeY;
	world.m[3][0] = static_cast<float>(sprite.GetX());
	world.m[3][1] = static_cast<float>(sprite.GetY());

	m_pSpriteShader->SetMatrixUniform("uWorldTransform", world);

	// CAMERA-AWARE VIEW-PROJECTION MATRIX
	Matrix4 ortho;
	CreateOrthoProjection(ortho, static_cast<float>(m_iWidth), static_cast<float>(m_iHeight));

	Matrix4 view;
	SetIdentity(view);
	view.m[0][0] = m_zoom;
	view.m[1][1] = m_zoom;
	view.m[3][0] = -m_cameraX * m_zoom + (m_iWidth / 2.0f);
	view.m[3][1] = -m_cameraY * m_zoom + (m_iHeight / 2.0f);

	Matrix4 viewProj = view * ortho;
	m_pSpriteShader->SetMatrixUniform("uViewProj", viewProj);

	m_pSpriteShader->SetVector4Uniform("colour",
		sprite.GetRedTint(),
		sprite.GetGreenTint(),
		sprite.GetBlueTint(),
		sprite.GetAlpha());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::DrawSpriteScreenSpace(Sprite& sprite)
{
	m_pSpriteShader->SetActive();
	m_pSpriteVertexData->SetActive();
	sprite.GetTexture()->SetActive();

	float sizeX = static_cast<float>(sprite.GetWidth()) * sprite.GetScale();
	float sizeY = static_cast<float>(sprite.GetHeight()) * sprite.GetScale();

	Matrix4 world;
	SetIdentity(world);
	world.m[0][0] = sizeX;
	world.m[1][1] = -sizeY;
	world.m[3][0] = static_cast<float>(sprite.GetX());
	world.m[3][1] = static_cast<float>(sprite.GetY());
	m_pSpriteShader->SetMatrixUniform("uWorldTransform", world);

	Matrix4 ortho;
	CreateOrthoProjection(ortho,
		static_cast<float>(m_iWidth),
		static_cast<float>(m_iHeight));

	Matrix4 view;
	SetIdentity(view);

	Matrix4 viewProj = view * ortho;
	m_pSpriteShader->SetMatrixUniform("uViewProj", viewProj);

	m_pSpriteShader->SetVector4Uniform("colour",
		sprite.GetRedTint(),
		sprite.GetGreenTint(),
		sprite.GetBlueTint(),
		sprite.GetAlpha());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


AnimatedSprite*
Renderer::CreateAnimatedSprite(const char* pcFilename)
{
	assert(m_pTextureManager);

	Texture* pTexture = m_pTextureManager->GetTexture(pcFilename);

	AnimatedSprite* pSprite = new AnimatedSprite();
	if (!pSprite->Initialise(*pTexture))
	{
		LogManager::GetInstance().Log("AnimatedSprite failed to create!");
	}

	return pSprite;
}

void
Renderer::DrawAnimatedSprite(AnimatedSprite& sprite, int frame, bool flipHorizontal)
{
	m_pSpriteShader->SetActive();
	float angleInDegrees = sprite.GetAngle();
	float sizeX = static_cast<float>(sprite.GetWidth());
	float sizeY = static_cast<float>(sprite.GetHeight());
	const float PI = 3.14159f;
	float angleInRadians = (angleInDegrees * PI) / 180.0f;
	Matrix4 world;
	SetIdentity(world);

	float scaleX = flipHorizontal ? -1.0f : 1.0f; 
	float scaleY = 1.0f; 

	world.m[0][0] = cosf(angleInRadians) * (scaleX * sizeX);
	world.m[0][1] = -sinf(angleInRadians) * (scaleY * sizeY);
	world.m[1][0] = sinf(angleInRadians) * (scaleX * sizeX);
	world.m[1][1] = cosf(angleInRadians) * (scaleY * sizeY);

	world.m[3][0] = static_cast<float>(sprite.GetX());
	world.m[3][1] = static_cast<float>(sprite.GetY());
	m_pSpriteShader->SetMatrixUniform("uWorldTransform", world);
	Matrix4 ortho;
	CreateOrthoProjection(ortho, static_cast<float>(m_iWidth), static_cast<float>(m_iHeight));

	// Create view matrix based on camera position and zoom
	Matrix4 view;
	SetIdentity(view);
	view.m[0][0] = m_zoom;
	view.m[1][1] = m_zoom;
	view.m[3][0] = -m_cameraX * m_zoom + (m_iWidth / 2.0f);
	view.m[3][1] = -m_cameraY * m_zoom + (m_iHeight / 2.0f);

	Matrix4 viewProj = view * ortho;
	m_pSpriteShader->SetMatrixUniform("uViewProj", viewProj);

	m_pSpriteShader->SetVector4Uniform("colour", sprite.GetRedTint(),
		sprite.GetGreenTint(), sprite.GetBlueTint(), sprite.GetAlpha());

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)((frame * 6) * sizeof(GLuint)));
}

void 
Renderer::SetCameraPosition(float x, float y)
{
	m_cameraX = x;
	m_cameraY = y;
}

void Renderer::DrawRectScreenSpace(const Vector2& position, const Vector2& size, float r, float g, float b, float a)
{
	m_pSpriteShader->SetActive();
	m_pSpriteVertexData->SetActive();

	m_pWhiteTexture->SetActive();

	Matrix4 world;
	SetIdentity(world);
	world.m[0][0] = size.x;
	world.m[1][1] = -size.y;
	world.m[3][0] = position.x;
	world.m[3][1] = position.y;
	m_pSpriteShader->SetMatrixUniform("uWorldTransform", world);

	Matrix4 ortho;
	CreateOrthoProjection(ortho, (float)m_iWidth, (float)m_iHeight);

	Matrix4 view;
	SetIdentity(view);

	Matrix4 viewProj = view * ortho;
	m_pSpriteShader->SetMatrixUniform("uViewProj", viewProj);

	m_pSpriteShader->SetVector4Uniform("colour", r, g, b, a);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

GLuint Renderer::CreateWhiteTexture()
{
	GLuint texID;
	unsigned char whitePixel[4] = { 255, 255, 255, 255 }; // RGBA white pixel

	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, whitePixel);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	return texID;
}