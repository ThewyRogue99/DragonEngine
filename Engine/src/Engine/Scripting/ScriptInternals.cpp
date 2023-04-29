#include "depch.h"

#include "Engine/Scene/Entity.h"

#include "Engine/Scene/SceneManager.h"

#include "Engine/Audio/AudioEngine.h"
#include "ScriptEngine.h"
#include "ScriptInternals.h"

#include <unordered_map>

#include <box2d/b2_body.h>

#include <mono/metadata/object.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/exception.h>
#include <mono/metadata/appdomain.h>

namespace Engine
{
#define DE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("DragonEngine.InternalCalls::" #Name, Name)

	struct EntityInfo
	{
		unsigned int EntityHandle;
		MonoString* SceneName;
	};

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> EntityHasComponentFunctions = { };
	static std::unordered_map<MonoType*, std::function<bool(Entity)>> EntityAddComponentFunctions = { };

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
		{
			CString typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(':');
			CString structName = typeName.substr(pos + 1);
			CString managedTypename = "DragonEngine." + structName;
			char* cstr = (char*)managedTypename.c_str();

			MonoType* managedType = mono_reflection_type_from_name(cstr, (MonoImage*)ScriptEngine::GetCoreAssemblyImage());
			if (!managedType)
			{
				DE_ERROR(ScriptInternals, "Could not find component type {}", managedTypename);
				return;
			}

			EntityHasComponentFunctions[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
			EntityAddComponentFunctions[managedType] = [](Entity entity) { entity.AddComponent<Component>(); return true; };
		}(), ...);
	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptInternals::RegisterComponents()
	{
		RegisterComponent(AllComponents{});
	}

	static Entity GetEntity(const EntityInfo& Info)
	{
		const wchar_t* namestr = mono_string_to_utf16(Info.SceneName);

		Scene* SceneRef = SceneManager::GetScene(namestr);

		mono_free((void*)namestr);

		if (SceneRef)
		{
			return Entity((entt::entity)Info.EntityHandle, SceneRef);
		}

		return Entity();
	}

#pragma region Log

	static void Log_Native(MonoString* str)
	{
		const char* cstr = mono_string_to_utf8(str);

		DE_INFO(App, cstr);

		mono_free((void*)cstr);
	}

#pragma endregion

#pragma region Input

	static bool Input_IsKeyDown(KeyInput Key)
	{
		return Input::IsKeyPressed(Key);
	}

	static bool Input_IsMouseButtonDown(MouseButtonInput Button)
	{
		return Input::IsMouseButtonPressed(Button);
	}

#pragma endregion

#pragma region SceneManager

	static MonoString* SceneManager_LoadScene(MonoString* Path, MonoString* Name)
	{
		const wchar_t* pathstr = mono_string_to_utf16(Path);
		const wchar_t* namestr = mono_string_to_utf16(Name);

		Scene* LoadedScene = SceneManager::LoadScene(pathstr, namestr);

		mono_free((void*)pathstr);
		mono_free((void*)namestr);

		if (LoadedScene)
		{
			return mono_string_from_utf16(
				(mono_unichar2*)LoadedScene->GetName().c_str()
			);
		}

		return nullptr;
	}

	static MonoString* SceneManager_GetActiveSceneName()
	{
		Scene* ActiveScene = SceneManager::GetActiveScene();

		if (ActiveScene)
		{
			return mono_string_from_utf16(
				(mono_unichar2*)ActiveScene->GetName().c_str()
			);
		}

		return nullptr;
	}

	static bool SceneManager_SetActiveScene(MonoString* SceneName)
	{
		const wchar_t* namestr = mono_string_to_utf16(SceneName);
		bool result = SceneManager::SetActiveScene(namestr);

		mono_free((void*)namestr);

		return result;
	}

	static MonoArray* SceneManager_GetLoadedSceneNameArray()
	{
		const std::vector<Scene*>& SceneList = SceneManager::GetScenes();

		MonoArray* stringArray = mono_array_new(mono_domain_get(), mono_get_string_class(), SceneList.size());

		for (size_t i = 0; i < SceneList.size(); i++)
		{
			Scene* SceneRef = SceneList[i];
			const WString& SceneName = SceneRef->GetName();

			mono_array_set(
				stringArray,
				MonoString*,
				i,
				mono_string_from_utf16((mono_unichar2*)SceneName.c_str())
			);
		}

		return stringArray;
	}

#pragma endregion

#pragma region Scene

	static MonoString* Scene_CreateEntity(MonoString* SceneName, MonoString* EntityTag)
	{
		const wchar_t* namestr = mono_string_to_utf16(SceneName);
		const wchar_t* tagstr = mono_string_to_utf16(EntityTag);

		MonoString* result = nullptr;

		Scene* SceneRef = SceneManager::GetScene(namestr);
		if (SceneRef)
		{
			Entity entity = SceneRef->CreateEntity(tagstr);
			if (entity.IsValid())
			{
				WString WID = TypeUtils::FromUTF8(entity.GetUUID().GetString());
				result = mono_string_from_utf16((mono_unichar2*)WID.c_str());
			}
		}

		mono_free((void*)namestr);
		mono_free((void*)tagstr);

		return result;
	}

