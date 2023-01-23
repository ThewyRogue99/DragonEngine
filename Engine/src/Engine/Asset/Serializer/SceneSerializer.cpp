#include "depch.h"
#include "SceneSerializer.h"

#include "Engine/Scene/Entity.h"
#include "Engine/Scene/Components.h"

#include "Engine/Scripting/ScriptEngine.h"

#include <glm/glm.hpp>

namespace Engine
{
	void SceneSerializer::Serialize(Scene* scene, AssetMetadata& metadata)
	{
		metadata.SetStringField<wchar_t>("SceneName", scene->SceneName);

		scene->SceneRegistry.each([&](auto entityID)
		{
			Entity entity = { entityID, scene };

			if (entity.IsValid())
			{
				if (entity.HasComponent<IDComponent>())
				{
					auto& id = entity.GetComponent<IDComponent>().ID;

					AssetMetadata data;

					SerializeEntity(data, entity);

					metadata.SetField("Entity:" + id.GetString(), data);
				}
			}
		});
	}

	void SceneSerializer::Deserialize(Scene* scene, const AssetMetadata& metadata)
	{
		for (auto& [key, value] : metadata)
		{
			if (value.DataPtr)
			{
				if (key == "SceneName")
				{
					CString sceneName = (const wchar_t*)value.DataPtr;
					scene->SetName(sceneName);

					DE_LOG(SceneSerializer, "Deserializing Scene: {0}", TypeUtils::FromUTF16(sceneName).c_str());
				}
				else
				{
					if (value.IsMap)
					{
						size_t pos = key.find_first_of(':');

						std::string type = key.substr(0, (size_t)pos);
						std::string id = key.substr(pos + 1);

						if (type == "Entity")
						{
							AssetMetadata* data = (AssetMetadata*)value.DataPtr;
							Entity deserializedEntity = DeserializeEntity(*data, scene);
						}
					}
				}
			}
		}
	}

#define SERIALIZE_COMPONENT_MACRO(ComponentName, Ent, Out, ...) \
if (Ent.HasComponent<ComponentName>()) \
{ \
	auto& cc = Ent.GetComponent<ComponentName>(); \
\
	AssetMetadata data; \
	cc.OnSerialize(data); \
\
	Out.SetField(ComponentName::GetStaticTypeName(), data); \
	__VA_ARGS__ \
}

	void SceneSerializer::SerializeEntity(AssetMetadata& out, Entity entity)
	{
		SERIALIZE_COMPONENT_MACRO(IDComponent, entity, out)
		SERIALIZE_COMPONENT_MACRO(TagComponent, entity, out)
		SERIALIZE_COMPONENT_MACRO(TransformComponent, entity, out)
		SERIALIZE_COMPONENT_MACRO(CameraComponent, entity, out)
		SERIALIZE_COMPONENT_MACRO(SpriteRendererComponent, entity, out)
		SERIALIZE_COMPONENT_MACRO(CircleRendererComponent, entity, out)
		SERIALIZE_COMPONENT_MACRO(Rigidbody2DComponent, entity, out)
		SERIALIZE_COMPONENT_MACRO(BoxCollider2DComponent, entity, out)
		SERIALIZE_COMPONENT_MACRO(CircleCollider2DComponent, entity, out)
		SERIALIZE_COMPONENT_MACRO(ScriptComponent, entity, out)
		SERIALIZE_COMPONENT_MACRO(AudioComponent, entity, out)
	}

#define DESERIALIZE_COMPONENT_MACRO(KeyName, ComponentName, Ent, DataPtr,...) \
if (KeyName == #ComponentName) \
{ \
	ComponentName cc = SceneSerializer::DeserializeComponent<ComponentName>(DataPtr); \
	Ent.AddComponent<ComponentName>(cc); \
	__VA_ARGS__ \
}

	Entity SceneSerializer::DeserializeEntity(AssetMetadata& in, Scene* scene)
	{
		Entity deserializedEntity = { };

		{
			// Get ID
			AssetMetadata& idata = in.GetField<AssetMetadata>(IDComponent::GetStaticTypeName());
			IDComponent idc;
			idc.OnDeserialize(idata);

			// Get Name
			AssetMetadata& tagdata = in.GetField<AssetMetadata>(TagComponent::GetStaticTypeName());
			TagComponent tagc;
			tagc.OnDeserialize(tagdata);

			deserializedEntity = scene->CreateEntityWithUUID(idc.ID, tagc.Tag);

			DE_LOG(SceneSerializer, "Deserialized entity with ID = {0}, name = {1}", idc.ID.GetString(), TypeUtils::FromUTF16(tagc.Tag));

			// Get Transform
			AssetMetadata& tdata = in.GetField<AssetMetadata>(TransformComponent::GetStaticTypeName());
			TransformComponent tc;
			tc.OnDeserialize(tdata);

			deserializedEntity.GetComponent<TransformComponent>() = tc;
		}

		for (auto& [name, component] : in)
		{
			DESERIALIZE_COMPONENT_MACRO(name, CameraComponent, deserializedEntity, component.DataPtr)
			DESERIALIZE_COMPONENT_MACRO(name, SpriteRendererComponent, deserializedEntity, component.DataPtr)
			DESERIALIZE_COMPONENT_MACRO(name, CircleRendererComponent, deserializedEntity, component.DataPtr)
			DESERIALIZE_COMPONENT_MACRO(name, Rigidbody2DComponent, deserializedEntity, component.DataPtr)
			DESERIALIZE_COMPONENT_MACRO(name, BoxCollider2DComponent, deserializedEntity, component.DataPtr)
			DESERIALIZE_COMPONENT_MACRO(name, CircleCollider2DComponent, deserializedEntity, component.DataPtr)
			DESERIALIZE_COMPONENT_MACRO(name, ScriptComponent, deserializedEntity, component.DataPtr)
			DESERIALIZE_COMPONENT_MACRO(name, AudioComponent, deserializedEntity, component.DataPtr)
		}

		return deserializedEntity;
	}
}