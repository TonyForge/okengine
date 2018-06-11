#pragma once


namespace Zoner
{
	class UID
	{
	public:
		UID();
		Zoner::UID& operator =(const unsigned int& value);

		void acquire_uid();

		bool cmp_uid(const Zoner::UID& other);

		void save_uid_to_xml(tinyxml2::XMLElement& element);
		void load_uid_from_xml(tinyxml2::XMLElement& element);

		static void save_global_uid_to_xml(tinyxml2::XMLElement& element);
		static void load_global_uid_from_xml(tinyxml2::XMLElement& element);

		static const unsigned int uid_length = 4;
		unsigned int uid[uid_length];
		static unsigned int global_uid_next[uid_length]; //must be saved at save game and loaded back at load game
	};

	bool operator ==(const Zoner::UID& left, const Zoner::UID& right);
	bool operator ==(const Zoner::UID& left, const unsigned int& right);
}

namespace std
{
	template <> struct less<Zoner::UID>
	{
		inline bool operator() (const Zoner::UID& lhs, const Zoner::UID& rhs) const
		{
			for (int i = 3; i > -1; i--)
			{
				if (lhs.uid[i] < rhs.uid[i]) return true;
				else if (lhs.uid[i] == rhs.uid[i]) {}
				else return false;
			}

			return false;
		}
	};
}
