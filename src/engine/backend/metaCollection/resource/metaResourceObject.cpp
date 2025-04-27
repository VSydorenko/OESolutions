#include "metaResourceObject.h"
#include "backend/metadata.h"

wxIMPLEMENT_DYNAMIC_CLASS(CMetaObjectResource, CMetaObjectAttribute);

eSelectorDataType CMetaObjectResource::GetFilterDataType() const
{
	IMetaObjectGenericData* metaObject = dynamic_cast<IMetaObjectGenericData*>(m_parent);
	if (metaObject->GetClassType() == g_metaInformationRegisterCLSID) 
		return metaObject->GetFilterDataType();
	return eSelectorDataType::eSelectorDataType_resource;
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

METADATA_TYPE_REGISTER(CMetaObjectResource, "resource", g_metaResourceCLSID);