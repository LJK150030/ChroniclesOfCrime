#pragma once
#include <string>
#include "Engine/Core/ErrorWarningAssert.hpp"

// structure inspired by Kevin Dill's Generic Data containment
// (https://www.youtube.com/watch?v=iVBCBcEANBc&t=805s)

// Interface the the Datum in the blackboard
class BbDatum
{
public:
	BbDatum() = default;
	virtual ~BbDatum() = default;
	
	virtual const std::string& GetDataType() const = 0;
};

//------------------------------------------------------------------


// Abstract using the Interface
template <class T>
class BbDatumT : public BbDatum
{
public:
	virtual const T& GetDataValue() const = 0;
	
	virtual bool CanSet() const;	
	virtual void SetDataValue(const T& val);
};

template <class T>
bool BbDatumT<T>::CanSet() const
{
	return false;
}

template <class T>
void BbDatumT<T>::SetDataValue(const T& val)
{
	UNUSED(val);
	ASSERT_OR_DIE(false, "Cannont assign to this Data");
}


//------------------------------------------------------------------


template <class T>
class BbDatumTsimple : public BbDatumT<T>
{
public:
	//const std::string& GetDataType() const override; dont know what the type is yet
	const T& GetDataValue() const override;
	bool CanSet() const override;
	void SetDataValue(const T& val) override;
	
protected:
	T m_val;
};

template <class T>
const T& BbDatumTsimple<T>::GetDataValue() const
{
	return m_val;
}

template <class T>
bool BbDatumTsimple<T>::CanSet() const
{
	return true;
}

template <class T>
void BbDatumTsimple<T>::SetDataValue(const T& val)
{
	m_val = val;
}


//------------------------------------------------------------------


class BbDatum_Int : public BbDatumTsimple<int>
{
public:
	virtual const std::string& GetDataType() const
	{
		static const std::string TYPE_NAME("int");
		return TYPE_NAME;
	}
};


//------------------------------------------------------------------


class Blackboard
{
public:
	Blackboard();
	~Blackboard();
	
	void Add(const std::string& semantic_name, int num);
	void Remove(const std::string& semantic_name);
	
private:
	std::pair<std::string, BbDatum*> m_testElement;
	//passedActions: Action[] if I want to use an arbiter
};