#include "metaObjectMetadataEnum.h"

wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumVersion, CValue);

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

//add new enumeration
ENUM_TYPE_REGISTER(CValueEnumVersion, "programVersion", string_to_clsid("EN_VRSN"));