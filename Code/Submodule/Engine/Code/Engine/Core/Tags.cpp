#include "Engine/Core/Tags.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"

void Tags::SetOrRemoveTags(const std::string& comma_separated_tag_names)
{
	if (comma_separated_tag_names.empty())
		return;

	Strings tags = SplitStringOnDelimiter(comma_separated_tag_names, ',');

	for (int tag_id = 0; tag_id < static_cast<int>(tags.size()); ++tag_id)
	{
		if (tags[tag_id][0] == '!')
		{
			RemoveTag(tags[tag_id].substr(1));
		}
		else
			SetTag(tags[tag_id]);
	}
}

bool Tags::HasTags(const std::string& comma_separated_tag_names)
{
	Strings tags_to_find = SplitStringOnDelimiter(comma_separated_tag_names, ',');

	for (int search_id = 0; search_id < static_cast<int>(tags_to_find.size()); ++search_id)
	{
		if (tags_to_find[search_id][0] == '!')
		{
			if (HasTag(tags_to_find[search_id].substr(1)))
				return false;
		}
		else
		{
			if (!HasTag(tags_to_find[search_id]))
				return false;
		}
	}

	return true;
}

void Tags::SetTag(const std::string& tag_name)
{
	Strings tags = SplitStringOnDelimiter(tag_name, ' ');
	if (tag_name[0] == '!' || tags.size() > 1)
	{
		g_theDevConsole->PrintString(DevConsole::CONSOLE_ERROR,
		                             Stringf("ERROR: %s, is not a proper tag. Only one word and no ! tags allowed",
		                                     tag_name.c_str()));
		return;
	}

	if (m_tags.empty())
	{
		m_tags.push_back(tag_name);
	}
	else
	{
		const Strings::const_iterator tag_to_find = std::find(m_tags.begin(), m_tags.end(), tag_name);

		if (tag_to_find == m_tags.end())
			m_tags.push_back(tag_name);
	}
}

void Tags::RemoveTag(const std::string& tag_name)
{
	Strings tags = SplitStringOnDelimiter(tag_name, ' ');
	if (tag_name[0] == '!' || tags.size() > 1)
	{
		g_theDevConsole->PrintString(DevConsole::CONSOLE_ERROR,
		                             Stringf("ERROR: %s, is not a proper tag. Only one word and no ! tags allowed",
		                                     tag_name.c_str()));
		return;
	}

	if (m_tags.empty())
		return;

	const Strings::const_iterator tag_to_find = std::find(m_tags.begin(), m_tags.end(), tag_name);

	if (tag_to_find != m_tags.end())
		m_tags.erase(tag_to_find);
}

bool Tags::HasTag(const std::string& tag_name)
{
	Strings tags = SplitStringOnDelimiter(tag_name, ' ');
	if (tag_name[0] == '!' || tags.size() > 1)
	{
		g_theDevConsole->PrintString(DevConsole::CONSOLE_ERROR,
		                             Stringf("ERROR: %s, is not a proper tag. Only one word and no ! tags allowed",
		                                     tag_name.c_str()));
		return false;
	}

	const Strings::const_iterator tag_to_find = std::find(m_tags.begin(), m_tags.end(), tag_name);

	return tag_to_find != m_tags.end();
}
