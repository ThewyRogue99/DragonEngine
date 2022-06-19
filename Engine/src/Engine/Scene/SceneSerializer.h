#pragma once

#include "Scene.h"

namespace Engine
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const std::wstring& filepath);
		void SerializeRuntime(const std::wstring& filepath);

		bool Deserialize(const std::wstring& filepath);
		bool DeserializeRuntime(const std::wstring& filepath);

	private:
		Ref<Scene> m_Scene;
	};
}
