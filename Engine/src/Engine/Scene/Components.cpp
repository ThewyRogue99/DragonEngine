#include "depch.h"
#include "Components.h"

#include "Engine/Asset/Serializer/Serializer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scripting/ScriptEngine.h"

namespace Engine
{
	void IDComponent::OnSerialize(AssetMetadata& Metadata)
	{
		Metadata.SetStringField("ID", ID.GetString());
	}

	void IDComponent::OnDeserialize(AssetMetadata& Metadata)
	{
		ID = Metadata.GetStringField<char>("ID");
	}

	void TagComponent::OnSerialize(AssetMetadata& Metadata)
	{
		Metadata.SetStringField("Tag", Tag);
	}

	void TagComponent::OnDeserialize(AssetMetadata& Metadata)
	{
		Tag = Metadata.GetStringField<wchar_t>("Tag");
	}

	void TransformComponent::OnSerialize(AssetMetadata& Metadata)
	{
		Metadata.SetField("Position", Position);
		Metadata.SetField("Rotation", Rotation);
		Metadata.SetField("Scale", Scale);
	}

	void TransformComponent::OnDeserialize(AssetMetadata& Metadata)
	{
		Position = Metadata.GetField<glm::vec3>("Position");
		Rotation = Metadata.GetField<glm::vec3>("Rotation");
		Scale = Metadata.GetField<glm::vec3>("Scale");
	}

	void SpriteRendererComponent::OnSerialize(AssetMetadata& Metadata)
	{
		Metadata.SetField("Color", Color);

		Metadata.SetStringField("TextureID", TextureID);

		Metadata.SetField("TilingFactor", TilingFactor);
	}

	void SpriteRendererComponent::OnDeserialize(AssetMetadata& Metadata)
	{
		Color = Metadata.GetField<glm::vec4>("Color");

		//TODO: Implement texture deserialize
		TextureID = Metadata.GetStringField<char>("TextureID");
		Asset TextureAsset = AssetManager::LoadAsset(TextureID);
		if (TextureAsset.GetAssetType() == AssetType::Texture)
		{
			Texture = Serializer::DeserializeTexture(*TextureAsset.GetData());
		}

		AssetManager::CloseAsset(TextureAsset);

		TilingFactor = Metadata.GetField<float>("TilingFactor");
	}

	void CircleRendererComponent::OnSerialize(AssetMetadata& Metadata)
	{
		Metadata.SetField("Color", Color);
		Metadata.SetField("Radius", Radius);
		Metadata.SetField("Thickness", Thickness);
		Metadata.SetField("Fade", Fade);
	}

	void CircleRendererComponent::OnDeserialize(AssetMetadata& Metadata)
	{
		Color = Metadata.GetField<glm::vec4>("Color");
		Radius = Metadata.GetField<float>("Radius");
		Thickness = Metadata.GetField<float>("Thickness");
		Fade = Metadata.GetField<float>("Fade");
	}

	void CameraComponent::OnSerialize(AssetMetadata& Metadata)
	{
		Metadata.SetField("SceneCamera", Camera);

		Metadata.SetField("Primary", Primary);
		Metadata.SetField("FixedAspectRatio", FixedAspectRatio);
	}

	void CameraComponent::OnDeserialize(AssetMetadata& Metadata)
	{
		SceneCamera CopyCamera = Metadata.GetField<SceneCamera>("SceneCamera");

		Primary = Metadata.GetField<bool>("Primary");
		FixedAspectRatio = Metadata.GetField<bool>("FixedAspectRatio");
	}

	void AudioComponent::OnSerialize(AssetMetadata& Metadata)
	{
		Metadata.SetStringField("AudioID", AudioID);
	}

