#pragma once

#include "Editor/Panels/EditorPanel.h"

#include "Editor/Debug/EditorConsole.h"
#include <array>

namespace Engine
{
    class ConsolePanel : public EditorPanel
    {
    public:
        ConsolePanel();

        virtual void OnRender(float DeltaTime) override;

    private:
        void DrawConsole();

        void DrawFilterBar();

        void DrawInputBar();

        void ScrollToBottom() { bShouldScroll = true; }

    private:
        std::string Buffer;

        bool bShouldScroll = true;

        ImGuiTextFilter m_TextFilter;

        const std::vector<Logger::LogData>* LogList = nullptr;

        enum COLOR_PALETTE
        {
            COL_COMMAND = 0,    // Color for command logs
            COL_LOG,            // Color for in-command logs
            COL_WARNING,        // Color for warnings logs
            COL_ERROR,          // Color for error logs
            COL_INFO,           // Color for info logs

            COL_TIMESTAMP,      // Color for timestamps

            COL_COUNT
        };

        COLOR_PALETTE GetLogLevelColor(LogLevel level);

        std::array<ImVec4, COL_COUNT> ColorPalette;
    };
}