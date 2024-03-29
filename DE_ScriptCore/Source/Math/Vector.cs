﻿using System;
using System.Runtime.InteropServices;

namespace DragonEngine
{
	[StructLayout(LayoutKind.Sequential)]
	public struct Vector2
	{
		public static Vector2 Zero = new Vector2(0, 0);
		public static Vector2 Right = new Vector2(1, 0);
		public static Vector2 Left = new Vector2(-1, 0);
		public static Vector2 Up = new Vector2(0, 1);
		public static Vector2 Down = new Vector2(0, -1);

		public float X;
		public float Y;

		public Vector2(float scalar) => X = Y = scalar;

		public Vector2(float x, float y)
		{
			X = x;
			Y = y;
		}

		public Vector2(Vector3 vector)
		{
			X = vector.X;
			Y = vector.Y;
		}

		public void Clamp(Vector2 min, Vector2 max)
		{
			X = Mathf.Clamp(X, min.X, max.X);
			Y = Mathf.Clamp(Y, min.Y, max.Y);
		}

		public float Length() => (float)Math.Sqrt(X * X + Y * Y);

		public Vector2 Normalized()
		{
			float length = Length();
			return length > 0.0f ? New(v => v / length) : new Vector2(X, Y);
		}

		public void Normalize()
		{
			float length = Length();
			if (length == 0.0f)
				return;

			Apply(v => v / length);
		}

		public float Distance(Vector3 other)
		{
			return (float)Math.Sqrt(Math.Pow(other.X - X, 2) +
									Math.Pow(other.Y - Y, 2));
		}

		public static float Distance(Vector3 p1, Vector3 p2)
		{
			return (float)Math.Sqrt(Math.Pow(p2.X - p1.X, 2) +
									Math.Pow(p2.Y - p1.Y, 2));
		}

		//Lerps from p1 to p2
		public static Vector2 Lerp(Vector2 p1, Vector2 p2, float maxDistanceDelta)
		{
			if (maxDistanceDelta < 0.0f)
				return p1;
			if (maxDistanceDelta > 1.0f)
				return p2;

			return p1 + ((p2 - p1) * maxDistanceDelta);
		}

		/// <summary>
		/// Allows you to pass in a delegate that takes in
		/// a double to process the vector per axis.
		/// i.e. (Math.Cos) or a lambda (v => v * 3)
		/// </summary>
		/// <param name="func">Delegate 'double' method to act as a scalar to process X and Y</param>
		public void Apply(Func<double, double> func)
		{
			X = (float)func(X);
			Y = (float)func(Y);
		}

		/// <summary>
		/// Allows you to pass in a delegate that takes in and returns
		/// a new Vector processed per axis.
		/// i.e. (Math.Cos) or a lambda (v => v * 3)
		/// </summary>
		/// <param name="func">Delegate 'double' method to act as a scalar to process X and Y</param>
		public Vector2 New(Func<double, double> func) => new Vector2((float)func(X), (float)func(Y));

		public static Vector2 operator *(Vector2 left, float scalar) => new Vector2(left.X * scalar, left.Y * scalar);
		public static Vector2 operator *(float scalar, Vector2 right) => new Vector2(scalar * right.X, scalar * right.Y);
		public static Vector2 operator *(Vector2 left, Vector2 right) => new Vector2(left.X * right.X, left.Y * right.Y);
		public static Vector2 operator /(Vector2 left, Vector2 right) => new Vector2(left.X / right.X, left.Y / right.Y);
		public static Vector2 operator /(Vector2 left, float scalar) => new Vector2(left.X / scalar, left.Y / scalar);
		public static Vector2 operator /(float scalar, Vector2 right) => new Vector2(scalar / right.X, scalar / right.Y);
		public static Vector2 operator +(Vector2 left, Vector2 right) => new Vector2(left.X + right.X, left.Y + right.Y);
		public static Vector2 operator +(Vector2 left, float right) => new Vector2(left.X + right, left.Y + right);
		public static Vector2 operator -(Vector2 left, Vector2 right) => new Vector2(left.X - right.X, left.Y - right.Y);
		public static Vector2 operator -(Vector2 vector) => new Vector2(-vector.X, -vector.Y);

