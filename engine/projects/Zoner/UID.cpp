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

void Zoner::UID::save_uid_to_xml(tinyxml2::XMLElement & element)
{
	for (int i = 0; i < uid_length; i++)
	{
		element.SetAttribute(("id"+std::to_string(i)).c_str(), uid[i]);
	}
}

void Zoner::UID::load_uid_from_xml(tinyxml2::XMLElement & element)
{
	for (int i = 0; i < uid_length; i++)
	{
		uid[i] = element.UnsignedAttribute(("id" + std::to_string(i)).c_str());
	}
}

void Zoner::UID::save_global_uid_to_xml(tinyxml2::XMLElement & element)
{
	for (int i = 0; i < uid_length; i++)
	{
		element.SetAttribute(("id" + std::to_string(i)).c_str(), global_uid_next[i]);
	}
}

void Zoner::UID::load_global_uid_from_xml(tinyxml2::XMLElement & element)
{
	for (int i = 0; i < uid_length; i++)
	{
		global_uid_next[i] = element.UnsignedAttribute(("id" + std::to_string(i)).c_str());
	}
}

bool Zoner::operator==(const Zoner::UID & left, const Zoner::UID & right)
{
	return memcmp(left.uid, right.uid, sizeof(unsigned int)*Zoner::UID::uid_length) == 0;
}