#include "depch.h"
#include "Components.h"

#include "Engine/Asset/TextureManager.h"
#include "Engine/Asset/Serializer/Serializer.h"
#include "Engine/Asset/AssetManager.h"
#include "Engine/Scripting/ScriptEngine.h"

namespace Engine
{
	void IDComponent::OnSerialize(MemoryMap& DataMap)
	{
		DataMap.SetStringField("ID", ID.GetString());
	}

	void IDComponent::OnDeserialize(const MemoryMap& DataMap)
	{
		ID = DataMap.GetStringField<char>("ID");
	}

	void TagComponent::OnSerialize(MemoryMap& DataMap)
	{
		DataMap.SetStringField("Tag", Tag);
	}

	void TagComponent::OnDeserialize(const MemoryMap& DataMap)
	{
		Tag = DataMap.GetStringField<wchar_t>("Tag");
	}

	void TransformComponent::OnSerialize(MemoryMap& DataMap)
	{
		DataMap.SetField("Position", Position);
		DataMap.SetField("Rotation", Rotation);
		DataMap.SetField("Scale", Scale);
	}

	void TransformComponent::OnDeserialize(const MemoryMap& DataMap)
	{
		Position = DataMap.GetField<glm::vec3>("Position");
		Rotation = DataMap.GetField<glm::vec3>("Rotation");
		Scale = DataMap.GetField<glm::vec3>("Scale");
	}

	void SpriteRendererComponent::OnSerialize(MemoryMap& DataMap)
	{
		DataMap.SetField("Color", Color);

		DataMap.SetStringField("TextureID", TextureID);

		DataMap.SetField("TilingFactor", TilingFactor);
	}

	void SpriteRendererComponent::OnDeserialize(const MemoryMap& DataMap)
	{
		Color = DataMap.GetField<glm::vec4>("Color");

		TextureID = DataMap.GetStringField<char>("TextureID");

		if (!TextureID.empty())
			Texture = TextureManager::LoadTexture(TextureID);

		TilingFactor = DataMap.GetField<float>("TilingFactor");
	}

	void CircleRendererComponent::OnSerialize(MemoryMap& DataMap)
	{
		DataMap.SetField("Color", Color);
		DataMap.SetField("Radius", Radius);
		DataMap.SetField("Thickness", Thickness);
		DataMap.SetField("Fade", Fade);
	}

	void CircleRendererComponent::OnDeserialize(const MemoryMap& DataMap)
	{
		Color = DataMap.GetField<glm::vec4>("Color");
		Radius = DataMap.GetField<float>("Radius");
		Thickness = DataMap.GetField<float>("Thickness");
		Fade = DataMap.GetField<float>("Fade");
	}

	void CameraComponent::OnSerialize(MemoryMap& DataMap)
	{
		DataMap.SetField("SceneCamera", Camera);

		DataMap.SetField("Primary", Primary);
		DataMap.SetField("FixedAspectRatio", FixedAspectRatio);
	}

	void CameraComponent::OnDeserialize(const MemoryMap& DataMap)
	{
		SceneCamera CopyCamera = DataMap.GetField<SceneCamera>("SceneCamera");

		Camera = CopyCamera;
		Primary = DataMap.GetField<bool>("Primary");
		FixedAspectRatio = DataMap.GetField<bool>("FixedAspectRatio");
	}

	void AudioComponent::OnSerialize(MemoryMap& DataMap)
	{
		DataMap.SetStringField("AudioID", AudioID);
	}

	void AudioComponent::OnDeserialize(const MemoryMap& DataMap)
	{
		AudioID = DataMap.GetStringField<char>("AudioID");

		Ref<Asset> AudioAsset = AssetManager::LoadAsset(AudioID);

		if (AudioAsset && AudioAsset->GetInfo().Type == AssetType::Audio)
		{
			Source = AudioEngine::CreateAudioSource();

			Ref<AudioBuffer> Buff = AudioBuffer::Create();
			Buff->SetAudioEffect(Serializer::DeserializeAudio(AudioAsset->Metadata));

			Source->SetBuffer(Buff);
		}
	}

	void ScriptComponent::OnSerialize(MemoryMap& DataMap)
	{
		DataMap.SetStringField("Name", Name);
		DataMap.SetStringField("Namespace", Namespace);

		MemoryMap fieldData;
		if (Fields)
		{
			for (auto& field : *Fields)
			{
				void* Data = field.GetBufferData();
				size_t size = field.GetBufferSize();

				fieldData.SetField(field.GetName(), Data, size);
			}
		}

		DataMap.SetField("Fields", fieldData);
	}

	void ScriptComponent::OnDeserialize(const MemoryMap& DataMap)
	{
		Name = DataMap.GetStringField<char>("Name");
		Namespace = DataMap.GetStringField<char>("Namespace");

		MemoryMap& fieldData = DataMap.GetField<MemoryMap>("Fields");
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

	void Rigidbody2DComponent::OnSerialize(MemoryMap& DataMap)
	{
		DataMap.SetField("Type", Type);
		DataMap.SetField("FixedRotation", FixedRotation);
	}

	void Rigidbody2DComponent::OnDeserialize(const MemoryMap& DataMap)
	{
		Type = DataMap.GetField<BodyType>("Type");
		FixedRotation = DataMap.GetField<bool>("FixedRotation");
	}

	void BoxCollider2DComponent::OnSerialize(MemoryMap& DataMap)
	{
		DataMap.SetField("Offset", Offset);
		DataMap.SetField("Size", Size);

		DataMap.SetField("Density", Density);
		DataMap.SetField("Friction", Friction);
		DataMap.SetField("Restitution", Restitution);
		DataMap.SetField("RestitutionThreshold", RestitutionThreshold);
	}

	void BoxCollider2DComponent::OnDeserialize(const MemoryMap& DataMap)
	{
		Offset = DataMap.GetField<glm::vec2>("Offset");
		Size = DataMap.GetField<glm::vec2>("Size");

		Density = DataMap.GetField<float>("Density");
		Friction = DataMap.GetField<float>("Friction");
		Restitution = DataMap.GetField<float>("Restitution");
		RestitutionThreshold = DataMap.GetField<float>("RestitutionThreshold");
	}

	void CircleCollider2DComponent::OnSerialize(MemoryMap& DataMap)
	{
		DataMap.SetField("Offset", Offset);
		DataMap.SetField("Radius", Radius);

		DataMap.SetField("Density", Density);
		DataMap.SetField("Friction", Friction);
		DataMap.SetField("Restitution", Restitution);
		DataMap.SetField("RestitutionThreshold", RestitutionThreshold);
	}

	void CircleCollider2DComponent::OnDeserialize(const MemoryMap& DataMap)
	{
		Offset = DataMap.GetField<glm::vec2>("Offset");
		Radius = DataMap.GetField<float>("Radius");

		Density = DataMap.GetField<float>("Density");
		Friction = DataMap.GetField<float>("Friction");
		Restitution = DataMap.GetField<float>("Restitution");
		RestitutionThreshold = DataMap.GetField<float>("RestitutionThreshold");
	}
}