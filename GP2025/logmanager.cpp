//COMP710 GP Framework

#include "logmanager.h"

#include <Windows.h>
#include "imgui/imgui.h"


LogManager* LogManager::sm_pInstance = 0;

LogManager& LogManager::GetInstance()
{
    if (sm_pInstance == 0) {
        sm_pInstance = new LogManager();
    }

    return (*sm_pInstance);
}

void LogManager::DestroyInstance() {
    delete sm_pInstance;
    sm_pInstance = 0;
}

void LogManager::DebugDraw()
{
    ImGui::Checkbox("Show Logging", &m_bShowLogMessages);
    if (m_bShowLogMessages)
    {
        ImGui::Text("Log Manager:");
        ImGui::BeginChild("Log", ImVec2(ImGui::GetWindowWidth(),
            200), true, ImGuiWindowFlags_HorizontalScrollbar);
        std::reverse(m_logHistory.begin(), m_logHistory.end());
        std::vector<std::string>::iterator iter = m_logHistory.begin();
        while (iter != m_logHistory.end())
        {
            ImGui::Text((*iter).c_str());
            ++iter;
        }
        std::reverse(m_logHistory.begin(), m_logHistory.end());
        ImGui::EndChild();
    }
}

LogManager::LogManager() {

}

LogManager::~LogManager() {

}

void LogManager::Log(const char* pcMessage) {
    OutputDebugStringA(pcMessage);
    OutputDebugStringA("\n");
    m_logHistory.push_back(pcMessage);
}