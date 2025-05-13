////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : metagrid menu
////////////////////////////////////////////////////////////////////////////

#include "metaGridObject.h"
#include "backend/metaData.h"

bool IMetaObjectGrid::PrepareContextMenu(wxMenu *defaultMenu)
{
	wxMenuItem *menuItem = defaultMenu->Append(ID_METATREE_OPEN_TEMPLATE, _("Open template"));
	menuItem->SetBitmap(GetIcon());
	defaultMenu->AppendSeparator();
	return false;
}

void IMetaObjectGrid::ProcessCommand(unsigned int id)
{
	IBackendMetadataTree *metaTree = m_metaData->GetMetaTree();
	wxASSERT(metaTree);

	if (id == ID_METATREE_OPEN_TEMPLATE)
		metaTree->OpenFormMDI(this);
}