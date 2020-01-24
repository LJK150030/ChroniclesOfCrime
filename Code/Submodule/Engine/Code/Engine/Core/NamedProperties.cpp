#include "Engine/Core/NamedProperties.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"

NamedProperties::NamedProperties()
{
}

NamedProperties::~NamedProperties()
{
	for(auto itr = m_properties.begin(); itr != m_properties.end(); )
	{
		// found it - delete it
		delete (itr->second);
		itr = m_properties.erase(itr);
	}
}

std::string NamedProperties::GetPropertyString(std::string const& name, std::string const& def)
{
	auto template_iterator = m_properties.find( name ); 
	if (template_iterator == m_properties.end()) {
		return def; 
	}
	
	return template_iterator->second->GetAsString();
}

void NamedProperties::DebugPrintContents()
{
	for (auto key_iterator = m_properties.begin();
		key_iterator != m_properties.end(); ++key_iterator)
	{
		g_theDevConsole->PrintString(DevConsole::CONSOLE_ECHO,
			Stringf("[%s] -> %s", key_iterator->first.c_str(), key_iterator->second->GetAsString().c_str()));
	}
}


