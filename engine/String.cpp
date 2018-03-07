#include "String.h"

const std::size_t ok::String::InvalidPos = std::basic_string<sf::Uint32>::npos;

ok::String::String()
{
}

ok::String::String(char ansiChar, const std::locale& locale)
{
	m_string += sf::Utf32::decodeAnsi(ansiChar, locale);

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

ok::String::String(wchar_t wideChar)
{
	m_string += sf::Utf32::decodeWide(wideChar);

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

ok::String::String(sf::Uint32 utf32Char)
{
	m_string += utf32Char;

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

ok::String::String(const char* ansiString, const std::locale& locale)
{
	if (ansiString)
	{
		std::size_t length = strlen(ansiString);
		if (length > 0)
		{
			m_string.reserve(length + 1);
			sf::Utf32::fromAnsi(ansiString, ansiString + length, std::back_inserter(m_string), locale);
		}
	}

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

ok::String::String(const std::string& ansiString, const std::locale& locale)
{
	m_string.reserve(ansiString.length() + 1);
	sf::Utf32::fromAnsi(ansiString.begin(), ansiString.end(), std::back_inserter(m_string), locale);

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

ok::String::String(const wchar_t* wideString)
{
	if (wideString)
	{
		std::size_t length = std::wcslen(wideString);
		if (length > 0)
		{
			m_string.reserve(length + 1);
			sf::Utf32::fromWide(wideString, wideString + length, std::back_inserter(m_string));
		}
	}

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

ok::String::String(const std::wstring& wideString)
{
	m_string.reserve(wideString.length() + 1);
	sf::Utf32::fromWide(wideString.begin(), wideString.end(), std::back_inserter(m_string));

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

ok::String::String(const sf::Uint32* utf32String)
{
	if (utf32String)
		m_string = utf32String;

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

ok::String::String(const std::basic_string<sf::Uint32>& utf32String) :
	m_string(utf32String)
{
	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

ok::String::String(const ok::String& copy) :
	m_string(copy.m_string)
{
	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

ok::String::operator std::string() const
{
	return toAnsiString();
}

ok::String::operator std::wstring() const
{
	return toWideString();
}

ok::String::operator sf::String() const
{
	return sf::String(m_string);
}

std::vector<ok::String> ok::String::split_whitespace(int maxsplit)
{
	std::vector< ok::String > result;

	std::string::size_type i, j, len = getSize();
	for (i = j = 0; i < len; )
	{

		while (i < len && ::isspace(m_string[i])) i++;
		j = i;

		while (i < len && !::isspace(m_string[i])) i++;



		if (j < i)
		{
			if (maxsplit-- <= 0) break;

			result.push_back(substring(j, i - j));

			while (i < len && ::isspace(m_string[i])) i++;
			j = i;
		}
	}
	if (j < len)
	{
		result.push_back(substring(j, len - j));
	}

	return result;
}

std::vector<ok::String> ok::String::split(const ok::String & sep, int maxsplit)
{
	std::vector< ok::String > result;
	result.clear();

	if (maxsplit < 0) maxsplit = MAX_32BIT_INT;//result.max_size();


	if (sep.getSize() == 0)
	{
		return split_whitespace(maxsplit);
	}

	size_t i, j, len = getSize(), n = sep.getSize();

	i = j = 0;

	while (i + n <= len)
	{
		if (m_string[i] == sep[0] && substring(i, n) == sep)
		{
			if (maxsplit-- <= 0) break;

			result.push_back(substring(j, i - j));
			i = j = i + n;
		}
		else
		{
			i++;
		}
	}

	result.push_back(substring(j, len - j));

	return result;
}

ok::String ok::String::get_folder()
{
	std::string str = toAnsiString();

	size_t pos = str.find_last_of("/\\");
	if (pos == std::string::npos)
	{
		return "";
	}

	return ok::String(str.substr(0, pos));
}

ok::String ok::String::get_file_name_ext()
{
	std::string str = toAnsiString();
	size_t left_range = str.find_last_of("/\\");

	if (left_range == std::string::npos)
	{
		return *this;
	}

	left_range += 1;

	return ok::String(str.substr(left_range, str.length() - left_range));
}

ok::String ok::String::get_file_ext()
{
	std::string str = toAnsiString();
	size_t left_range = str.find_last_of(".");

	if (left_range == std::string::npos)
	{
		return "";
	}

	left_range += 1;
	return ok::String(str.substr(left_range, str.length() - left_range));
}

std::string ok::String::toAnsiString(const std::locale& locale) const
{
	// Prepare the output string
	std::string output;
	output.reserve(m_string.length() + 1);

	// Convert
	sf::Utf32::toAnsi(m_string.begin(), m_string.end(), std::back_inserter(output), 0, locale);

	return output;
}

std::wstring ok::String::toWideString() const
{
	// Prepare the output string
	std::wstring output;
	output.reserve(m_string.length() + 1);

	// Convert
	sf::Utf32::toWide(m_string.begin(), m_string.end(), std::back_inserter(output), 0);

	return output;
}

std::basic_string<sf::Uint8> ok::String::toUtf8() const
{
	// Prepare the output string
	std::basic_string<sf::Uint8> output;
	output.reserve(m_string.length());

	// Convert
	sf::Utf32::toUtf8(m_string.begin(), m_string.end(), std::back_inserter(output));

	return output;
}

std::basic_string<sf::Uint16> ok::String::toUtf16() const
{
	// Prepare the output string
	std::basic_string<sf::Uint16> output;
	output.reserve(m_string.length());

	// Convert
	sf::Utf32::toUtf16(m_string.begin(), m_string.end(), std::back_inserter(output));

	return output;
}

std::basic_string<sf::Uint32> ok::String::toUtf32() const
{
	return m_string;
}

ok::String& ok::String::operator =(const ok::String& right)
{
	m_string = right.m_string;

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif

	return *this;
}

ok::String& ok::String::operator +=(const ok::String& right)
{
	m_string += right.m_string;

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif

	return *this;
}

sf::Uint32 ok::String::operator [](std::size_t index) const
{
	return m_string[index];
}

sf::Uint32& ok::String::operator [](std::size_t index)
{
	return m_string[index];
}

void ok::String::clear()
{
	m_string.clear();

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

std::size_t ok::String::getSize() const
{
	return m_string.size();
}

bool ok::String::isEmpty() const
{
	return m_string.empty();
}

void ok::String::erase(std::size_t position, std::size_t count)
{
	m_string.erase(position, count);

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

void ok::String::insert(std::size_t position, const ok::String& str)
{
	m_string.insert(position, str.m_string);

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

std::size_t ok::String::find(const ok::String& str, std::size_t start) const
{
	return m_string.find(str.m_string, start);
}

void ok::String::replace(std::size_t position, std::size_t length, const ok::String& replaceWith)
{
	m_string.replace(position, length, replaceWith.m_string);

	#ifdef _DEBUG_STRING
		m_string_debug = *this;
	#endif
}

void ok::String::replace(const ok::String& searchFor, const ok::String& replaceWith)
{
	std::size_t step = replaceWith.getSize();
	std::size_t len = searchFor.getSize();
	std::size_t pos = find(searchFor);

	// Replace each occurrence of search
	while (pos != InvalidPos)
	{
		replace(pos, len, replaceWith);
		pos = find(searchFor, pos + step);
	}
}

ok::String ok::String::substring(std::size_t position, std::size_t length) const
{
	return m_string.substr(position, length);
}

const sf::Uint32* ok::String::getData() const
{
	return m_string.c_str();
}

ok::String::Iterator ok::String::begin()
{
	return m_string.begin();
}

ok::String::ConstIterator ok::String::begin() const
{
	return m_string.begin();
}

ok::String::Iterator ok::String::end()
{
	return m_string.end();
}

ok::String::ConstIterator ok::String::end() const
{
	return m_string.end();
}

bool ok::operator ==(const ok::String& left, const ok::String& right)
{
	return left.m_string == right.m_string;
}

bool ok::operator !=(const ok::String& left, const ok::String& right)
{
	return !(left == right);
}

bool ok::operator <(const ok::String& left, const ok::String& right)
{
	return left.m_string < right.m_string;
}

bool ok::operator >(const ok::String& left, const ok::String& right)
{
	return right < left;
}

bool ok::operator <=(const ok::String& left, const ok::String& right)
{
	return !(right < left);
}

bool ok::operator >=(const ok::String& left, const ok::String& right)
{
	return !(left < right);
}

ok::String ok::operator +(const ok::String& left, const ok::String& right)
{
	ok::String string = left;
	string += right;

	return string;
}