		public override string ToString() => "Vector2[" + X + ", " + Y + "]";
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct Vector3 : IEquatable<Vector3>
	{
		public static Vector3 Zero = new Vector3(0, 0, 0);
		public static Vector3 One = new Vector3(1, 1, 1);
		public static Vector3 Forward = new Vector3(0, 0, -1);
		public static Vector3 Back = new Vector3(0, 0, 1);
		public static Vector3 Right = new Vector3(1, 0, 0);
		public static Vector3 Left = new Vector3(-1, 0, 0);
		public static Vector3 Up = new Vector3(0, 1, 0);
		public static Vector3 Down = new Vector3(0, -1, 0);

		public static Vector3 Inifinity = new Vector3(float.PositiveInfinity);

		public float X;
		public float Y;
		public float Z;

		public Vector3(float scalar) => X = Y = Z = scalar;

		public Vector3(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}

		public Vector3(float x, Vector2 yz)
		{
			X = x;
			Y = yz.X;
			Z = yz.Y;
		}

		public Vector3(Vector2 vector)
		{
			X = vector.X;
			Y = vector.Y;
			Z = 0.0f;
		}

		public Vector3(Vector4 vector)
		{
			X = vector.X;
			Y = vector.Y;
			Z = vector.Z;
		}

		public void Clamp(Vector3 min, Vector3 max)
		{
			X = Mathf.Clamp(X, min.X, max.X);
			Y = Mathf.Clamp(Y, min.Y, max.Y);
			Z = Mathf.Clamp(Z, min.Z, max.Z);
		}

		public float Length() => (float)Math.Sqrt(X * X + Y * Y + Z * Z);

		public Vector3 Normalized()
		{
			float length = Length();
			return length > 0.0f ? New(v => v / length) : new Vector3(X, Y, Z);
		}


		public void Normalize()
		{
			float length = Length();
			Apply(v => v / length);
		}

		public static Vector3 DirectionFromEuler(Vector3 rotation)
		{
			float pitch = rotation.X;
			float yaw = rotation.Y;
			float roll = rotation.Z;

			return new Vector3(Mathf.Cos(yaw) * Mathf.Cos(pitch),
				Mathf.Sin(yaw) * Mathf.Cos(pitch),
				Mathf.Sin(pitch));
		}

		public float Distance(Vector3 other)
		{
			return (float)Math.Sqrt(Math.Pow(other.X - X, 2) +
									Math.Pow(other.Y - Y, 2) +
									Math.Pow(other.Z - Z, 2));
		}

		public static float Distance(Vector3 p1, Vector3 p2)
		{
			return (float)Math.Sqrt(Math.Pow(p2.X - p1.X, 2) +
									Math.Pow(p2.Y - p1.Y, 2) +
									Math.Pow(p2.Z - p1.Z, 2));
		}

		//Lerps from p1 to p2
		public static Vector3 Lerp(Vector3 p1, Vector3 p2, float maxDistanceDelta)
		{
			if (maxDistanceDelta < 0.0f)
				return p1;

			if (maxDistanceDelta > 1.0f)
				return p2;

			return p1 + ((p2 - p1) * maxDistanceDelta);
		}

		public const float kEpsilonNormalSqrt = 1e-15F;

		public float sqrMagnitude { get { return X * X + Y * Y + Z * Z; } }

		public static float Dot(Vector3 lhs, Vector3 rhs) { return lhs.X * rhs.X + lhs.Y * rhs.Y + lhs.Z * rhs.Z; }

		public static float Angle(Vector3 from, Vector3 to)
		{
			// sqrt(a) * sqrt(b) = sqrt(a * b) -- valid for real numbers
			float denominator = (float)Math.Sqrt(from.sqrMagnitude * to.sqrMagnitude);
			if (denominator < kEpsilonNormalSqrt)
				return 0F;

			float dot = Mathf.Clamp(Dot(from, to) / denominator, -1F, 1F);
			return ((float)Math.Acos(dot)) * Mathf.Rad2Deg;
		}

		/// <summary>
		/// Allows you to pass in a delegate that takes in
		/// a double to process the vector per axis.
		/// i.e. (Math.Cos) or a lambda (v => v * 3)
		/// </summary>
		/// <param name="func">Delegate 'double' method to act as a scalar to process X, Y and Z</param>
		public void Apply(Func<double, double> func)
		{
			X = (float)func(X);
			Y = (float)func(Y);
			Z = (float)func(Z);
		}

		/// <summary>
		/// Allows you to pass in a delegate that takes in and returns
		/// a new Vector processed per axis.
		/// i.e. (Math.Cos) or a lambda (v => v * 3)
		/// </summary>
		/// <param name="func">Delegate 'double' method to act as a scalar to process X, Y and Z</param>
		public Vector3 New(Func<double, double> func) => new Vector3((float)func(X), (float)func(Y), (float)func(Z));

		public static Vector3 operator *(Vector3 left, float scalar) => new Vector3(left.X * scalar, left.Y * scalar, left.Z * scalar);
		public static Vector3 operator *(float scalar, Vector3 right) => new Vector3(scalar * right.X, scalar * right.Y, scalar * right.Z);
		public static Vector3 operator *(Vector3 left, Vector3 right) => new Vector3(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
		public static Vector3 operator /(Vector3 left, Vector3 right) => new Vector3(left.X / right.X, left.Y / right.Y, left.Z / right.Z);
		public static Vector3 operator /(Vector3 left, float scalar) => new Vector3(left.X / scalar, left.Y / scalar, left.Z / scalar);
		public static Vector3 operator /(float scalar, Vector3 right) => new Vector3(scalar / right.X, scalar / right.Y, scalar / right.Z);
		public static Vector3 operator +(Vector3 left, Vector3 right) => new Vector3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);
		public static Vector3 operator +(Vector3 left, float right) => new Vector3(left.X + right, left.Y + right, left.Z + right);
		public static Vector3 operator -(Vector3 left, Vector3 right) => new Vector3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);
		public static Vector3 operator -(Vector3 vector) => new Vector3(-vector.X, -vector.Y, -vector.Z);

