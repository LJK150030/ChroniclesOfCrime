#include "Engine/EngineCommon.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/Vec3.hpp"
#include "StringUtils.hpp"


void NamedStrings::PopulateFromXmlElementAttributes(const XmlElement& element)
{
	for (const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute; attribute = attribute->Next())
	{
		SetValue(attribute->Name(), attribute->Value());
	}
}

void NamedStrings::SetValue(const std::string& key_name, const std::string& new_value)
{
	m_keyValuePairs[key_name] = new_value;
}

bool NamedStrings::GetValue(const std::string& key_name, const bool default_value) const
{
	bool temp_bool(default_value);
	const std::map<std::string, std::string>::const_iterator string_iterator =
		m_keyValuePairs.find(key_name);

	if (string_iterator != m_keyValuePairs.end())
	{
		if (string_iterator->second == "True" || string_iterator->second == "true")
			temp_bool = true;
		else if (string_iterator->second == "False" || string_iterator->second == "false")
			temp_bool = false;
	}

	return temp_bool;
}

int NamedStrings::GetValue(const std::string& key_name, const int default_value) const
{
	int temp_int(default_value);
	const std::map<std::string, std::string>::const_iterator string_iterator =
		m_keyValuePairs.find(key_name);

	if (string_iterator != m_keyValuePairs.end())
		temp_int = stoi(string_iterator->second);

	return temp_int;
}

float NamedStrings::GetValue(const std::string& key_name, const float default_value) const
{
	float temp_float(default_value);
	const std::map<std::string, std::string>::const_iterator string_iterator =
		m_keyValuePairs.find(key_name);

	if (string_iterator != m_keyValuePairs.end())
		temp_float = stof(string_iterator->second);

	return temp_float;
}

std::string NamedStrings::GetValue(const std::string& key_name, const std::string& default_value) const
{
	std::string temp_string(default_value);
	const std::map<std::string, std::string>::const_iterator string_iterator =
		m_keyValuePairs.find(key_name);

	if (string_iterator != m_keyValuePairs.end())
		temp_string = string_iterator->second;

	return temp_string;
}

std::string NamedStrings::GetValue(const std::string& key_name, const char* default_value) const
{
	std::string temp_char(Stringf(default_value));
	const std::map<std::string, std::string>::const_iterator string_iterator =
		m_keyValuePairs.find(key_name);

	if (string_iterator != m_keyValuePairs.end())
		temp_char = string_iterator->second;

	return temp_char;
}

Rgba NamedStrings::GetValue(const std::string& key_name, const Rgba& default_value) const
{
	Rgba temp_rgba(default_value);
	const std::map<std::string, std::string>::const_iterator string_iterator =
		m_keyValuePairs.find(key_name);

	if (string_iterator != m_keyValuePairs.end())
		temp_rgba.SetFromText(string_iterator->second.c_str());

	return temp_rgba;
}

Vec2 NamedStrings::GetValue(const std::string& key_name, const Vec2& default_value) const
{
	Vec2 temp_vec2(default_value);
	const std::map<std::string, std::string>::const_iterator string_iterator =
		m_keyValuePairs.find(key_name);

	if (string_iterator != m_keyValuePairs.end())
		temp_vec2.SetFromText(string_iterator->second.c_str());

	return temp_vec2;
}

IntVec2 NamedStrings::GetValue(const std::string& key_name, const IntVec2& default_value) const
{
	IntVec2 temp_int_vec2(default_value);
	const std::map<std::string, std::string>::const_iterator string_iterator =
		m_keyValuePairs.find(key_name);

	if (string_iterator != m_keyValuePairs.end())
		temp_int_vec2.SetFromText(string_iterator->second.c_str());

	return temp_int_vec2;
}

FloatRange NamedStrings::GetValue(const std::string& key_name, const FloatRange& default_value) const
{
	FloatRange temp_float_range(default_value);
	const std::map<std::string, std::string>::const_iterator string_iterator =
		m_keyValuePairs.find(key_name);

	if (string_iterator != m_keyValuePairs.end())
		temp_float_range.SetFromText(string_iterator->second.c_str());

	return temp_float_range;
}

IntRange NamedStrings::GetValue(const std::string& key_name, const IntRange& default_value) const
{
	IntRange temp_int_range(default_value);
	const std::map<std::string, std::string>::const_iterator string_iterator =
		m_keyValuePairs.find(key_name);

	if (string_iterator != m_keyValuePairs.end())
		temp_int_range.SetFromText(string_iterator->second.c_str());

	return temp_int_range;
}

Vec3 NamedStrings::GetValue(const std::string& key_name, const Vec3& default_value)
{
	Vec3 temp_vec3(default_value);
	const std::map<std::string, std::string>::const_iterator string_iterator =
		m_keyValuePairs.find(key_name);

	if (string_iterator != m_keyValuePairs.end())
		temp_vec3.SetFromText(string_iterator->second.c_str());

	return temp_vec3;
}

std::string NamedStrings::GetMapContents()
{
	std::string concatenate_value = "";

	for (auto const& entity : m_keyValuePairs)
	{
		concatenate_value += entity.first + " -> " + entity.second + "\n";
	}

	return concatenate_value;
}

void NamedStrings::DebugPrintContents()
{
	for (std::map<std::string, std::string>::const_iterator key_iterator = m_keyValuePairs.begin();
	     key_iterator != m_keyValuePairs.end(); ++key_iterator)
	{
		g_theDevConsole->PrintString(DevConsole::CONSOLE_ECHO,
		                             Stringf("[%s] -> %s", key_iterator->first.c_str(), key_iterator->second.c_str()));
	}
}

bool NamedStrings::HasKey(const std::string& key_name)
{
	return m_keyValuePairs.count(key_name);
}
