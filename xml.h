#pragma once

#include "basic.h"

struct _xml_element
{
	std::string name;
	std::vector<_xml_element> child;

	struct _xml_parameter
	{
		std::string name;
		std::string value;
	};

	_xml_element() = default;
	_xml_element(_xml_element* parent_, std::string& name_, std::vector<_xml_parameter>& param_, char* data_, i64 size_);
	~_xml_element();

	bool load_from_file(std::wstring_view file_name);
	const _xml_element& find(std::string_view name_) const;

private:
	char* data = nullptr;
	i64 size = 0;
	_xml_element* parent = nullptr;
	std::vector<_xml_parameter> param;

	void parse();
	i64 find_symbol(i64 n, char c, i64 k = -1); // возвращает следующую позицию, -1 если не найдено
	i64 find_not_symbol(i64 n, char c, i64 k = -1); // возвращает позицию несимвола, -1 если не найдено
	void parse_parameters(i64 n, i64 k, std::string& name, std::vector<_xml_parameter>& parameters);
};
