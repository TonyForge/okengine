#pragma once

namespace Zoner
{
	namespace Requests
	{
		const int JumpHole_Destination = 1000;
		const int JumpHole_DestinationPosition = 1001;
	}
	

	class RequestsDispatcher
	{
	public:
		virtual void* GetPtr(int id) { return nullptr; };
	};
}