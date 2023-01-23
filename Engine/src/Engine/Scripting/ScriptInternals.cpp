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

namespace Engine
{
#define DE_ADD_INTERNAL_CALL(Name) mono_add_internal_call("DragonEngine.InternalCalls::" #Name, Name)

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> EntityHasComponentFunctions = { };

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
		{
			std::string typeName = typeid(Component).name();
			size_t pos = typeName.find_last_of(':');
			std::string structName = typeName.substr(pos + 1);
			std::string managedTypename = "DragonEngine." + structName;
			char* cstr = (char*)managedTypename.c_str();

			MonoType* managedType = mono_reflection_type_from_name(cstr, (MonoImage*)ScriptEngine::GetCoreAssemblyImage());
			if (!managedType)
			{
				DE_ERROR(ScriptInternals, "Could not find component type {}", managedTypename);
				return;
			}

			EntityHasComponentFunctions[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
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

	static Entity GetEntity(UUID id)
	{
		Scene* scene = SceneManager::GetActiveScene();
		
		if (scene)
			return scene->GetEntityWithUUID(id);

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
				CString WID = TypeUtils::FromUTF8(entity.GetUUID().GetString());
				result = mono_string_from_utf16((mono_unichar2*)WID.c_str());
			}
		}

		mono_free((void*)namestr);
		mono_free((void*)tagstr);

		return result;
	}

	static bool Scene_DestroyEntity(MonoString* SceneName, MonoString* EntityID)
	{
		const wchar_t* namestr = mono_string_to_utf16(SceneName);
		const char* idstr = mono_string_to_utf8(EntityID);

		bool result = false;

		Scene* SceneRef = SceneManager::GetScene(namestr);
		if (SceneRef)
		{
			Entity entity = SceneRef->GetEntityWithUUID(idstr);
			if (entity.IsValid())
			{
				SceneRef->DestroyEntity(entity);
				result = true;
			}
		}

		mono_free((void*)namestr);
		mono_free((void*)idstr);

		return result;
	}

	static int Scene_GetEntityCount(MonoString* SceneName)
	{
		const wchar_t* namestr = mono_string_to_utf16(SceneName);

		Scene* SceneRef = SceneManager::GetScene(namestr);

		int result = 0;

		if (SceneRef)
			result = SceneRef->GetEntityCount();

		mono_free((void*)namestr);

		return result;
	}

	static void Scene_GetEntityIDList(MonoString* SceneName, MonoArray** IDArray)
	{
		const wchar_t* namestr = mono_string_to_utf16(SceneName);

		Scene* SceneRef = SceneManager::GetScene(namestr);

		if (SceneRef)
		{
			std::vector<Entity> EntityList = SceneRef->GetEntities();
			uintptr_t len = mono_array_length(*IDArray);

			if (len == EntityList.size())
			{
				for (uintptr_t i = 0; i < len; i++)
				{
					Entity entity = EntityList[i];
					std::string entityID = entity.GetUUID().GetString();
					CString WID = TypeUtils::FromUTF8(entityID);

					mono_array_set(
						*IDArray,
						MonoString*,
						i,
						mono_string_from_utf16((mono_unichar2*)WID.c_str())
					);
				}
			}
		}

		mono_free((void*)namestr);
	}

#pragma endregion

#pragma region Entity

	static bool Entity_HasComponent(MonoString* EntityID, MonoReflectionType* ComponentType)
	{
		Entity entity = GetEntity(mono_string_to_utf8(EntityID));

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

#pragma endregion

#pragma region TagComponent

	static MonoString* TagComponent_GetTag(MonoString* EntityID)
	{
		auto entity = GetEntity(mono_string_to_utf8(EntityID));
		if (entity.IsValid())
		{
			if (entity.HasComponent<TagComponent>())
			{
				const auto& tagComponent = entity.GetComponent<TagComponent>();

				return mono_string_from_utf16((mono_unichar2*)tagComponent.Tag.c_str());
			}

			MonoException* ex = mono_get_exception_io("Entity doesn't have TagComponent attached.");
			mono_raise_exception(ex);

			return nullptr;
		}

		MonoException* ex = mono_get_exception_io("Failed to find entity in active scene.");
		mono_raise_exception(ex);

		return nullptr;
	}

	static void TagComponent_SetTag(MonoString* EntityID, MonoString* value)
	{
		auto entity = GetEntity(mono_string_to_utf8(EntityID));
		if (entity.IsValid())
		{
			if (entity.HasComponent<TagComponent>())
			{
				auto& tagComponent = entity.GetComponent<TagComponent>();

				const wchar_t* str = mono_string_to_utf16(value);

				tagComponent.Tag = str;
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have TagComponent attached.");
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

#pragma region TransformComponent

	static void TransformComponent_GetTransform(MonoString* EntityID, TransformComponent* result)
	{
		auto entity = GetEntity(mono_string_to_utf8(EntityID));
		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				*result = entity.GetComponent<TransformComponent>();
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have TransformComponent attached.");
				mono_raise_exception(ex);
			}
		}
		else
		{
			MonoException* ex = mono_get_exception_io("Failed to find entity in active scene.");
			mono_raise_exception(ex);
		}
	}

	static void TransformComponent_GetPosition(MonoString* EntityID, glm::vec3* result)
	{
		auto entity = GetEntity(mono_string_to_utf8(EntityID));
		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				*result = entity.GetComponent<TransformComponent>().Position;
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have TransformComponent attached.");
				mono_raise_exception(ex);
			}
		}
		else
		{
			MonoException* ex = mono_get_exception_io("Failed to find entity in active scene.");
			mono_raise_exception(ex);
		}
	}

