namespace DragonEngine
{
	public abstract class Component
	{
		public Entity Entity { get; internal set; }
	}

    public class IDComponent : Component
    {
        public string ID
        {
            get => InternalCalls.IDComponent_GetID(Entity.GetEntityInfo());
        }
    }

    public class TagComponent : Component
	{
		public string Tag
		{
			get => InternalCalls.TagComponent_GetTag(Entity.GetEntityInfo());
			set => InternalCalls.TagComponent_SetTag(Entity.GetEntityInfo(), ref value);
		}
	}

	public class TransformComponent : Component
	{
		public Transform Transform
		{
			get
			{
				InternalCalls.TransformComponent_GetTransform(Entity.GetEntityInfo(), out Transform result);
				return result;
			}

			set => InternalCalls.TransformComponent_SetTransform(Entity.GetEntityInfo(), ref value);
		}

		public Vector3 Position
		{
			get
			{
				InternalCalls.TransformComponent_GetPosition(Entity.GetEntityInfo(), out Vector3 result);
				return result;
			}

			set => InternalCalls.TransformComponent_SetPosition(Entity.GetEntityInfo(), ref value);
		}

		public Vector3 Rotation
		{
			get
			{
				InternalCalls.TransformComponent_GetRotation(Entity.GetEntityInfo(), out Vector3 result);
				return result;
			}

			set => InternalCalls.TransformComponent_SetRotation(Entity.GetEntityInfo(), ref value);
		}

		public Vector3 Scale
		{
			get
			{
				InternalCalls.TransformComponent_GetScale(Entity.GetEntityInfo(), out Vector3 result);
				return result;
			}

			set => InternalCalls.TransformComponent_SetScale(Entity.GetEntityInfo(), ref value);
		}

		public Matrix4 TransformMatrix
		{
			get
			{
				InternalCalls.TransformComponent_GetTransformMatrix(Entity.GetEntityInfo(), out Matrix4 result);
				return result;
			}

			set => InternalCalls.TransformComponent_SetTransformMatrix(Entity.GetEntityInfo(), ref value);
		}
	}

    public class SpriteRendererComponent : Component
    {

    }

    public class Rigidbody2DComponent : Component
	{
		public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
		{
			InternalCalls.Rigidbody2DComponent_ApplyLinearImpulse(Entity.GetEntityInfo(), ref impulse, ref worldPosition, wake);
		}

		public void ApplyLinearImpulse(Vector2 impulse, bool wake)
		{
			InternalCalls.Rigidbody2DComponent_ApplyLinearImpulseToCenter(Entity.GetEntityInfo(), ref impulse, wake);
		}
	}

    public class AudioComponent : Component
    {
		public bool IsPlaying
		{
			get
			{
				return InternalCalls.AudioComponent_IsPlaying(Entity.GetEntityInfo());
			}
		}

        public void PlayAudio()
		{
			InternalCalls.AudioComponent_PlayAudio(Entity.GetEntityInfo());
		}

		public void StopAudio()
		{
			InternalCalls.AudioComponent_StopAudio(Entity.GetEntityInfo());
		}
    }
}
