#include "depch.h"
#include "UUID.h"

#include <uuid_v4.h>
#include <endianness.h>

namespace Engine
{
	static UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;

	UUID::UUID()
	{
		UUIDv4::UUID id = uuidGenerator.getUUID();

		id.str(ID);
	}
}