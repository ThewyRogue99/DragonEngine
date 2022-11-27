#include "depch.h"
#include "AssetUtils.h"

namespace Engine
{
	namespace AssetUtils
	{
		AssetType GetAssetTypeFromExtension(const CString& Extension)
		{
			// ---------- Texture ----------
			if (
				Extension == L".jpg" ||
				Extension == L".png" ||
				Extension == L".tga" ||
				Extension == L".bmp" ||
				Extension == L".psd" ||
				Extension == L".gif" ||
				Extension == L".hdr" ||
				Extension == L".pic"
				) {
				return AssetType::Texture;
			}
			else if (Extension == L".cs")
				return AssetType::Script;
			
			return AssetType::Undefined;
		}
	}
}