#pragma once
#include "Engine/EngineCommon.hpp"
#include <vector>

typedef std::vector<std::string> Strings;

class Tags
{
public:
	// These support comma-separated strings of 0+ tag names, with “!” (NOT) tag names to remove
	void SetOrRemoveTags(const std::string& comma_separated_tag_names); // "blessed,!poisoned"
	bool HasTags(const std::string& comma_separated_tag_names); // "good,!cursed"

	// These each take exactly one tag name, and do not support commas nor “!” (NOT) tags
	void SetTag(const std::string& tag_name);
	void RemoveTag(const std::string& tag_name);
	bool HasTag(const std::string& tag_name);

protected:
	Strings m_tags;
};
