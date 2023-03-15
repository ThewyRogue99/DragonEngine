#include "depch.h"
#include "AssetMetadata.h"

namespace Engine
{
	AssetMetadata::~AssetMetadata()
	{
		FieldTable.Clear();
	}

	void AssetMetadata::Write(std::ostream& ss)
	{
		FieldTable.Write(ss);
	}

	void AssetMetadata::Read(std::istream& ss)
	{
		FieldTable.Read(ss);
	}

	Ref<AssetMetadata> AssetMetadata::Create()
	{
		return CreateRef<AssetMetadata>(phold{ 0 });
	}
}