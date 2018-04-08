#include "UID.h"

unsigned int Zoner::UID::global_uid_next[Zoner::UID::uid_length] = { 0,0,0,0 };

Zoner::UID::UID()
{
}

void Zoner::UID::acquire_uid()
{
	memcpy(uid, global_uid_next, sizeof(uid));

	int i = 0;

	while (true)
	{
		global_uid_next[i]++;
		if (global_uid_next[i] == UINT_MAX)
		{
			global_uid_next[i] = 0;
			i++;
			if (i == uid_length) break;
		}
		else
		{
			break;
		}
	}
}

bool Zoner::UID::cmp_uid(const Zoner::UID & other)
{
	return *this == other;
}

bool Zoner::operator==(const Zoner::UID & left, const Zoner::UID & right)
{
	return memcmp(left.uid, right.uid, sizeof(unsigned int)*Zoner::UID::uid_length) == 0;
}