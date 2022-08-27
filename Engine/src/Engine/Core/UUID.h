#pragma once
#include <string>

namespace Engine
{
	class UUID
	{
	public:
		UUID();
		UUID(const std::string& str) : ID(str) { }
		UUID(const char* str) : ID(str) { }
		UUID(const UUID& id) : ID(id.ID) { }

		const std::string& GetString() const { return ID; }

		bool operator==(const UUID& id) { return id.ID == ID; }

	private:
		std::string ID;
	};
}