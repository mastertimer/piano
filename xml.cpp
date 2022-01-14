#include "xml.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_xml_element::~_xml_element()
{
	if (!parent) delete[] data;
}

_xml_element::_xml_element(std::wstring_view file_name)
{
	load_file(file_name, &data, &size, 1);
	parse();
}

i64 _xml_element::find_symbol(i64 n, char c, i64 k)
{
	if (n < 0) return -1;
	if (k < 0 || k > size) k = size;
	for (i64 i = n; i < k; i++) if (data[i] == c) return i + 1;
	return -1;
}

void _xml_element::parse()
{
	std::string name;
	std::vector<_xml_parameter> parameters;
	i64 n = 0;
	for (;;)
	{
		n = find_symbol(n, '<');
		if (n < 0) break;
		i64 k = find_symbol(n, '>');
		if (k < 0) break;
		if (data[n] == '!' || data[n] == '?')
		{
			n = k;
			continue;
		}
		parse_parameters(n, k, name, parameters);
		if (data[k - 2] == '/')
		{
			child.emplace_back(this, name, parameters, nullptr, 0);
			n = k;
		}
		else
		{
			std::string s = "</" + name + ">";
			char ww = data[size];
			data[size] = 0;
			auto res = strstr(&data[k], s.c_str());
			data[size] = ww;
			if (!res) break;
			child.emplace_back(this, name, parameters, &data[k], res - &data[k]);
			n = res - data + s.size();
		}
	}
}

void _xml_element::parse_parameters(i64 n, i64 k, std::string& name, std::vector<_xml_parameter>& parameters)
{
	parameters.clear();
	k--;
	i64 sp = find_symbol(n, ' ', k);
	i64 kn = (sp < 0) ? k : sp;
	auto ww = data[kn];
	data[kn] = 0;
	name = &data[n];
	data[kn] = ww;
	while (sp >= 0)
	{
		i64 ravno = find_symbol(sp, '=', k);
		if (ravno < 0) break;
		if (data[ravno] != '"') break;
		i64 dk2 = find_symbol(ravno + 1, '"', k);
		auto ww1 = data[ravno - 1];
		data[ravno - 1] = 0;
		auto ww2 = data[dk2 - 1];
		data[dk2 - 1] = 0;
		parameters.emplace_back(&data[sp], &data[ravno + 1]);
		data[ravno - 1] = ww1;
		data[dk2 - 1] = ww2;
	}
}

_xml_element::_xml_element(_xml_element* parent_, std::string& name, std::vector<_xml_parameter>& param_, char* data_,
	i64 size_)
	: data{data_}
	, size{size_}
	, parent{parent_}
	, param{std::move(param_)}
{
	parse();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
