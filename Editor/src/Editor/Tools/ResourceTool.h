#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "Engine/Renderer/Texture.h"

namespace Engine
{
	class ResourceTool
	{
	public:
		static void LoadResources();

		static void LoadIcons();

		static Ref<Texture2D> GetIcon(const CString& IconName);
	};
}