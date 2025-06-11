// This include
#include "scenecontrollercontrols.h"

// Locals
#include "renderer.h"
#include "texture.h"
#include "sprite.h"
#include "inputsystem.h"
#include "game.h"
#include "font.h"
#include "vector2.h"
#include "logmanager.h"

// IMGUI
#include "imgui/imgui.h"

const int CONTROLS_MENU_SCENE_INDEX = 8;

SceneControllerControls::SceneControllerControls()
    : m_pBackBtnTexture(nullptr)
    , m_pBackBtnSprite(nullptr)
    , m_pBackgroundSprite(nullptr)
    , m_pTextFont(nullptr)
    , m_bisMouseOverBack(false)
    , m_screenWidth(0.f)
    , m_screenHeight(0.f)
{
}

SceneControllerControls::~SceneControllerControls()
{
    delete m_pBackBtnSprite; 
    m_pBackBtnSprite = nullptr;
    delete m_pBackBtnTexture; 
    m_pBackBtnTexture = nullptr;
    delete m_pBackgroundSprite; 
    m_pBackgroundSprite = nullptr;
    delete m_pTextFont; 
    m_pTextFont = nullptr;

    for (auto& sprite : m_controlSprites) { delete sprite; }
    m_controlSprites.clear();
    for (auto& texture : m_controlTextures) { delete texture; }
    m_controlTextures.clear();

    m_pSoundSystem.Release();
}

bool SceneControllerControls::Initialise(Renderer& renderer)
{
    m_pSoundSystem.Initialise();
    m_pSoundSystem.LoadSound("click", "../assets/sound/Button_Click.mp3");

    m_screenWidth = static_cast<float>(renderer.GetWidth());
    m_screenHeight = static_cast<float>(renderer.GetHeight());

    // Background
    m_pBackgroundSprite = renderer.CreateSprite("../assets/pauseBackground.png");
    m_pBackgroundSprite->SetX(static_cast<int>(m_screenWidth / 2));
    m_pBackgroundSprite->SetY(static_cast<int>(m_screenHeight / 2));
    float bgScaleX = m_screenWidth / m_pBackgroundSprite->GetWidth();
    float bgScaleY = m_screenHeight / m_pBackgroundSprite->GetHeight();
    m_pBackgroundSprite->SetScale(std::max(bgScaleX, bgScaleY));

    // Font
    m_pTextFont = new Font("../assets/fonts/joystix.otf", 28);
    SDL_Color textColor = { 255, 255, 255, 255 };

    // Lines
    const int numControlLines = 8;
    const int spacingY = 50;
    const int totalBlockHeight = numControlLines * spacingY;

    int startY = static_cast<int>((m_screenHeight / 2.0f) - (totalBlockHeight / 2.0f));

    // Text only
    CreateControlLineWithText(renderer, *m_pTextFont, textColor, "Movement:", "Left Joystick", startY, m_controlSprites, m_controlTextures);
    CreateControlLineWithText(renderer, *m_pTextFont, textColor, "Mine Up:", "Right Joystick Up", startY + spacingY * 2, m_controlSprites, m_controlTextures);
    CreateControlLineWithText(renderer, *m_pTextFont, textColor, "Mine Down:", "Right Joystick Down", startY + spacingY * 3, m_controlSprites, m_controlTextures);
    CreateControlLineWithText(renderer, *m_pTextFont, textColor, "Mine Left:", "Right Joystick Left", startY + spacingY * 4, m_controlSprites, m_controlTextures);
    CreateControlLineWithText(renderer, *m_pTextFont, textColor, "Mine Right:", "Right Joystick Right", startY + spacingY * 5, m_controlSprites, m_controlTextures);

    // With icons
    CreateControlLine(renderer, *m_pTextFont, textColor, "Jump: ", { "../assets/controlsSprite/A_Button.png" }, startY + spacingY, m_controlSprites, m_controlTextures);
    CreateControlLine(renderer, *m_pTextFont, textColor, "Interact: ", { "../assets/controlsSprite/X_Button.png" }, startY + spacingY * 6, m_controlSprites, m_controlTextures);
    CreateControlLine(renderer, *m_pTextFont, textColor, "Pause: ", { "../assets/controlsSprite/Start_XboxButton.png" }, startY + spacingY * 7, m_controlSprites, m_controlTextures);

    // Back Button
    m_pBackBtnTexture = new Texture();
    m_pBackBtnTexture->LoadTextTexture(renderer.GetSDLRenderer(), "Back", *m_pTextFont, textColor);
    m_pBackBtnSprite = new Sprite();
    m_pBackBtnSprite->Initialise(*m_pBackBtnTexture);
    m_pBackBtnSprite->SetX(m_pBackBtnTexture->GetWidth());
    m_pBackBtnSprite->SetY(static_cast<int>(m_screenHeight - 30));

    return true;
}

