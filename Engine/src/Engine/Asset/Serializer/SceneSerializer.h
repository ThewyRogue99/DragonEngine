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
		static void Serialize(Scene* scene, AssetMetadata& metadata);

		static void Deserialize(Scene* scene, const AssetMetadata& metadata);

	private:
		static void SerializeEntity(AssetMetadata& out, Entity entity);

		static Entity DeserializeEntity(AssetMetadata& in, Scene* scene);

		template<typename T>
		static T DeserializeComponent(void* DataPtr)
		{
			AssetMetadata* data = (AssetMetadata*)DataPtr;

			T comp = T();
			comp.OnDeserialize(*data);

			return comp;
		}
	};
}
