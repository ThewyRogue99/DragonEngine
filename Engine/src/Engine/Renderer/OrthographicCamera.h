#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
		void SetProjection(float left, float right, float bottom, float top);

		inline const glm::vec3& GetPosition() const { return Position; }
		inline void SetPosition(const glm::vec3& Position) { this->Position = Position; RecalculateViewMatrix(); }
		inline void AddPosition(const glm::vec3& Position) { this->Position += Position; RecalculateViewMatrix(); }

		inline float GetRotation() const { return Rotation; }
		inline void SetRotation(float Rotation) { this->Rotation = Rotation; RecalculateViewMatrix(); }
		inline void AddRotation(float Rotation) { this->Rotation += Rotation; RecalculateViewMatrix(); }

		const glm::mat4& GetProjectionMatrix() const { return ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return ViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return ViewProjectionMatrix; }

		void RecalculateViewMatrix();

	private:
		glm::mat4 ProjectionMatrix = glm::mat4(1.f);
		glm::mat4 ViewMatrix = glm::mat4(1.f);
		glm::mat4 ViewProjectionMatrix = glm::mat4(1.f);

		glm::vec3 Position = glm::vec3(0.f);
		float Rotation = 0.f;
	};
}
