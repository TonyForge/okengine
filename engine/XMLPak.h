#pragma once

#include "String.h"
#include "tinyxml\tinyxml2.h"

namespace ok
{
	class XMLPakWriter
	{
	public:
		void Begin(ok::String& path);
		void Push(tinyxml2::XMLDocument& document, ok::String& document_name);
		void End();
	private:
		std::ofstream _out;
		tinyxml2::XMLDocument _header;
		bool _lock = false;
		ok::String* _path;
		tinyxml2::XMLPrinter _printer;

	protected:
	};

	class XMLPakReader
	{
	public:
		void Begin(ok::String& path);
		void Pop(tinyxml2::XMLDocument& out_document, ok::String& document_name);
		void End();
	private:
		bool _lock = false;
		std::ifstream _in;
		ok::String* _path;
		tinyxml2::XMLDocument _header;
		std::vector<char>* _buffer;
	protected:
	};
}