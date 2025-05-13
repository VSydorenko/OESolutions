////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : metagrid object
////////////////////////////////////////////////////////////////////////////

#include "metaGridObject.h"
#include "backend/metaData.h"

wxIMPLEMENT_ABSTRACT_CLASS(IMetaObjectGrid, IMetaObject);

wxIMPLEMENT_DYNAMIC_CLASS(CMetaObjectGrid, IMetaObjectGrid);
wxIMPLEMENT_DYNAMIC_CLASS(CMetaObjectCommonGrid, IMetaObjectGrid);

//***********************************************************************
//*                           Metagrid                                  *
//***********************************************************************

bool IMetaObjectGrid::LoadData(CMemoryReader &reader)
{
	return true;
}

bool IMetaObjectGrid::SaveData(CMemoryWriter &writer)
{
	return true;
}

//***********************************************************************
//*                           read & save events                        *
//***********************************************************************

bool IMetaObjectGrid::OnBeforeRunMetaObject(int flags)
{
	return IMetaObject::OnBeforeRunMetaObject(flags);
}

bool IMetaObjectGrid::OnAfterCloseMetaObject()
{
	return IMetaObject::OnAfterCloseMetaObject();
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

METADATA_TYPE_REGISTER(CMetaObjectGrid, "template", g_metaTemplateCLSID);
METADATA_TYPE_REGISTER(CMetaObjectCommonGrid, "commonTemplate", g_metaCommonTemplateCLSID);