	static void TransformComponent_GetRotation(MonoString* EntityID, glm::vec3* result)
	{
		auto entity = GetEntity(mono_string_to_utf8(EntityID));
		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				*result = entity.GetComponent<TransformComponent>().Rotation;
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have TransformComponent attached.");
				mono_raise_exception(ex);
			}
		}
		else
		{
			MonoException* ex = mono_get_exception_io("Failed to find entity in active scene.");
			mono_raise_exception(ex);
		}
	}

	static void TransformComponent_GetScale(MonoString* EntityID, glm::vec3* result)
	{
		auto entity = GetEntity(mono_string_to_utf8(EntityID));
		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				*result = entity.GetComponent<TransformComponent>().Scale;
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have TransformComponent attached.");
				mono_raise_exception(ex);
			}
		}
		else
		{
			MonoException* ex = mono_get_exception_io("Failed to find entity in active scene.");
			mono_raise_exception(ex);
		}
	}

	static void TransformComponent_GetTransformMatrix(MonoString* EntityID, glm::mat4* result)
	{
		auto entity = GetEntity(mono_string_to_utf8(EntityID));
		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				*result = entity.GetComponent<TransformComponent>().GetTransformMat4();
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have TransformComponent attached.");
				mono_raise_exception(ex);
			}
		}
		else
		{
			MonoException* ex = mono_get_exception_io("Failed to find entity in active scene.");
			mono_raise_exception(ex);
		}
	}

	static void TransformComponent_SetTransform(MonoString* EntityID, TransformComponent* value)
	{
		auto entity = GetEntity(mono_string_to_utf8(EntityID));
		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				entity.GetComponent<TransformComponent>() = *value;
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have TransformComponent attached.");
				mono_raise_exception(ex);
			}
		}
		else
		{
			MonoException* ex = mono_get_exception_io("Failed to find entity in active scene.");
			mono_raise_exception(ex);
		}
	}

	static void TransformComponent_SetPosition(MonoString* EntityID, glm::vec3* value)
	{
		auto entity = GetEntity(mono_string_to_utf8(EntityID));
		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				entity.GetComponent<TransformComponent>().Position = *value;
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have TransformComponent attached.");
				mono_raise_exception(ex);
			}
		}
		else
		{
			MonoException* ex = mono_get_exception_io("Failed to find entity in active scene.");
			mono_raise_exception(ex);
		}
	}

	static void TransformComponent_SetRotation(MonoString* EntityID, glm::vec3* value)
	{
		auto entity = GetEntity(mono_string_to_utf8(EntityID));
		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				entity.GetComponent<TransformComponent>().Rotation = *value;
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have TransformComponent attached.");
				mono_raise_exception(ex);
			}
		}
		else
		{
			MonoException* ex = mono_get_exception_io("Failed to find entity in active scene.");
			mono_raise_exception(ex);
		}
	}

	static void TransformComponent_SetScale(MonoString* EntityID, glm::vec3* value)
	{
		auto entity = GetEntity(mono_string_to_utf8(EntityID));
		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				entity.GetComponent<TransformComponent>().Scale = *value;
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have TransformComponent attached.");
				mono_raise_exception(ex);
			}
		}
		else
		{
			MonoException* ex = mono_get_exception_io("Failed to find entity in active scene.");
			mono_raise_exception(ex);
		}
	}

	static void TransformComponent_SetTransformMatrix(MonoString* EntityID, glm::mat4* value)
	{
		auto entity = GetEntity(mono_string_to_utf8(EntityID));
		if (entity.IsValid())
		{
			if (entity.HasComponent<TransformComponent>())
			{
				entity.GetComponent<TransformComponent>() = TransformComponent(*value);
			}
			else
			{
				MonoException* ex = mono_get_exception_io("Entity doesn't have TransformComponent attached.");
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

#pragma region Rigidbody2DComponent

	static void Rigidbody2DComponent_ApplyLinearImpulse(MonoString* EntityID, glm::vec2* Impulse, glm::vec2* Point, bool Wake)
	{
		Entity entity = GetEntity(mono_string_to_utf8(EntityID));

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

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(MonoString* EntityID, glm::vec2* Impulse, bool Wake)
	{
		Entity entity = GetEntity(mono_string_to_utf8(EntityID));

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

	static bool AudioComponent_PlayAudio(MonoString* EntityID)
	{
		Entity entity = GetEntity(mono_string_to_utf8(EntityID));

		if (entity.HasComponent<AudioComponent>())
		{
			auto& ac = entity.GetComponent<AudioComponent>();
			AudioEngine::PlayAudio(ac.Source);

			return true;
		}

		return false;
	}

	static bool AudioComponent_StopAudio(MonoString* EntityID)
	{
		Entity entity = GetEntity(mono_string_to_utf8(EntityID));

		if (entity.HasComponent<AudioComponent>())
		{
			auto& ac = entity.GetComponent<AudioComponent>();
			AudioEngine::StopAudio(ac.Source);

			return true;
		}

		return false;
	}

	static bool AudioComponent_IsPlaying(MonoString* EntityID)
	{
		Entity entity = GetEntity(mono_string_to_utf8(EntityID));

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

		// Scene
		DE_ADD_INTERNAL_CALL(Scene_CreateEntity);
		DE_ADD_INTERNAL_CALL(Scene_DestroyEntity);
		DE_ADD_INTERNAL_CALL(Scene_GetEntityCount);
		DE_ADD_INTERNAL_CALL(Scene_GetEntityIDList);

		// Entity
		DE_ADD_INTERNAL_CALL(Entity_HasComponent);

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