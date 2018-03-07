#pragma once

#include "SFML\System\Utf.hpp"
#include <iterator>
#include <locale>
#include <string>
#include <cstring>
#include "pystring.h"

#ifdef _DEBUG
#define _DEBUG_STRING
#endif

namespace ok
{
	class  String
	{
	public:
		typedef std::basic_string<sf::Uint32>::iterator       Iterator;
		typedef std::basic_string<sf::Uint32>::const_iterator ConstIterator;

		static const std::size_t InvalidPos;

		String();
		String(char ansiChar, const std::locale& locale = std::locale());
		String(wchar_t wideChar);
		String(sf::Uint32 utf32Char);
		String(const char* ansiString, const std::locale& locale = std::locale());
		String(const std::string& ansiString, const std::locale& locale = std::locale());
		String(const wchar_t* wideString);
		String(const std::wstring& wideString);
		String(const sf::Uint32* utf32String);
		String(const std::basic_string<sf::Uint32>& utf32String);
		String(const ok::String& copy);

		template <typename T>
		static ok::String fromUtf8(T begin, T end);

		template <typename T>
		static ok::String fromUtf16(T begin, T end);

		template <typename T>
		static ok::String fromUtf32(T begin, T end);

		operator std::string() const;
		operator std::wstring() const;
		operator sf::String() const;

		std::vector<ok::String> ok::String::split_whitespace(int maxsplit = -1);
		std::vector<ok::String> split(const ok::String & sep, int maxsplit = -1);
		ok::String get_folder();
		ok::String get_file_name_ext();
		ok::String get_file_ext();

		std::string toAnsiString(const std::locale& locale = std::locale()) const;
		std::wstring toWideString() const;
		std::basic_string<sf::Uint8> toUtf8() const;
		std::basic_string<sf::Uint16> toUtf16() const;
		std::basic_string<sf::Uint32> toUtf32() const;
		ok::String& operator =(const ok::String& right);
		ok::String& operator +=(const ok::String& right);
		sf::Uint32 operator [](std::size_t index) const;
		sf::Uint32& operator [](std::size_t index);
		void clear();
		std::size_t getSize() const;
		bool isEmpty() const;
		void erase(std::size_t position, std::size_t count = 1);
		void insert(std::size_t position, const ok::String& str);
		std::size_t find(const ok::String& str, std::size_t start = 0) const;
		void replace(std::size_t position, std::size_t length, const ok::String& replaceWith);
		void replace(const ok::String& searchFor, const ok::String& replaceWith);
		ok::String substring(std::size_t position, std::size_t length = InvalidPos) const;
		const sf::Uint32* getData() const;
		Iterator begin();
		ConstIterator begin() const;
		Iterator end();
		ConstIterator end() const;

	private:

		friend  bool operator ==(const ok::String& left, const ok::String& right);
		friend  bool operator <(const ok::String& left, const ok::String& right);

		std::basic_string<sf::Uint32> m_string;

		#ifdef _DEBUG_STRING
		std::string m_string_debug;
		#endif
	};

	bool operator ==(const ok::String& left, const ok::String& right);
	bool operator !=(const ok::String& left, const ok::String& right);
	bool operator <(const ok::String& left, const ok::String& right);
	bool operator >(const ok::String& left, const ok::String& right);
	bool operator <=(const ok::String& left, const ok::String& right);
	bool operator >=(const ok::String& left, const ok::String& right);
	ok::String operator +(const ok::String& left, const ok::String& right);

	template <typename T>
	ok::String ok::String::fromUtf8(T begin, T end)
	{
		ok::String string;
		Utf8::toUtf32(begin, end, std::back_inserter(string.m_string));
		return string;
	}

	template <typename T>
	ok::String ok::String::fromUtf16(T begin, T end)
	{
		ok::String string;
		Utf16::toUtf32(begin, end, std::back_inserter(string.m_string));
		return string;
	}

	template <typename T>
	ok::String ok::String::fromUtf32(T begin, T end)
	{
		ok::String string;
		string.m_string.assign(begin, end);
		return string;
	}
}
