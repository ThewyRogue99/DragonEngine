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
        internal extern static bool Entity_HasComponent(ulong EntityID, Type ComponentType);

        #endregion

        #region TagComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static string TagComponent_GetTag(ulong EntityID);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TagComponent_SetTag(ulong EntityID, ref string value);

        #endregion

        #region TransformComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTransform(ulong EntityID, out Transform result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetPosition(ulong EntityID, out Vector3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetRotation(ulong EntityID, out Vector3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetScale(ulong EntityID, out Vector3 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTransformMatrix(ulong EntityID, out Matrix4 result);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTransform(ulong EntityID, ref Transform value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetPosition(ulong EntityID, ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetRotation(ulong EntityID, ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetScale(ulong EntityID, ref Vector3 value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTransformMatrix(ulong EntityID, ref Matrix4 value);

        #endregion

        #region Rgidbody2DComponent

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulse(ulong EntityID, ref Vector2 Impulse, ref Vector2 Point, bool Wake);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(ulong EntityID, ref Vector2 Impulse, bool Wake);

        #endregion
    }
}
