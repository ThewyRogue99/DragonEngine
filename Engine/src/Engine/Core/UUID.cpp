#include "depch.h"
#include "UUID.h"

#include <random>

namespace Engine
{
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	UUID::UUID() : m_UUID(s_UniformDistribution(s_Engine))
	{

	}

	UUID::UUID(uint64_t uuid) : m_UUID(uuid)
	{

	}
}

namespace std
{
	template<>
	struct hash<Engine::UUID>
	{
		std::size_t operator()(const Engine::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}