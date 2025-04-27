#include "propertyEnum.h"

//load & save object in control 
bool IPropertyEnum::LoadData(CMemoryReader& reader) 
{
	IPropertyEnum::SetValue(reader.r_s32());
	return true;
}

bool IPropertyEnum::SaveData(CMemoryWriter& writer) 
{
	writer.w_s32(IPropertyEnum::GetValueAsInteger());
	return true;
}