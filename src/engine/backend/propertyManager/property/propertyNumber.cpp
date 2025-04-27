#include "propertyNumber.h"

//base property for "number"
bool IPropertyNumber::SetDataValue(const CValue& varPropVal)
{
	SetValue(varPropVal.GetNumber());
	return true;
}

bool IPropertyNumber::GetDataValue(CValue& pvarPropVal) const
{
	pvarPropVal = IPropertyNumber::GetValueAsNumber();
	return true;
}

bool IPropertyNumber::LoadData(CMemoryReader& reader)
{
	number_t value;
	reader.r(&value, sizeof(number_t));
	SetValue(value);
	return true;
}

bool IPropertyNumber::SaveData(CMemoryWriter& writer)
{
	writer.w(&GetValueAsNumber(), sizeof(number_t));
	return true;
}