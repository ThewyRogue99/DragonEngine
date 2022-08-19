using System.Runtime.InteropServices;

namespace DragonEngine
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Transform
	{
		public Vector3 Position;
		public Vector3 Rotation;
		public Vector3 Scale;

		public Vector3 Up { get => new Quaternion(Rotation) * Vector3.Up; }
		public Vector3 Right { get => new Quaternion(Rotation) * Vector3.Right; }
		public Vector3 Forward { get => new Quaternion(Rotation) * Vector3.Forward; }

		public Transform(Vector3 Position, Vector3 Rotation, Vector3 Scale)
		{
			this.Position = Position;
			this.Rotation = Rotation;
			this.Scale = Scale;
		}
	}
}
