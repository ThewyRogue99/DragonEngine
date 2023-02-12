#include "PanelManager.h"

#include "Engine/Debug/Log.h"

#include <algorithm>

namespace Engine
{
    PanelManager::~PanelManager()
    {
        RemoveAllPanels();
        ClearData();
    }

    void PanelManager::AddPanel(EditorPanel* panel)
    {
        Panels.push_back(panel);
        panel->manager = this;

        panel->OnCreate();
    }

    void PanelManager::AddPanels(std::initializer_list<EditorPanel*> panels)
    {
        for (auto& panel : panels)
            AddPanel(panel);
    }

    void PanelManager::RemovePanel(const std::string& PanelName)
    {
        for (uint32_t i = 0; i < Panels.size(); i++)
        {
            auto panel = Panels[i];

            if (panel->PanelName == PanelName)
            {
                Panels.erase(Panels.begin() + i);
                panel->OnDelete();

                delete panel;
                break;
            }
        }
    }

    void PanelManager::RemoveAllPanels()
    {
        for (auto& panel : Panels)
        {
            panel->OnDelete();
            delete panel;
        }

        Panels.clear();
    }

    void PanelManager::Update(float DeltaTime)
    {
        for (auto& panel : Panels)
            panel->OnUpdate(DeltaTime);
    }

    void PanelManager::Render(float DeltaTime)
    {
        for (auto& panel : Panels)
            panel->Render(DeltaTime);
    }

    void PanelManager::AddData(const CString& Name, void* Data, size_t size)
    {
        void* Buffer = new uint8_t[size];

        memcpy(Buffer, Data, size);

        auto it = std::find_if(DataList.begin(), DataList.end(), [Name](const PanelData& data)
        {
            return data.name == Name;
        });

        if (it != DataList.end())
        {
            void* ptr = (*it).Data;

            (*it).name = Name;
            (*it).Data = Buffer;
            (*it).size = size;

            delete[] ptr;
        }
        else
        {
            DataList.push_back({ Name, Buffer, size });
        }

        for (auto& panel : Panels)
            panel->OnData(Name, Buffer, size);
    }

    void PanelManager::RemoveData(const CString& name)
    {
        auto end = std::remove_if(DataList.begin(), DataList.end(), [name](const PanelData& data)
        {
            if (data.name == name)
            {
                delete[] data.Data;
                return true;
            }

            return false;
        });

        DataList.erase(end, DataList.end());
    }

    void PanelManager::ClearData()
    {
        std::for_each(DataList.begin(), DataList.end(), [](const PanelData& data)
        {
            delete[] data.Data;
        });

        DataList.clear();
    }

    void PanelManager::OnEvent(Event& event)
    {
        Event* ptr = &event;
        std::for_each(Panels.begin(), Panels.end(), [ptr](EditorPanel* panel)
        {
            panel->OnEvent(*ptr);
        });
    }

    void PanelManager::DisablePanel(const std::string& PanelName)
    {
        for (auto& panel : Panels)
        {
            if (panel->PanelName == PanelName)
            {
                panel->IsDisabled = true;
                break;
            }
        }
    }

    void PanelManager::DisableAllPanels()
    {
        for (auto& panel : Panels)
            panel->IsDisabled = true;
    }

    void PanelManager::ActivatePanel(const std::string& PanelName)
    {
        for (auto& panel : Panels)
        {
            if (panel->PanelName == PanelName)
            {
                panel->IsDisabled = false;
                break;
            }
        }
    }

    void PanelManager::ActivateAllPanels()
    {
        for (auto& panel : Panels)
            panel->IsDisabled = false;
    }

    PanelManager::PanelData& PanelManager::GetData(const CString& name)
    {
        auto it = std::find_if(DataList.begin(), DataList.end(), [name](const PanelData& data)
        {
            return data.name == name;
        });

        return *it;
    }
}