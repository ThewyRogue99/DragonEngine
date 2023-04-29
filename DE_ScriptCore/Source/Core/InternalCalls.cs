using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace DragonEngine
{
    internal static class InternalCalls
    {
        [StructLayout(LayoutKind.Sequential)]
        internal struct EntityInfo
        {
            internal EntityInfo(uint Index, string Name)
            {
                EntityIndex = Index;
                SceneName = Name;
            }

            public uint EntityIndex;
            public string SceneName;
        };

        #region Log

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Log_Native(string str);

        #endregion

        #region Input

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode Key);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsMouseButtonDown(MouseButton Button);

        #endregion

        #region SceneManager

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static string SceneManager_LoadScene(string Path, string Name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static string SceneManager_GetActiveSceneName();

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool SceneManager_SetActiveScene(string SceneName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static string[] SceneManager_GetLoadedSceneNameArray();

        #endregion

        #region Scene

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static uint Scene_CreateEntity(string SceneName, string EntityTag);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool Scene_DestroyEntity(EntityInfo Info);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static int Scene_GetEntityCount(string SceneName);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static uint[] Scene_GetEntityHandleArray(string SceneName);

        #endregion

        #region Entity

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(EntityInfo Info, Type ComponentType);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_AddComponent(EntityInfo Info, Type ComponentType);

        #endregion

        #region IDComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static string IDComponent_GetID(EntityInfo Info);

        #endregion

        #region TagComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static string TagComponent_GetTag(EntityInfo Info);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TagComponent_SetTag(EntityInfo Info, ref string value);

        #endregion

        #region TransformComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTransform(EntityInfo Info, out Transform result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetPosition(EntityInfo Info, out Vector3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(EntityInfo Info, out Vector3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(EntityInfo Info, out Vector3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTransformMatrix(EntityInfo Info, out Matrix4 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTransform(EntityInfo Info, ref Transform value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetPosition(EntityInfo Info, ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(EntityInfo Info, ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(EntityInfo Info, ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTransformMatrix(EntityInfo Info, ref Matrix4 value);

        #endregion

        #region Rigidbody2DComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulse(EntityInfo Info, ref Vector2 Impulse, ref Vector2 Point, bool Wake);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(EntityInfo Info, ref Vector2 Impulse, bool Wake);

        #endregion

        #region AudioComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool AudioComponent_PlayAudio(EntityInfo Info);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool AudioComponent_StopAudio(EntityInfo Info);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool AudioComponent_IsPlaying(EntityInfo Info);

        #endregion
    }
}
