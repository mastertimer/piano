#include "xml.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace
{
	const _xml_element zero_xml_element;
}

const _xml_element& _xml_element::find(std::string_view name_) const
{
	for (auto& i : child) if (i.name == name_) return i;
	return zero_xml_element;
}

_xml_element::~_xml_element()
{
	if (!parent) delete[] data;
}

bool _xml_element::load_from_file(std::wstring_view file_name)
{
	if (parent || data) return false;
	if (!load_file(file_name, &data, &size, 1)) return false;
	parse();
	return true;
}

i64 _xml_element::find_symbol(i64 n, char c, i64 k)
{
	if (n < 0) return -1;
	if (k < 0 || k > size) k = size;
	for (i64 i = n; i < k; i++) if (data[i] == c) return i + 1;
	return -1;
}

i64 _xml_element::find_not_symbol(i64 n, char c, i64 k)
{
	if (n < 0) return -1;
	if (k < 0 || k > size) k = size;
	for (i64 i = n; i < k; i++) if (data[i] != c) return i;
	return -1;
}

void _xml_element::parse()
{
	std::string name2;
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
		parse_parameters(n, k, name2, parameters);
		if (data[k - 2] == '/')
		{
			child.emplace_back(this, name2, parameters, nullptr, 0);
			n = k;
		}
		else
		{
			std::string s = "</" + name2 + ">";
			char ww = data[size];
			data[size] = 0;
			auto res = strstr(&data[k], s.c_str());
			data[size] = ww;
			if (!res) break;
			child.emplace_back(this, name2, parameters, &data[k], res - &data[k]);
			n = res - data + s.size();
		}
	}
}

void _xml_element::parse_parameters(i64 n, i64 k, std::string& name_, std::vector<_xml_parameter>& parameters)
{
	parameters.clear();
	k--;
	i64 sp = find_symbol(n, ' ', k);
	i64 kn = (sp < 0) ? k : (sp - 1);
	auto ww = data[kn];
	data[kn] = 0;
	name_ = &data[n];
	data[kn] = ww;
	sp = find_not_symbol(sp, ' ', k);
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
		sp = find_not_symbol(dk2, ' ', k);
	}
}

_xml_element::_xml_element(_xml_element* parent_, std::string& name_, std::vector<_xml_parameter>& param_, char* data_,
	i64 size_)
	: data{data_}
	, size{size_}
	, parent{parent_}
	, name{name_}
	, param{std::move(param_)}
{
	parse();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
