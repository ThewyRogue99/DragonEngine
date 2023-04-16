#pragma once

#include "Engine/Core/Core.h"
#include "Engine/Types/Types.h"
#include "Engine/Renderer/Texture.h"

struct ImFont;

namespace Engine
{
	enum class FontIconType
	{
		FontAwesome6_Regular
	};

	class ResourceTool
	{
	public:
		static void LoadResources();

		static void LoadFontIcons();
		static void LoadImageIcons();

		static Ref<Texture2D> GetImageIcon(const WString& IconName);
		static ImFont* GetFontIcon(FontIconType Type);
	};
}