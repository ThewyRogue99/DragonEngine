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
			
			return AssetType::Undefined;
		}
	}
}