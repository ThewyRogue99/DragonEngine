#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"

#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Scene.h"
#include "Engine/Asset/AssetMetadata.h"

namespace Engine
{
	class SceneSerializer
	{
	public:
		static void Serialize(Scene* scene, Ref<AssetMetadata> Metadata);

		static void Deserialize(Scene* scene, Ref<AssetMetadata> Metadata);

	private:
		static void SerializeEntity(MemoryMap& out, Entity entity);

		static Entity DeserializeEntity(const MemoryMap& in, Scene* scene);

		template<typename T>
		static T DeserializeComponent(void* DataPtr)
		{
			MemoryMap* data = (MemoryMap*)DataPtr;

			T comp = T();
			comp.OnDeserialize(*data);

			return comp;
		}
	};
}