		public static Vector3 Cross(Vector3 x, Vector3 y)
		{
			return new Vector3(
				x.Y * y.Z - y.Y * x.Z,
				x.Z * y.X - y.Z * x.X,
				x.X * y.Y - y.X * x.Y
			);
		}

		public override bool Equals(object obj) => obj is Vector3 other && Equals(other);
		public bool Equals(Vector3 right) => X == right.X && Y == right.Y && Z == right.Z;

		public override int GetHashCode() => (X, Y, Z).GetHashCode();

		public static bool operator ==(Vector3 left, Vector3 right) => left.Equals(right);
		public static bool operator !=(Vector3 left, Vector3 right) => !(left == right);

		public static Vector3 Cos(Vector3 vector) => vector.New(Math.Cos);
		public static Vector3 Sin(Vector3 vector) => vector.New(Math.Sin);

		public override string ToString() => $"Vector3[{X}, {Y}, {Z}]";

		public Vector2 XY
		{
			get => new Vector2(X, Y);
			set { X = value.X; Y = value.Y; }
		}

		public Vector2 XZ
		{
			get => new Vector2(X, Z);
			set { X = value.X; Z = value.Y; }
		}

		public Vector2 YZ
		{
			get => new Vector2(Y, Z);
			set { Y = value.X; Z = value.Y; }
		}
	}

	[StructLayout(LayoutKind.Sequential)]
	public struct Vector4
	{
		public float X;
		public float Y;
		public float Z;
		public float W;

		public Vector4(float scalar) => X = Y = Z = W = scalar;

		public Vector4(float x, float y, float z, float w)
		{
			X = x;
			Y = y;
			Z = z;
			W = w;
		}

		public static Vector4 Lerp(Vector4 a, Vector4 b, float t)
		{
			if (t < 0.0f) t = 0.0f;
			if (t > 1.0f) t = 1.0f;
			return (1.0f - t) * a + t * b;
		}

		/// <summary>
		/// Allows you to pass in a delegate that takes in
		/// a double to process the vector per axis, retaining W.
		/// i.e. (Math.Cos) or a lambda (v => v * 3)
		/// </summary>
		/// <param name="func">Delegate 'double' method to act as a scalar to process X, Y and Z, retaining W</param>
		public void Apply(Func<double, double> func)
		{
			X = (float)func(X);
			Y = (float)func(Y);
			Z = (float)func(Z);
		}

		/// <summary>
		/// Allows you to pass in a delegate that takes in and returns
		/// a new Vector processed per axis, retaining W.
		/// i.e. (Math.Cos) or a lambda (v => v * 3)
		/// </summary>
		/// <param name="func">Delegate 'double' method to act as a scalar to process X, Y and Z, retaining W</param>
		public Vector4 New(Func<double, double> func) => new Vector4((float)func(X), (float)func(Y), (float)func(Z), W);

		public static Vector4 operator +(Vector4 left, Vector4 right) => new Vector4(left.X + right.X, left.Y + right.Y, left.Z + right.Z, left.W + right.W);
		public static Vector4 operator -(Vector4 left, Vector4 right) => new Vector4(left.X - right.X, left.Y - right.Y, left.Z - right.Z, left.W - right.W);
		public static Vector4 operator *(Vector4 left, Vector4 right) => new Vector4(left.X * right.X, left.Y * right.Y, left.Z * right.Z, left.W * right.W);
		public static Vector4 operator *(Vector4 left, float scalar) => new Vector4(left.X * scalar, left.Y * scalar, left.Z * scalar, left.W * scalar);
		public static Vector4 operator *(float scalar, Vector4 right) => new Vector4(scalar * right.X, scalar * right.Y, scalar * right.Z, scalar * right.W);
		public static Vector4 operator /(Vector4 left, Vector4 right) => new Vector4(left.X / right.X, left.Y / right.Y, left.Z / right.Z, left.W / right.W);
		public static Vector4 operator /(Vector4 left, float scalar) => new Vector4(left.X / scalar, left.Y / scalar, left.Z / scalar, left.W / scalar);
	}
}