	static bool Scene_DestroyEntity(EntityInfo Info)
	{
		const wchar_t* namestr = mono_string_to_utf16(Info.SceneName);

		Scene* SceneRef = SceneManager::GetScene(namestr);

		mono_free((void*)namestr);

		if (SceneRef)
		{
			Entity entity((entt::entity)Info.EntityHandle, SceneRef);
			if (entity.IsValid())
			{
				SceneRef->DestroyEntity(entity);
				return true;
			}
		}

		return false;
	}

	static int Scene_GetEntityCount(MonoString* SceneName)
	{
		const wchar_t* namestr = mono_string_to_utf16(SceneName);

		Scene* SceneRef = SceneManager::GetScene(namestr);

		mono_free((void*)namestr);

		if (SceneRef)
			return (int)SceneRef->GetEntityCount();

		return 0;
	}

	static MonoArray* Scene_GetEntityHandleArray(MonoString* SceneName)
	{
		const wchar_t* namestr = mono_string_to_utf16(SceneName);

		Scene* SceneRef = SceneManager::GetScene(namestr);

		mono_free((void*)namestr);

		if (SceneRef)
		{
			std::vector<unsigned int> RegistryVector = SceneRef->GetRegistryVector();

			size_t len = RegistryVector.size();
			if (len > 0)
			{
				MonoArray* IDArray = mono_array_new(mono_domain_get(), mono_get_uint32_class(), len);

				for (size_t i = 0; i < len; i++)
				{
					mono_array_set(
						IDArray,
						unsigned int,
						i,
						RegistryVector[i]
					);
				}

				return IDArray;
			}
		}

		return nullptr;
	}

#pragma endregion

#pragma region Entity

	static bool Entity_HasComponent(EntityInfo Info, MonoReflectionType* ComponentType)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			MonoType* type = mono_reflection_type_get_type(ComponentType);

			if (type)
			{
				return EntityHasComponentFunctions.at(type)(entity);
			}
		}

		return false;
	}

	static bool Entity_AddComponent(EntityInfo Info, MonoReflectionType* ComponentType)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			MonoType* type = mono_reflection_type_get_type(ComponentType);

			if (type)
			{
				return EntityAddComponentFunctions.at(type)(entity);
			}
		}

		return false;
	}

#pragma endregion

#pragma region IDComponent

	static MonoString* IDComponent_GetID(EntityInfo Info)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			const CString& ID = entity.GetUUID().GetString();
			const WString WID = TypeUtils::FromUTF8(ID);

			return mono_string_from_utf16((mono_unichar2*)WID.c_str());
		}

		return nullptr;
	}

#pragma endregion

#pragma region TagComponent

	static MonoString* TagComponent_GetTag(EntityInfo Info)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<TagComponent>())
			{
				const auto& Tag = entity.GetComponent<TagComponent>().Tag;

				return mono_string_from_utf16((mono_unichar2*)Tag.c_str());
			}
		}

		return nullptr;
	}

	static void TagComponent_SetTag(EntityInfo Info, MonoString* value)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<TagComponent>())
			{
				auto& tagComponent = entity.GetComponent<TagComponent>();

				const wchar_t* str = mono_string_to_utf16(value);

				tagComponent.Tag = str;

				mono_free((void*)str);
			}
		}
	}

#pragma endregion

#pragma region TransformComponent

	static void TransformComponent_GetTransform(EntityInfo Info, TransformComponent* result)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				*result = entity.GetComponent<TransformComponent>();
			}
		}
	}

	static void TransformComponent_GetPosition(EntityInfo Info, glm::vec3* result)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				*result = entity.GetComponent<TransformComponent>().Position;
			}
		}
	}

	static void TransformComponent_GetRotation(EntityInfo Info, glm::vec3* result)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				*result = entity.GetComponent<TransformComponent>().Rotation;
			}
		}
	}

	static void TransformComponent_GetScale(EntityInfo Info, glm::vec3* result)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				*result = entity.GetComponent<TransformComponent>().Scale;
			}
		}
	}

	static void TransformComponent_GetTransformMatrix(EntityInfo Info, glm::mat4* result)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				*result = entity.GetComponent<TransformComponent>().GetTransformMat4();
			}
		}
	}

	static void TransformComponent_SetTransform(EntityInfo Info, TransformComponent* value)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				entity.GetComponent<TransformComponent>() = *value;
			}
		}
	}

	static void TransformComponent_SetPosition(EntityInfo Info, glm::vec3* value)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				entity.GetComponent<TransformComponent>().Position = *value;
			}
		}
	}

	static void TransformComponent_SetRotation(EntityInfo Info, glm::vec3* value)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				entity.GetComponent<TransformComponent>().Rotation = *value;
			}
		}
	}

	static void TransformComponent_SetScale(EntityInfo Info, glm::vec3* value)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				entity.GetComponent<TransformComponent>().Scale = *value;
			}
		}
	}

	static void TransformComponent_SetTransformMatrix(EntityInfo Info, glm::mat4* value)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				entity.GetComponent<TransformComponent>() = TransformComponent(*value);
			}
		}
	}