	void AudioComponent::OnDeserialize(AssetMetadata& Metadata)
	{
		AudioID = Metadata.GetStringField<char>("AudioID");

		Asset AudioAsset = AssetManager::LoadAsset(AudioID);

		if (AudioAsset.GetAssetType() == AssetType::Audio)
		{
			Source = AudioEngine::CreateAudioSource();

			Ref<AudioBuffer> Buff = AudioEngine::CreateAudioBuffer(Serializer::DeserializeAudio(*AudioAsset.GetData()));

			Source->SetBuffer(Buff);
		}
		AssetManager::CloseAsset(AudioAsset);
	}

	void ScriptComponent::OnSerialize(AssetMetadata& Metadata)
	{
		Metadata.SetStringField("Name", Name);
		Metadata.SetStringField("Namespace", Namespace);

		AssetMetadata fieldData;
		if (Fields)
		{
			for (auto& field : *Fields)
			{
				void* Data = field.GetBufferData();
				size_t size = field.GetBufferSize();

				fieldData.SetField(field.GetName(), Data, size);
			}
		}

		Metadata.SetField("Fields", fieldData);
	}

	void ScriptComponent::OnDeserialize(AssetMetadata& Metadata)
	{
		Name = Metadata.GetStringField<char>("Name");
		Namespace = Metadata.GetStringField<char>("Namespace");

		AssetMetadata& fieldData = Metadata.GetField<AssetMetadata>("Fields");
		Fields = &ScriptEngine::GetScriptData(Namespace, Name)->GetFields();

		for (auto& tuple : fieldData)
		{
			auto& fieldName = tuple.first;
			auto& field = tuple.second;

			auto& it = std::find_if(Fields->begin(), Fields->end(), [fieldName](const ScriptField& field)
			{
				return field.GetName() == fieldName;
			});

			if (it != Fields->end())
			{
				(*it).SetBufferData(field.DataPtr, field.DataSize);
			}
		}
	}

	void Rigidbody2DComponent::OnSerialize(AssetMetadata& Metadata)
	{
		Metadata.SetField("Type", Type);
		Metadata.SetField("FixedRotation", FixedRotation);
	}

	void Rigidbody2DComponent::OnDeserialize(AssetMetadata& Metadata)
	{
		Type = Metadata.GetField<BodyType>("Type");
		FixedRotation = Metadata.GetField<bool>("FixedRotation");
	}

	void BoxCollider2DComponent::OnSerialize(AssetMetadata& Metadata)
	{
		Metadata.SetField("Offset", Offset);
		Metadata.SetField("Size", Size);

		Metadata.SetField("Density", Density);
		Metadata.SetField("Friction", Friction);
		Metadata.SetField("Restitution", Restitution);
		Metadata.SetField("RestitutionThreshold", RestitutionThreshold);
	}

	void BoxCollider2DComponent::OnDeserialize(AssetMetadata& Metadata)
	{
		Offset = Metadata.GetField<glm::vec2>("Offset");
		Size = Metadata.GetField<glm::vec2>("Size");

		Density = Metadata.GetField<float>("Density");
		Friction = Metadata.GetField<float>("Friction");
		Restitution = Metadata.GetField<float>("Restitution");
		RestitutionThreshold = Metadata.GetField<float>("RestitutionThreshold");
	}

	void CircleCollider2DComponent::OnSerialize(AssetMetadata& Metadata)
	{
		Metadata.SetField("Offset", Offset);
		Metadata.SetField("Radius", Radius);

		Metadata.SetField("Density", Density);
		Metadata.SetField("Friction", Friction);
		Metadata.SetField("Restitution", Restitution);
		Metadata.SetField("RestitutionThreshold", RestitutionThreshold);
	}

	void CircleCollider2DComponent::OnDeserialize(AssetMetadata& Metadata)
	{
		Offset = Metadata.GetField<glm::vec2>("Offset");
		Radius = Metadata.GetField<float>("Radius");

		Density = Metadata.GetField<float>("Density");
		Friction = Metadata.GetField<float>("Friction");
		Restitution = Metadata.GetField<float>("Restitution");
		RestitutionThreshold = Metadata.GetField<float>("RestitutionThreshold");
	}
}