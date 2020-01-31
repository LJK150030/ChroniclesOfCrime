#pragma once
#include <string>


class BbDatum
{
public:
	BbDatum();
	virtual ~BbDatum() = default;
	
	virtual const std::string& GetDataType() = 0;
	virtual bool GetAsBool() const = 0;
};

template <class T>
class BbDatumT : public BbDatum
{
};

class Blackboard
{
	BbDatum entries[10];
	//passedActions: Action[] if I want to use an arbiter
};


