////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : meta-attribues
////////////////////////////////////////////////////////////////////////////

#include "metaTableObject.h"
#include "backend/metaData.h"

#include <wx/propgrid/manager.h>

void CMetaObjectTableData::OnPropertyRefresh(wxPropertyGridManager* pg, wxPGProperty* pgProperty, IProperty* property)
{
	if (m_propertyUse == property) {
		IMetaObjectRecordDataFolderMutableRef* metaObject = dynamic_cast<IMetaObjectRecordDataFolderMutableRef*>(m_parent);
		pg->HideProperty(pgProperty, metaObject == nullptr);
	}
}