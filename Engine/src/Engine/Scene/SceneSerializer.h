#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Scene.h"

namespace Engine
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);

		void Serialize(const CString& filepath);
		void SerializeRuntime(const CString& filepath);

		bool Deserialize(const CString& filepath);
		bool DeserializeRuntime(const CString& filepath);

	private:
		Ref<Scene> m_Scene;
	};
}