void SceneControllerControls::Process(float deltaTime, InputSystem& inputSystem)
{
    m_bisMouseOverBack = IsMouseOverSprite(inputSystem, m_pBackBtnSprite);
    if (m_bisMouseOverBack)
    {
        m_pBackBtnSprite->SetRedTint(m_hoverRed); m_pBackBtnSprite->SetGreenTint(m_hoverGreen); m_pBackBtnSprite->SetBlueTint(m_hoverBlue);
        if (inputSystem.GetMouseButtonState(SDL_BUTTON_LEFT) == BS_PRESSED)
        {
            m_pSoundSystem.PlaySound("click");
            Game::GetInstance().SetCurrentScene(CONTROLS_MENU_SCENE_INDEX);
        }
    }
    else
    {
        m_pBackBtnSprite->SetRedTint(m_defaultRed); m_pBackBtnSprite->SetGreenTint(m_defaultGreen); m_pBackBtnSprite->SetBlueTint(m_defaultBlue);
    }
    m_pSoundSystem.Update();
}

void SceneControllerControls::Draw(Renderer& renderer)
{
    renderer.SetCameraPosition(m_screenWidth / 2.0f, m_screenHeight / 2.0f);
    renderer.SetZoom(1.0f);
    if (m_pBackgroundSprite) m_pBackgroundSprite->Draw(renderer);

    for (auto& sprite : m_controlSprites)
    {
        sprite->Draw(renderer);
    }

    if (m_pBackBtnSprite) m_pBackBtnSprite->Draw(renderer);
}

void SceneControllerControls::DebugDraw()
{
    ImGui::Text("Scene: Controller Controls");
}

// With icons
void SceneControllerControls::CreateControlLine(Renderer& renderer, Font& font, SDL_Color color, const char* text, const std::vector<const char*>& spriteFiles, int yPos, std::vector<Sprite*>& outSprites, std::vector<Texture*>& outTextures)
{
    int textPadding = 20;
    int keySpacing = 10;
    int startX = static_cast<int>(m_screenWidth / 2) - 250;

    // Create text label
    Texture* textTexture = new Texture();
    textTexture->LoadTextTexture(renderer.GetSDLRenderer(), text, font, color);
    Sprite* textSprite = new Sprite();
    textSprite->Initialise(*textTexture);
    textSprite->SetX(startX + textTexture->GetWidth() / 2);
    textSprite->SetY(yPos);
    outTextures.push_back(textTexture);
    outSprites.push_back(textSprite);

    int currentX = startX + textTexture->GetWidth() + textPadding;

    // Create key sprites
    for (const char* file : spriteFiles)
    {
        Sprite* keySprite = renderer.CreateSprite(file);
        keySprite->SetX(currentX + keySprite->GetWidth() / 2);
        keySprite->SetY(yPos);
        outSprites.push_back(keySprite);
        currentX += keySprite->GetWidth() + keySpacing;
    }
}

// For pure text lines
void SceneControllerControls::CreateControlLineWithText(Renderer& renderer, Font& font, SDL_Color color, const char* labelText, const char* descriptionText, int yPos, std::vector<Sprite*>& outSprites, std::vector<Texture*>& outTextures)
{
    int textPadding = 20;
    int startX = static_cast<int>(m_screenWidth / 2) - 250;

    // Create label text sprite
    Texture* labelTexture = new Texture();
    labelTexture->LoadTextTexture(renderer.GetSDLRenderer(), labelText, font, color);
    Sprite* labelSprite = new Sprite();
    labelSprite->Initialise(*labelTexture);
    labelSprite->SetX(startX + labelTexture->GetWidth() / 2);
    labelSprite->SetY(yPos);
    outTextures.push_back(labelTexture);
    outSprites.push_back(labelSprite);

    int currentX = startX + labelTexture->GetWidth() + textPadding;

    // Create description text sprite
    Texture* descTexture = new Texture();
    descTexture->LoadTextTexture(renderer.GetSDLRenderer(), descriptionText, font, color);
    Sprite* descSprite = new Sprite();
    descSprite->Initialise(*descTexture);
    descSprite->SetX(currentX + descTexture->GetWidth() / 2);
    descSprite->SetY(yPos);
    outTextures.push_back(descTexture);
    outSprites.push_back(descSprite);
}

bool SceneControllerControls::IsMouseOverSprite(InputSystem& inputSystem, Sprite* sprite)
{
    if (!sprite) return false;
    const Vector2& mousePos = inputSystem.GetMousePosition();
    int mouseX = static_cast<int>(mousePos.x);
    int mouseY = static_cast<int>(mousePos.y);
    int spriteX = sprite->GetX();
    int spriteY = sprite->GetY();
    int spriteWidth = sprite->GetWidth();
    int spriteHeight = sprite->GetHeight();
    float halfWidth = static_cast<float>(spriteWidth) / 2.0f;
    float halfHeight = static_cast<float>(spriteHeight) / 2.0f;
    return (mouseX >= spriteX - halfWidth &&
        mouseX <= spriteX + halfWidth &&
        mouseY >= spriteY - halfHeight &&
        mouseY <= spriteY + halfHeight);
}