#pragma endregion

#pragma region Rigidbody2DComponent

	static void Rigidbody2DComponent_ApplyLinearImpulse(EntityInfo Info, glm::vec2* Impulse, glm::vec2* Point, bool Wake)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<Rigidbody2DComponent>())
			{
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
				b2Body* body = (b2Body*)rb2d.RuntimeBody;

				body->ApplyLinearImpulse(b2Vec2(Impulse->x, Impulse->y), b2Vec2(Point->x, Point->y), Wake);
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have Rigidbody2DComponent attached.");
				mono_raise_exception(ex);
			}
		}
		else
		{
			MonoException* ex = mono_get_exception_io("Failed to find entity in active scene.");
			mono_raise_exception(ex);
		}
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(EntityInfo Info, glm::vec2* Impulse, bool Wake)
	{
		Entity entity = GetEntity(Info);

		if (entity.IsValid())
		{
			if (entity.HasComponent<Rigidbody2DComponent>())
			{
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
				b2Body* body = (b2Body*)rb2d.RuntimeBody;

				body->ApplyLinearImpulseToCenter(b2Vec2(Impulse->x, Impulse->y), Wake);
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have Rigidbody2DComponent attached.");
				mono_raise_exception(ex);
			}
		}
		else
		{
			MonoException* ex = mono_get_exception_io("Failed to find entity in active scene.");
			mono_raise_exception(ex);
		}
	}

#pragma endregion

#pragma region AudioComponent

	static bool AudioComponent_PlayAudio(EntityInfo Info)
	{
		Entity entity = GetEntity(Info);

		if (entity.HasComponent<AudioComponent>())
		{
			auto& ac = entity.GetComponent<AudioComponent>();
			AudioEngine::PlayAudio(ac.Source);

			return true;
		}

		return false;
	}

	static bool AudioComponent_StopAudio(EntityInfo Info)
	{
		Entity entity = GetEntity(Info);

		if (entity.HasComponent<AudioComponent>())
		{
			auto& ac = entity.GetComponent<AudioComponent>();
			AudioEngine::StopAudio(ac.Source);

			return true;
		}

		return false;
	}

	static bool AudioComponent_IsPlaying(EntityInfo Info)
	{
		Entity entity = GetEntity(Info);

		if (entity.HasComponent<AudioComponent>())
		{
			auto& ac = entity.GetComponent<AudioComponent>();
			return AudioEngine::IsPlaying(ac.Source);
		}

		return false;
	}

#pragma endregion

	void ScriptInternals::RegisterFunctions()
	{
		// Log
		DE_ADD_INTERNAL_CALL(Log_Native);

		// Input
		DE_ADD_INTERNAL_CALL(Input_IsKeyDown);
		DE_ADD_INTERNAL_CALL(Input_IsKeyDown);

		// SceneManager
		DE_ADD_INTERNAL_CALL(SceneManager_LoadScene);
		DE_ADD_INTERNAL_CALL(SceneManager_GetActiveSceneName);
		DE_ADD_INTERNAL_CALL(SceneManager_SetActiveScene);
		DE_ADD_INTERNAL_CALL(SceneManager_GetLoadedSceneNameArray);

		// Scene
		DE_ADD_INTERNAL_CALL(Scene_CreateEntity);
		DE_ADD_INTERNAL_CALL(Scene_DestroyEntity);
		DE_ADD_INTERNAL_CALL(Scene_GetEntityCount);
		DE_ADD_INTERNAL_CALL(Scene_GetEntityHandleArray);

		// Entity
		DE_ADD_INTERNAL_CALL(Entity_HasComponent);
		DE_ADD_INTERNAL_CALL(Entity_AddComponent);

		// IDComponent
		DE_ADD_INTERNAL_CALL(IDComponent_GetID);

		// TagComponent
		DE_ADD_INTERNAL_CALL(TagComponent_GetTag);
		DE_ADD_INTERNAL_CALL(TagComponent_SetTag);

		// TransformComponent
		DE_ADD_INTERNAL_CALL(TransformComponent_GetTransform);
		DE_ADD_INTERNAL_CALL(TransformComponent_GetPosition);
		DE_ADD_INTERNAL_CALL(TransformComponent_GetRotation);
		DE_ADD_INTERNAL_CALL(TransformComponent_GetScale);
		DE_ADD_INTERNAL_CALL(TransformComponent_GetTransformMatrix);

		DE_ADD_INTERNAL_CALL(TransformComponent_SetTransform);
		DE_ADD_INTERNAL_CALL(TransformComponent_SetPosition);
		DE_ADD_INTERNAL_CALL(TransformComponent_SetRotation);
		DE_ADD_INTERNAL_CALL(TransformComponent_SetScale);
		DE_ADD_INTERNAL_CALL(TransformComponent_SetTransformMatrix);

		// Rigidbody2DComponent
		DE_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		DE_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

		// AudioComponent
		DE_ADD_INTERNAL_CALL(AudioComponent_PlayAudio);
		DE_ADD_INTERNAL_CALL(AudioComponent_StopAudio);
	}
}