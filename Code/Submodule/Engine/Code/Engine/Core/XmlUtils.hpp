#pragma once
#include "ThirdParty/TinyXML2/tinyxml2.h"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntVec2.hpp"

typedef tinyxml2::XMLElement XmlElement;

int ParseXmlAttribute(const XmlElement& element, const char* attribute_name, int default_value);
char ParseXmlAttribute(const XmlElement& element, const char* attribute_name, char default_value);
bool ParseXmlAttribute(const XmlElement& element, const char* attribute_name, bool default_value);
float ParseXmlAttribute(const XmlElement& element, const char* attribute_name, float default_value);
Rgba ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const Rgba& default_value);
Vec2 ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const Vec2& default_value);
IntRange ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const IntRange& default_value);
FloatRange ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const FloatRange& default_value);
IntVec2 ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const IntVec2& default_value);
std::string ParseXmlAttribute(const XmlElement& element, const char* attribute_name, const std::string& default_value);
std::string ParseXmlAttribute(const XmlElement& element, const char* attribute_name,
                              const char* default_value = nullptr);
