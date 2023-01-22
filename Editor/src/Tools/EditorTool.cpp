#include "EditorTool.h"

#include "Engine/Audio/AudioEngine.h"
#include "../Scene/EditorSceneManager.h"

namespace Engine
{
    static CallbackDispatcher<> OnBeginPlayDispatch;
    static CallbackDispatcher<> OnEndPlayDispatch;

    static EditorScene* EScene = nullptr;

    static bool bIsPlaying = false;
    static bool bIsSimulating = false;

    void EditorTool::BeginPlay()
    {
        bIsPlaying = true;

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

        OnEndPlayDispatch.Run();

        SceneManager::Clear();
        AudioEngine::StopAllAudio();
    }

    void EditorTool::BeginSimulation()
    {
        if (EScene) { EndSimulation(); }

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
    }

    bool EditorTool::IsPlaying()
    {
        return bIsPlaying;
    }

    bool EditorTool::IsSimulating()
    {
        return bIsSimulating;
    }

    CallbackDispatcher<>::CallbackHandle EditorTool::OnBeginPlay()
    {
        return OnBeginPlayDispatch.GetHandle();
    }

    CallbackDispatcher<>::CallbackHandle EditorTool::OnEndPlay()
    {
        return OnEndPlayDispatch.GetHandle();
    }
}