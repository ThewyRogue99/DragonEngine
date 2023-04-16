#pragma once
#include "Engine/Types/Types.h"

namespace Engine
{
	class UUID
	{
	public:
		ENGINE_API UUID();
		UUID(const CString& str) : ID(str) { }
		UUID(const char* str) : ID(str) { }
		UUID(const UUID& id) : ID(id.ID) { }

		const CString& GetString() const { return ID; }

		bool operator==(const UUID& id) { return id.ID == ID; }

	private:
		CString ID;
	};
}