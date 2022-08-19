namespace DragonEngine
{
	public abstract class Component
	{
		public Entity Entity { get; internal set; }
	}

	public class TagComponent : Component
	{
		public string Tag
		{
			get => InternalCalls.TagComponent_GetTag(Entity.ID);
			set => InternalCalls.TagComponent_SetTag(Entity.ID, ref value);
		}
	}

	public class TransformComponent : Component
	{
		public Transform Transform
		{
			get
			{
				InternalCalls.TransformComponent_GetTransform(Entity.ID, out Transform result);
				return result;
			}

			set => InternalCalls.TransformComponent_SetTransform(Entity.ID, ref value);
		}

		public Vector3 Position
		{
			get
			{
				InternalCalls.TransformComponent_GetPosition(Entity.ID, out Vector3 result);
				return result;
			}

			set => InternalCalls.TransformComponent_SetPosition(Entity.ID, ref value);
		}

		public Vector3 Rotation
		{
			get
			{
				InternalCalls.TransformComponent_GetRotation(Entity.ID, out Vector3 result);
				return result;
			}

			set => InternalCalls.TransformComponent_SetRotation(Entity.ID, ref value);
		}

		public Vector3 Scale
		{
			get
			{
				InternalCalls.TransformComponent_GetScale(Entity.ID, out Vector3 result);
				return result;
			}

			set => InternalCalls.TransformComponent_SetScale(Entity.ID, ref value);
		}

		public Matrix4 TransformMatrix
		{
			get
			{
				InternalCalls.TransformComponent_GetTransformMatrix(Entity.ID, out Matrix4 result);
				return result;
			}

			set => InternalCalls.TransformComponent_SetTransformMatrix(Entity.ID, ref value);
		}
	}

	public class Rigidbody2DComponent : Component
	{
		public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
		{
			InternalCalls.Rigidbody2DComponent_ApplyLinearImpulse(Entity.ID, ref impulse, ref worldPosition, wake);
		}

		public void ApplyLinearImpulse(Vector2 impulse, bool wake)
		{
			InternalCalls.Rigidbody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
		}

	}
}
