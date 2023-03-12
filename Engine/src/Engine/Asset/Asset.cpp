#include "depch.h"
#include "Asset.h"

namespace Engine
{
	void Asset::Write(std::ostream& ss)
	{
		if (Metadata)
		{
			Metadata->Write(ss);
		}
	}

	void Asset::Read(std::istream& ss)
	{
		if (Metadata)
		{
			Metadata->Read(ss);
		}
	}
}
