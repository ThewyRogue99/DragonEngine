#include "depch.h"
#include "ConsolePanel.h"

#include "Engine/Core/CommandSystem.h"

#include <imgui_internal.h>

#include "IconsFontAwesome6.h"

static constexpr size_t DefaultBufferSize = 2046;

namespace ImGui
{
    static int InputTextCallback(ImGuiInputTextCallbackData* data)
    {
        std::string* str = (std::string*)data->UserData;
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
        {
            // Resize string callback
            // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
            IM_ASSERT(data->Buf == str->c_str());
            str->resize(data->BufTextLen);
            data->Buf = (char*)str->c_str();
        }
        return 0;
    }

    bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags)
    {
        IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
        flags |= ImGuiInputTextFlags_CallbackResize;

        return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, str);
    }
}

namespace Engine
{
	ConsolePanel::ConsolePanel() : EditorPanel(ICON_FA_CODE "  Console")
	{
        Buffer.resize(DefaultBufferSize);

        // Style
        ColorPalette[COL_COMMAND] = ImVec4(1.f, 1.f, 1.f, 1.f);
        ColorPalette[COL_LOG] = ImVec4(1.f, 1.f, 1.f, 0.5f);
        ColorPalette[COL_WARNING] = ImVec4(1.0f, 0.87f, 0.37f, 1.f);
        ColorPalette[COL_ERROR] = ImVec4(1.f, 0.365f, 0.365f, 1.f);
        ColorPalette[COL_INFO] = ImVec4(0.46f, 0.96f, 0.46f, 1.f);
        ColorPalette[COL_TIMESTAMP] = ImVec4(1.f, 1.f, 1.f, 0.5f);
	}

    void ConsolePanel::OnCreate()
    {
        SetPanelStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5.f, 5.f));
    }

	void ConsolePanel::OnRender(float DeltaTime)
	{
        if (!LogList)
        {
            Ref<EditorConsole> AttachedConsole = std::dynamic_pointer_cast<EditorConsole>(Log::GetConsole());

            if (AttachedConsole)
                LogList = &(AttachedConsole->GetLogs());
        }

        DrawFilterBar();

        DrawConsole();

        ImGui::Separator();

        DrawInputBar();
	}

    ConsolePanel::COLOR_PALETTE ConsolePanel::GetLogLevelColor(LogLevel level)
    {
        switch (level)
        {
        case Engine::LogLevel::Command:
            return COL_COMMAND;
        case Engine::LogLevel::Log:
            return COL_LOG;
        case Engine::LogLevel::Warning:
            return COL_WARNING;
        case Engine::LogLevel::Error:
            return COL_ERROR;
        case Engine::LogLevel::Info:
            return COL_INFO;
        default:
            return COL_LOG;
        }
    }

    static const char* GetLogLevelName(LogLevel level)
    {
        switch (level)
        {
        case Engine::LogLevel::Command:
            return "Command";
        case Engine::LogLevel::Log:
            return "Log";
        case Engine::LogLevel::Warning:
            return "Warning";
        case Engine::LogLevel::Error:
            return "Error";
        case Engine::LogLevel::Info:
            return "Info";
        default:
            return nullptr;
        }
    }

    void ConsolePanel::DrawConsole()
    {
        if (LogList)
        {
            const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();

            ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.f, 20.f));
            if (ImGui::BeginChild("console##", ImVec2(0, -footerHeightToReserve), false, 0))
            {
                static const float timestamp_width = ImGui::CalcTextSize("00:00:00").x;

                // Wrap items.
                ImGui::PushTextWrapPos();

                // Display items.
                for (const auto& item : *LogList)
                {
                    // Exit if word is filtered.
                    if (!m_TextFilter.PassFilter(item.LogString.c_str()))
                        continue;

                    std::stringstream ss;
                    ss << "[" << GetLogLevelName(item.Level) << "]" <<
                        "[" << item.LoggerName << "]" << item.Timestamp << ": " << item.LogString;

                    ImGui::TextColored(ColorPalette[GetLogLevelColor(item.Level)], ss.str().c_str());
                }

                ImGui::PopTextWrapPos();

                bool scrollToBottom = bShouldScroll;

                if (!scrollToBottom)
                    scrollToBottom = (ImGui::GetScrollY() == ImGui::GetScrollMaxY());

                // Auto-scroll logs.
                if (scrollToBottom)
                {
                    ImGui::SetScrollHereY(1.0f);
                    bShouldScroll = false;
                }
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();

            ImGui::EndChild();
        }

    }

    void ConsolePanel::DrawFilterBar()
    {
        m_TextFilter.Draw(ICON_FA_FILTER "  Filter", ImGui::GetWindowWidth() * 0.25f);
        ImGui::Separator();
    }

    void ConsolePanel::DrawInputBar()
    {
        ImGuiInputTextFlags inputTextFlags =
            ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_CallbackCompletion |
            ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackAlways;

        // Only reclaim after enter key is pressed!
        bool reclaimFocus = false;

        ImGui::BeginChild("##input_bar", ImGui::GetContentRegionAvail());
        {
            ImGui::SetCursorPosY(5.f);
            ImGui::Text(ICON_FA_TERMINAL);
            ImGui::SameLine();
            ImGui::SetCursorPosY(0.f);

            ImGui::PushItemWidth(-ImGui::GetStyle().ItemSpacing.x * 5.f);
            if (ImGui::InputText("Input", &Buffer, inputTextFlags))
            {
                CommandSystem::RunCommand(Buffer);

                reclaimFocus = true;
                ScrollToBottom();

                Buffer.clear();
            }
            ImGui::PopItemWidth();

            // Auto-focus on window apparition
            ImGui::SetItemDefaultFocus();
            if (reclaimFocus)
                ImGui::SetKeyboardFocusHere(-1); // Focus on command line after clearing.
        }
        ImGui::EndChild();
    }
}