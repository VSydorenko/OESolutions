////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : value enum  
////////////////////////////////////////////////////////////////////////////

#include "valueEnum.h"

wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumOrient, CValue);
wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumStretch, CValue);
wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumOrientNotebookPage, CValue);
wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumHorizontalAlignment, CValue);
wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumVerticalAlignment, CValue);
wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumBorder, CValue);
wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumTitleLocation, CValue);

//add new enumeration
ENUM_TYPE_REGISTER(CValueEnumOrient, "windowOrient", string_to_clsid("EN_WORI"));
ENUM_TYPE_REGISTER(CValueEnumStretch, "windowStretch", string_to_clsid("EN_STRH"));
ENUM_TYPE_REGISTER(CValueEnumOrientNotebookPage, "windowOrientNotebookPage", string_to_clsid("EN_WNKP"));
ENUM_TYPE_REGISTER(CValueEnumHorizontalAlignment, "windowHorizontalAlignment", string_to_clsid("EN_WHGT"));
ENUM_TYPE_REGISTER(CValueEnumVerticalAlignment, "windowVerticalAlignment", string_to_clsid("EN_WAGT"));
ENUM_TYPE_REGISTER(CValueEnumBorder, "windowBorder", string_to_clsid("EN_WBRD"));
ENUM_TYPE_REGISTER(CValueEnumTitleLocation, "titleLocation", string_to_clsid("EN_TILC"));