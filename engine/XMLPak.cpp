#include "XMLPak.h"

void ok::XMLPakWriter::Begin(ok::String & path)
{
	if (_lock) return;

	_path = &path;

	_out.open((const std::string&)path+".pak.xml", std::ofstream::in | std::ofstream::binary);
	_header.Clear();
	
	_header.InsertEndChild(_header.NewElement("xmlpak"));

	_lock = true;
}

void ok::XMLPakWriter::Push(tinyxml2::XMLDocument & document, ok::String & document_name)
{
	if (!_lock) return;

	_printer.ClearBuffer();
	document.Print(&_printer);

	auto buf = _printer.CStr();
	auto buf_size = _printer.CStrSize();

	auto record = _header.NewElement(document_name.toAnsiString().c_str());

	record->SetAttribute("position", _out.tellp());
	record->SetAttribute("length", buf_size);

	_header.FirstChildElement("xmlpak")->InsertEndChild(record);

	_out.write(buf, buf_size);
}

void ok::XMLPakWriter::End()
{
	if (!_lock) return;

	_out.close();
	_header.SaveFile(((const std::string&)*_path + ".header.xml").c_str());
	_header.Clear();
	_printer.ClearBuffer();

	_lock = false;
}

void ok::XMLPakReader::Begin(ok::String & path)
{
	if (_lock) return;

	_path = &path;

	_in.open((const std::string&)path + ".pak.xml", std::ifstream::in | std::ifstream::binary);

	_header.Clear();
	_header.LoadFile(((const std::string&)path + ".header.xml").c_str());

	_buffer = new std::vector<char>();

	_lock = true;
}

void ok::XMLPakReader::Pop(tinyxml2::XMLDocument & out_document, ok::String & document_name)
{
	if (!_lock) return;

	auto nested_xml_record = _header.FirstChildElement("xmlpak")->FirstChildElement(document_name.toAnsiString().c_str());
	_in.seekg(nested_xml_record->Int64Attribute("position"));

	_buffer->resize(nested_xml_record->IntAttribute("length"));

	char* buf_ptr = &(*_buffer)[0];
	_in.read(buf_ptr, _buffer->size());

	out_document.Clear();
	out_document.Parse(buf_ptr, _buffer->size());
}

void ok::XMLPakReader::End()
{
	if (!_lock) return;

	_in.close();
	_header.Clear();

	delete _buffer;

	_lock = false;
}
