#pragma once
#include <utility>
#include "Engine/EngineCommon.hpp"

class NamedProperties;

//------------------------------------------------------------------------
// Base type - what would this have to implement to support the above interfaces?
//------------------------------------------------------------------------
class BaseProperty 
{
	friend NamedProperties;

protected:
	virtual ~BaseProperty() = default;

public:
	virtual std::string GetAsString() const = 0; 
};



template<typename T>
std::string ToString(const T val)
{
	return val.GetAsString();
}

template<typename T>
std::string ToString(T *const val)
{
	UNUSED(val);
	return "";
}



// template<typename T>
// std::string ToString(const T* prt_val)
// {
// 	UNUSED(prt_val);
// 	return "";
// }
// 
// template<>
// inline std::string ToString(const void* val)
// {
// 	UNUSED(val);
// 	return "";
// }

//------------------------------------------------------------------------
// The storage
//------------------------------------------------------------------------
// There are a few ways we can get the "type" checking in
// One is to use RTTI (Run Time Type Information) which is a compile option.  This allows for dynamic_cast to work;
// Second (which I"ll show) will use what we know about static memory to create a unique identifier for types; 
template <typename T> 
class TypedProperty : public BaseProperty
{
public:
	explicit TypedProperty(T const& val ): m_value(std::move(val))	{}
	~TypedProperty();

	// using duck typing - this will fail if we don't implement a "GetAsString" for a type stored here
	std::string GetAsString() const override { return ::ToString(m_value); } 
public:
	T m_value; // actual value being stored here; 

};

template <typename T>
TypedProperty<T>::~TypedProperty() = default;

inline std::string TypedProperty<std::string>::GetAsString() const
{
	return m_value;
}

inline std::string TypedProperty<int>::GetAsString() const
{
	return std::to_string(m_value);
}

//FromString, converting a string to the type we need
template <typename T> 
T FromString(const std::string& str, const T& def )
{
	UNUSED(def);
	return T(str); 
}

template <> 
inline float FromString<float>(const std::string& str, const float& def )
{
	float val;

	try
	{
		val = std::stof(str);
	}
	catch(const std::invalid_argument& e)
	{
		UNUSED(e);
		return def;
	}
	catch(const std::out_of_range& e)
	{
		UNUSED(e);
		return def;
	}

	return val;
}

template <> 
inline std::string FromString<std::string>( const std::string& str, const std::string& def )
{
	UNUSED(def);
	return str;
}

//------------------------------------------------------------------------
// Our Collection
//------------------------------------------------------------------------
class NamedProperties
{
public:
	NamedProperties();
	~NamedProperties(); // be sure to now delete all the properties!  

	std::string GetPropertyString( std::string const &name, std::string const &def = "" ); 
	void DebugPrintContents();


public:
	// Template interface we want;
	template <typename T> 
	void SetValue( const std::string& id, T const& value )
	{
		auto*prop = new TypedProperty<T>(value);

		const auto template_iterator = m_properties.find( id ); 
		if (template_iterator != m_properties.end()) {
			m_properties.erase(template_iterator);
		}

		m_properties[id] = prop; 
	};

	template <typename T> 
	T GetValue( const std::string& id, T const& default_value ) 
	{
		const auto template_iterator = m_properties.find( id ); 
		if (template_iterator == m_properties.end()) {
			return default_value; 
		}

		BaseProperty* prop = template_iterator->second;

		TypedProperty<T>* typed_prop = dynamic_cast<TypedProperty<T>*>( prop );

		//if typed_prop is null, then try to get it from a string
		if(typed_prop == nullptr)
		{
			std::string temp_string = prop->GetAsString();
			T typed_value = FromString(temp_string, default_value);
			return typed_value;
			//return default_value;
		}

		return typed_prop->m_value; 
	};

	template <typename T> 
	void SetValue( const std::string& key, T* ptr )
	{
		//if I get an error later, make sure to use a refrence instead of pointer for T
		SetValue<T*>( key, ptr ); 
	}

	template <typename T> 
	T GetValue( const std::string& key, T& def )
	{
		const auto template_iterator = m_properties.find( key ); 
		if (template_iterator == m_properties.end()) {
			return def; 
		}

		BaseProperty *prop = template_iterator->second; 
		TypedProperty<T>* typed_prop = dynamic_cast<TypedProperty<T>*>( prop ); 
		if (typed_prop == nullptr) {
			return def;
		} 		
		return typed_prop->m_value; 
	}

	template <>
	void SetValue(const std::string& key, char const* ptr)
	{
		SetValue(key, std::string(ptr));
	}


	std::string GetValue( const std::string&key, char const* def )
	{
		return GetValue( key, std::string(def) ); 
	};


private: 
//	// supporting methods; 
// 	BaseProperty* GetProperty( std::string const &name ) const; 
// 	void SetProperty( std::string const &name, BaseProperty const *prop ); 

public:
	std::map<std::string, BaseProperty*> m_properties; 
}; 

