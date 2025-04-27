#include "metaAttributeObjectEnum.h"

wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumItemMode, CValue);
wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumSelectMode, CValue);

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

//add new enumeration
ENUM_TYPE_REGISTER(CValueEnumItemMode, "itemMode", string_to_clsid("EN_ITMO"));
ENUM_TYPE_REGISTER(CValueEnumSelectMode, "selectMode", string_to_clsid("EN_SEMO"));