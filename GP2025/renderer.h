// COMP710 GP Framework 2022
#ifndef RENDERER_H
#define RENDERER_H

// Forward Declarations:
class TextureManager;
class Shader;
class VertexArray;
class Sprite;
class AnimatedSprite;
class Texture;

class SceneMain;

struct SDL_Window;

// Library includes:
#include <SDL.h>
#include "texture.h"
#include "vector2.h"

class Renderer
{
	// Member methods:
public:
	Renderer();
	~Renderer();

	bool Initialise(bool windowed, int width = 0, int height = 0);

	void Clear();
	void Present();

	void SetClearColour(unsigned char r, unsigned char g, unsigned char b);
	void GetClearColour(unsigned char& r, unsigned char& g, unsigned char& b);

	int GetWidth() const;

	int GetHeight() const;
	int GetWorldHeight();

	Sprite* CreateSprite(const char* pcFilename);
	Sprite* CreateTextSprite(Texture* pTexture);
	void DrawSprite(Sprite& sprite);

	void DrawSpriteScreenSpace(Sprite& sprite);

	AnimatedSprite* CreateAnimatedSprite(const char* pcFilename);
	void DrawAnimatedSprite(AnimatedSprite& sprite, int frame, bool flipHorizontal);

	void SetCameraPosition(float x, float y);
	
	void DrawRectScreenSpace(const Vector2& position, const Vector2& size, float r, float g, float b, float a);
	GLuint CreateWhiteTexture();
	
	void SetZoom(float zoom) { m_zoom = zoom; }
	void SetSceneMain(SceneMain* scene);



	SDL_Renderer* GetSDLRenderer() const { return m_pSDLRenderer; }



protected:
	bool InitialiseOpenGL(int screenWidth, int screenHeight);
	void SetFullscreen(bool fullscreen);

	void LogSdlError();

	bool SetupSpriteShader();

private:
	Renderer(const Renderer& renderer);
	//Renderer& operator=(const Renderer& renderer);

	// Member data:
public:

protected:
	TextureManager* m_pTextureManager;
	SDL_Window* m_pWindow;
	SDL_GLContext m_glContext;
	SDL_Renderer* m_pSDLRenderer;

	SceneMain* m_sceneMain;
	Shader* m_pSpriteShader;
	VertexArray* m_pSpriteVertexData;
	int m_iWidth;
	int m_iHeight;

	float m_fClearRed;
	float m_fClearGreen;
	float m_fClearBlue;

	Texture* m_pWhiteTexture;

private:
	float m_zoom = 1.0f;
	float m_cameraX = 0.0f;
	float m_cameraY = 0.0f;
};

#endif // RENDERER_H