#include "depch.h"
#include "ConsolePanel.h"

#include <imgui/imgui_internal.h>

static constexpr size_t DefaultBufferSize = 2046;

namespace ImGui
{
    struct InputTextCallback_UserData
    {
        std::string* Str;
        ImGuiInputTextCallback ChainCallback;
        void* ChainCallbackUserData;
    };

    static int InputTextCallback(ImGuiInputTextCallbackData* data)
    {
        auto* user_data = (InputTextCallback_UserData*)data->UserData;
        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
        {
            // Resize string callback
            // If for some reason we refuse the new length (BufTextLen) and/or capacity (BufSize) we need to set them back to what we want.
            std::string* str = user_data->Str;
            IM_ASSERT(data->Buf == str->c_str());
            str->resize(data->BufTextLen);
            data->Buf = (char*)str->c_str();
        }
        else if (user_data->ChainCallback)
        {
            // Forward to user callback, if any
            data->UserData = user_data->ChainCallbackUserData;
            return user_data->ChainCallback(data);
        }
        return 0;
    }

    bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data)
    {
        IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
        flags |= ImGuiInputTextFlags_CallbackResize;

        InputTextCallback_UserData cb_user_data = { };
        cb_user_data.Str = str;
        cb_user_data.ChainCallback = callback;
        cb_user_data.ChainCallbackUserData = user_data;
        return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, &cb_user_data);
    }
}

namespace Engine
{
	ConsolePanel::ConsolePanel() : EditorPanel("Console")
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

	void ConsolePanel::OnRender(float DeltaTime)
	{
        if (!LogList)
        {
            Ref<Console> AttachedConsole = Log::GetConsole();

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
                    ss << "[" << item.LoggerName << "]" << item.Timestamp << ": " << item.LogString;

                    ImGui::TextColored(ColorPalette[GetLogLevelColor(item.Level)], ss.str().c_str());
                }

                ImGui::PopTextWrapPos();

                bShouldAutoScroll = (ImGui::GetScrollY() == ImGui::GetScrollMaxY());

                // Auto-scroll logs.
                if (bShouldAutoScroll)
                    ImGui::SetScrollHereY(1.0f);
            }
            ImGui::PopStyleColor();
            ImGui::PopStyleVar();

            ImGui::EndChild();
        }

    }

    void ConsolePanel::DrawFilterBar()
    {
        m_TextFilter.Draw("Filter", ImGui::GetWindowWidth() * 0.25f);
        ImGui::Separator();
    }

    void ConsolePanel::DrawInputBar()
    {
        ImGuiInputTextFlags inputTextFlags =
            ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackCharFilter | ImGuiInputTextFlags_CallbackCompletion |
            ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackAlways;

        // Only reclaim after enter key is pressed!
        bool reclaimFocus = false;

        ImGui::PushItemWidth(-ImGui::GetStyle().ItemSpacing.x * 7);
        if (ImGui::InputText("Input", &Buffer, inputTextFlags, InputCallback, this))
        {
            // TODO: Implement Command System

            reclaimFocus = true;

            Buffer.clear();
        }
        ImGui::PopItemWidth();

        // Auto-focus on window apparition
        ImGui::SetItemDefaultFocus();
        if (reclaimFocus)
            ImGui::SetKeyboardFocusHere(-1); // Focus on command line after clearing.
    }

    int ConsolePanel::InputCallback(ImGuiInputTextCallbackData* data)
    {
        return 0;
    }
}