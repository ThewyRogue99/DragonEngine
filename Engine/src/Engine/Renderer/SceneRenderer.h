#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Scene/Scene.h"

namespace Engine
{
	class Framebuffer;

	class SceneRenderer
	{
	public:
		static void BeginScene(Scene* SceneRef, Ref<Framebuffer> FramebufferRef);
		static void EndScene();

		static void Render();
	};
}