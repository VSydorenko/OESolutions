////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : register metaData - menu
////////////////////////////////////////////////////////////////////////////

#include "accumulationRegister.h"
#include "backend/metadataConfiguration.h"

bool CMetaObjectAccumulationRegister::PrepareContextMenu(wxMenu* defaultMenu)
{
	wxMenuItem* menuItem = nullptr;
	menuItem = defaultMenu->Append(ID_METATREE_OPEN_MODULE, _("Open record set"));
	menuItem->SetBitmap(m_propertyModuleObject->GetMetaObject()->GetIcon());
	menuItem = defaultMenu->Append(ID_METATREE_OPEN_MANAGER, _("Open manager"));
	menuItem->SetBitmap(m_propertyModuleManager->GetMetaObject()->GetIcon());
	defaultMenu->AppendSeparator();
	return false;
}

void CMetaObjectAccumulationRegister::ProcessCommand(unsigned int id)
{
	IBackendMetadataTree* metaTree = m_metaData->GetMetaTree();
	wxASSERT(metaTree);

	if (id == ID_METATREE_OPEN_MODULE)
		metaTree->OpenFormMDI(m_propertyModuleObject->GetMetaObject());
	else if (id == ID_METATREE_OPEN_MANAGER)
		metaTree->OpenFormMDI(m_propertyModuleManager->GetMetaObject());
}