#pragma once
#include <string>
#include <map>
#include "Engine/Core/XmlUtils.hpp"

class NamedStrings
{
public:
	void PopulateFromXmlElementAttributes(const XmlElement& element);
	void SetValue(const std::string& key_name, const std::string& new_value);
	bool GetValue(const std::string& key_name, bool default_value) const;
	int GetValue(const std::string& key_name, int default_value) const;
	float GetValue(const std::string& key_name, float default_value) const;
	std::string GetValue(const std::string& key_name, const std::string& default_value) const;
	std::string GetValue(const std::string& key_name, const char* default_value) const;
	Rgba GetValue(const std::string& key_name, const Rgba& default_value) const;
	Vec2 GetValue(const std::string& key_name, const Vec2& default_value) const;
	IntVec2 GetValue(const std::string& key_name, const IntVec2& default_value) const;
	FloatRange GetValue(const std::string& key_name, const FloatRange& default_value) const;
	IntRange GetValue(const std::string& key_name, const IntRange& default_value) const;
	Vec3 GetValue(const std::string& key_name, const Vec3& default_value);
	std::string GetMapContents();
	void DebugPrintContents();
	bool HasKey(const std::string& key_name);


private:
	std::map<std::string, std::string> m_keyValuePairs;
};
