#include "EditorTool.h"

#include "Engine/Audio/AudioEngine.h"
#include "../Scene/EditorSceneManager.h"

namespace Engine
{
    static CallbackDispatcher<void> OnBeginPlayDispatch;
    static CallbackDispatcher<void> OnEndPlayDispatch;

    static EditorScene* EScene = nullptr;

    static bool bIsPlaying = false;
    static bool bIsSimulating = false;

    void EditorTool::BeginPlay()
    {
        bIsPlaying = true;

        DE_INFO(EditorTool, "Editor BeginPlay");

        EditorScene* EScene = EditorSceneManager::GetEditorScene();

        SceneManager::Clear();
        Scene* SandboxScene = SceneManager::CreateScene(EScene->GetName());
        EScene->Copy(SandboxScene);

        SceneManager::SetActiveScene(SandboxScene);

        OnBeginPlayDispatch.Run();
    }

    void EditorTool::EndPlay()
    {
        bIsPlaying = false;

        DE_INFO(EditorTool, "Editor EndPlay");

        OnEndPlayDispatch.Run();

        SceneManager::Clear();
        AudioEngine::StopAllAudio();
    }

    void EditorTool::BeginSimulation()
    {
        if (EScene) { EndSimulation(); }

        DE_INFO(EditorTool, "Editor BeginSimulation");

        bIsSimulating = true;

        EScene = EditorSceneManager::GetEditorScene();

        EditorScene* CopyScene = EditorSceneManager::CreateEditorScene(EScene->GetName(), false);
        CopyScene->BeginSimulation();
    }

    void EditorTool::EndSimulation()
    {
        if (EScene)
        {
            EditorScene* CopyScene = EditorSceneManager::GetEditorScene();
            CopyScene->EndSimulation();

            EditorSceneManager::SetEditorScene(EScene);

            EScene = nullptr;
        }

        bIsSimulating = false;

        DE_INFO(EditorTool, "Editor EndSimulation");
    }

    bool EditorTool::IsPlaying()
    {
        return bIsPlaying;
    }

    bool EditorTool::IsSimulating()
    {
        return bIsSimulating;
    }

    CallbackDispatcher<void>::CallbackHandle EditorTool::OnBeginPlay()
    {
        return OnBeginPlayDispatch.GetHandle();
    }

    CallbackDispatcher<void>::CallbackHandle EditorTool::OnEndPlay()
    {
        return OnEndPlayDispatch.GetHandle();
    }
}