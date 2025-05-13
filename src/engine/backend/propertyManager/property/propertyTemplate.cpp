#include "propertyTemplate.h"

////////////////////////////////////////////////////////////////////////

wxVariantData* CPropertyTempalate::CreateVariantData()
{
	return nullptr;
}

////////////////////////////////////////////////////////////////////////

//base property for "template"
bool CPropertyTempalate::SetDataValue(const CValue& varPropVal)
{
	return false;
}

bool CPropertyTempalate::GetDataValue(CValue& pvarPropVal) const
{
	pvarPropVal = GetName();
	return true;
}

bool CPropertyTempalate::LoadData(CMemoryReader& reader)
{
	return true;
}

bool CPropertyTempalate::SaveData(CMemoryWriter& writer)
{
	return true;
}