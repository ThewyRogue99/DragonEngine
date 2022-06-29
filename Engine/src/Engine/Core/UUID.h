#pragma once
#include <xhash>

namespace Engine
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;

	private:
		uint64_t m_UUID = 0;
	};
}