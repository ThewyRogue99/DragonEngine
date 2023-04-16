#include "PanelManager.h"

#include "Engine/Debug/Log.h"

#include <algorithm>

namespace Engine
{
    PanelManager::~PanelManager()
    {
        RemoveAllPanels();
    }

    void PanelManager::AddPanel(const WString& PanelName, EditorPanel* Panel)
    {
        PanelMap[PanelName] = Panel;
        Panel->Manager = this;

        Panel->OnCreate();
    }

    void PanelManager::AddPanels(std::initializer_list<std::pair<WString, EditorPanel*>> Panels)
    {
        for (auto& [PanelName, Panel] : Panels)
            AddPanel(PanelName, Panel);
    }

    EditorPanel* PanelManager::GetPanel(const WString& PanelName)
    {
        auto it = PanelMap.find(PanelName);

        if (it != PanelMap.end())
            return it->second;

        return nullptr;
    }

    bool PanelManager::RemovePanel(const WString& PanelName)
    {
        auto it = PanelMap.find(PanelName);

        if (it != PanelMap.end())
        {
            it->second->OnDelete();
            delete it->second;

            PanelMap.erase(it);

            return true;
        }

        return false;
    }

    void PanelManager::RemoveAllPanels()
    {
        for (auto& [PanelName, Panel] : PanelMap)
        {
            Panel->OnDelete();
            delete Panel;
        }

        PanelMap.clear();
    }

    void PanelManager::Update(float DeltaTime)
    {
        for (auto& [PanelName, Panel] : PanelMap)
            Panel->OnUpdate(DeltaTime);
    }

    void PanelManager::Render(float DeltaTime)
    {
        for (auto& [PanelName, Panel] : PanelMap)
            Panel->Render(DeltaTime);
    }

    void PanelManager::AddData(const WString& Name, void* Data, size_t size)
    {
        void* Buffer = new uint8_t[size];

        memcpy(Buffer, Data, size);

        for (auto& [PanelName, Panel] : PanelMap)
            Panel->OnData(Name, Buffer, size);

        delete[] Buffer;
    }

    void PanelManager::OnEvent(Event& event)
    {
        for (auto& [PanelName, Panel] : PanelMap)
        {
            Panel->OnEvent(event);
        }
    }

    bool PanelManager::DisablePanel(const WString& PanelName)
    {
        auto it = PanelMap.find(PanelName);

        if (it != PanelMap.end())
        {
            it->second->IsDisabled = true;
            return true;
        }

        return false;
    }

    void PanelManager::DisableAllPanels()
    {
        for (auto& [PanelName, Panel] : PanelMap)
            Panel->IsDisabled = true;
    }

    bool PanelManager::ActivatePanel(const WString& PanelName)
    {
        auto it = PanelMap.find(PanelName);

        if (it != PanelMap.end())
        {
            it->second->IsDisabled = false;
            return true;
        }

        return false;
    }

    void PanelManager::ActivateAllPanels()
    {
        for (auto& [PanelName, Panel] : PanelMap)
            Panel->IsDisabled = false;
    }
}