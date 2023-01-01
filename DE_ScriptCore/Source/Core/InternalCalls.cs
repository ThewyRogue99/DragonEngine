using System;
using System.Runtime.CompilerServices;

namespace DragonEngine
{
    public static class InternalCalls
    {
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

        #region Entity

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(string EntityID, Type ComponentType);

        #endregion

        #region TagComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static string TagComponent_GetTag(string EntityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TagComponent_SetTag(string EntityID, ref string value);

        #endregion

        #region TransformComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTransform(string EntityID, out Transform result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetPosition(string EntityID, out Vector3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(string EntityID, out Vector3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(string EntityID, out Vector3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTransformMatrix(string EntityID, out Matrix4 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTransform(string EntityID, ref Transform value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetPosition(string EntityID, ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(string EntityID, ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(string EntityID, ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTransformMatrix(string EntityID, ref Matrix4 value);

        #endregion

        #region Rgidbody2DComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulse(string EntityID, ref Vector2 Impulse, ref Vector2 Point, bool Wake);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(string EntityID, ref Vector2 Impulse, bool Wake);

        #endregion

        #region AudioComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool AudioComponent_PlayAudio(string EntityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool AudioComponent_StopAudio(string EntityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static bool AudioComponent_IsPlaying(string EntityID);

        #endregion
    }
}
