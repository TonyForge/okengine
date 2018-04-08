#pragma once

namespace Zoner
{
	class UID
	{
	public:
		UID();

		void acquire_uid();

		bool cmp_uid(const Zoner::UID& other);

		static const unsigned int uid_length = 4;
		unsigned int uid[uid_length];
		static unsigned int global_uid_next[uid_length]; //must be saved at save game and loaded back at load game
	};

	bool operator ==(const Zoner::UID& left, const Zoner::UID& right);
}