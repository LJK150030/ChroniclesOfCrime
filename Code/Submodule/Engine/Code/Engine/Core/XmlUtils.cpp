#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

int ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const int default_value)
{
	int temp_int(default_value);
	const char* value = element.Attribute(attribute_name);

	if (value != nullptr)
		temp_int = atoi(value);

	return temp_int;
}

char ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const char default_value)
{
	char temp_char(default_value);
	const char* value = element.Attribute(attribute_name);

	if (value != nullptr)
		temp_char = value[0];

	return temp_char;
}

bool ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const bool default_value)
{
	bool temp_bool(default_value);
	const char* value = element.Attribute(attribute_name);

	if (value != nullptr)
	{
		if (strncmp(value, "True", 4))
			temp_bool = true;
		else if (strncmp(value, "False", 5))
			temp_bool = false;
	}

	return temp_bool;
}

float ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const float default_value)
{
	float temp_float(default_value);
	const char* value = element.Attribute(attribute_name);

	if (value != nullptr)
		temp_float = static_cast<float>(atof(value));

	return temp_float;
}

Rgba ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const Rgba& default_value)
{
	Rgba temp_rgba(default_value);
	const char* value = element.Attribute(attribute_name);

	if (value != nullptr)
		temp_rgba.SetFromText(value);

	return temp_rgba;
}

Vec2 ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const Vec2& default_value)
{
	Vec2 temp_vec2(default_value);
	const char* value = element.Attribute(attribute_name);

	if (value != nullptr)
		temp_vec2.SetFromText(value);

	return temp_vec2;
}

IntRange ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const IntRange& default_value)
{
	IntRange temp_int_range(default_value);
	const char* value = element.Attribute(attribute_name);

	if (value != nullptr)
		temp_int_range.SetFromText(value);

	return temp_int_range;
}

FloatRange ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const FloatRange& default_value)
{
	FloatRange temp_float_range(default_value);
	const char* value = element.Attribute(attribute_name);

	if (value != nullptr)
		temp_float_range.SetFromText(value);

	return temp_float_range;
}

IntVec2 ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const IntVec2& default_value)
{
	IntVec2 temp_int_vec2(default_value);
	const char* value = element.Attribute(attribute_name);

	if (value != nullptr)
		temp_int_vec2.SetFromText(value);

	return temp_int_vec2;
}

std::string ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const std::string& default_value)
{
	std::string temp_string(default_value);
	const char* value = element.Attribute(attribute_name);

	if (value != nullptr)
		temp_string = Stringf(value);

	return temp_string;
}

std::string ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const char* default_value)
{
	std::string temp_char(default_value);
	const char* value = element.Attribute(attribute_name);

	if (value != nullptr)
		temp_char = Stringf(value);

	return temp_char;
}
