////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : dataprocessor window
////////////////////////////////////////////////////////////////////////////

#include "dataReportWnd.h"
#include "frontend/mainFrame/mainFrame.h"
#include "frontend/docView/docManager.h"
#include "backend/appData.h"

#define	objectFormsName _("forms")
#define	objectModulesName _("modules")
#define	objectTemplatesName _("templates")
#define objectAttributesName _("attributes")
#define objectTablesName _("tables")
#define objectEnumerationsName _("enums")

//***********************************************************************
//*                         metaData                                    * 
//***********************************************************************

void CDataReportTree::ActivateItem(const wxTreeItemId& item)
{
	IMetaObject* m_currObject = GetMetaObject(item);

	if (!m_currObject)
		return;

	OpenFormMDI(m_currObject);
}

IMetaObject* CDataReportTree::NewItem(const class_identifier_t& clsid, IMetaObject* metaParent, bool rubObject)
{
	return m_metaData->CreateMetaObject(clsid, metaParent, rubObject);
}

IMetaObject* CDataReportTree::CreateItem(bool showValue)
{
	const wxTreeItemId& item = GetSelectionIdentifier();
	if (!item.IsOk()) return nullptr;

	IMetaObject* createdObject = NewItem(
		GetClassIdentifier(),
		GetMetaIdentifier()
	);

	if (createdObject == nullptr) return nullptr;

	if (showValue) { OpenFormMDI(createdObject); }
	UpdateToolbar(createdObject, FillItem(createdObject, item));
	for (auto doc : docManager->GetDocumentsVector()) { doc->UpdateAllViews(); }

	objectInspector->SelectObject(createdObject, m_metaTreeWnd->GetEventHandler());
	return createdObject;
}

wxTreeItemId CDataReportTree::FillItem(IMetaObject* metaItem, const wxTreeItemId& item)
{
	m_metaTreeWnd->Freeze();

	wxTreeItemId createdItem = nullptr;
	if (metaItem->GetClassType() == g_metaTableCLSID) {
		createdItem = AppendGroupItem(item, g_metaAttributeCLSID, metaItem);
	}
	else {
		createdItem = AppendItem(item, metaItem);
	}

	//Advanced mode
	if (metaItem->GetClassType() == g_metaTableCLSID) {

		CMetaObjectTableData* metaItemRecord = dynamic_cast<CMetaObjectTableData*>(metaItem);
		wxASSERT(metaItemRecord);

		for (auto attribute : metaItemRecord->GetObjectAttributes()) {
			if (attribute->IsDeleted())
				continue;
			if (attribute->GetClassType() == g_metaDefaultAttributeCLSID)
				continue;
			AppendItem(createdItem, attribute);
		}
	}

	m_metaTreeWnd->InvalidateBestSize();
	m_metaTreeWnd->SelectItem(createdItem);
	m_metaTreeWnd->Expand(createdItem);

	m_metaTreeWnd->Thaw();

	return createdItem;
}

void CDataReportTree::EditItem()
{
	wxTreeItemId selection = m_metaTreeWnd->GetSelection();

	if (!selection.IsOk())
		return;

	IMetaObject* m_currObject = GetMetaObject(selection);

	if (!m_currObject)
		return;

	OpenFormMDI(m_currObject);
}

void CDataReportTree::RemoveItem()
{
	wxTreeItemId selection = m_metaTreeWnd->GetSelection();

	if (!selection.IsOk())
		return;

	wxTreeItemIdValue m_cookie;
	wxTreeItemId hItem = m_metaTreeWnd->GetFirstChild(selection, m_cookie);

	while (hItem)
	{
		EraseItem(hItem);
		hItem = m_metaTreeWnd->GetNextChild(hItem, m_cookie);
	}

	IMetaObject* metaObject = GetMetaObject(selection);
	wxASSERT(metaObject);
	EraseItem(selection);
	m_metaData->RemoveMetaObject(metaObject);

	//Delete item from tree
	m_metaTreeWnd->Delete(selection);

	const wxTreeItemId nextSelection = m_metaTreeWnd->GetFocusedItem();

	if (nextSelection.IsOk()) {
		UpdateToolbar(GetMetaObject(nextSelection), nextSelection);
	}

	//update choice if need
	UpdateChoiceSelection();
}

void CDataReportTree::EraseItem(const wxTreeItemId& item)
{
	IMetaObject* const metaObject = GetMetaObject(item);
	for (auto& doc : docManager->GetDocumentsVector()) {
		CMetaDocument* metaDoc = wxDynamicCast(doc, CMetaDocument);
		if (metaDoc != nullptr && metaObject == metaDoc->GetMetaObject()) {
			metaDoc->DeleteAllViews();
		}
	}
}

void CDataReportTree::SelectItem()
{
	if (appData->GetAppMode() != eRunMode::eDESIGNER_MODE) return;
	const wxTreeItemId& selection = m_metaTreeWnd->GetSelection();
	IMetaObject* metaObject = GetMetaObject(selection);
	objectInspector->ClearProperty();
	UpdateToolbar(metaObject, selection);
	if (objectInspector->IsShownProperty()) {
		if (metaObject == nullptr) return;
		objectInspector->CallAfter(&CObjectInspector::SelectObject, metaObject, m_metaTreeWnd->GetEventHandler());
	}
}

void CDataReportTree::PropertyItem()
{
	if (appData->GetAppMode() != eRunMode::eDESIGNER_MODE) return;
	const wxTreeItemId& selection = m_metaTreeWnd->GetSelection();
	IMetaObject* metaObject = GetMetaObject(selection);
	objectInspector->ClearProperty();
	UpdateToolbar(metaObject, selection);
	if (metaObject == nullptr) return;
	if (!objectInspector->IsShownProperty()) objectInspector->ShowProperty();
	objectInspector->CallAfter(&CObjectInspector::SelectObject, metaObject, m_metaTreeWnd->GetEventHandler());
}

void CDataReportTree::Collapse()
{
	const wxTreeItemId& selection = m_metaTreeWnd->GetSelection();
	CTreeData* data =
		dynamic_cast<CTreeData*>(m_metaTreeWnd->GetItemData(selection));
	if (data != nullptr)
		data->m_expanded = false;
}

void CDataReportTree::Expand()
{
	const wxTreeItemId& selection = m_metaTreeWnd->GetSelection();
	CTreeData* data =
		dynamic_cast<CTreeData*>(m_metaTreeWnd->GetItemData(selection));
	if (data != nullptr)
		data->m_expanded = true;
}

void CDataReportTree::UpItem()
{
	if (appData->GetAppMode() != eRunMode::eDESIGNER_MODE)
		return;

	m_metaTreeWnd->Freeze();
	const wxTreeItemId& selection = m_metaTreeWnd->GetSelection();
	const wxTreeItemId& nextItem = m_metaTreeWnd->GetPrevSibling(selection);
	IMetaObject* metaObject = GetMetaObject(selection);
	if (metaObject != nullptr && nextItem.IsOk()) {
		const wxTreeItemId& parentItem = m_metaTreeWnd->GetItemParent(nextItem);
		wxTreeItemIdValue coockie; wxTreeItemId nextId = m_metaTreeWnd->GetFirstChild(parentItem, coockie);
		size_t pos = 0;
		do {
			if (nextId == nextItem)
				break;
			nextId = m_metaTreeWnd->GetNextChild(nextId, coockie); pos++;
		} while (nextId.IsOk());
		IMetaObject* parentObject = metaObject->GetParent();
		IMetaObject* nextObject = GetMetaObject(nextItem);
		if (parentObject->ChangeChildPosition(metaObject, parentObject->GetChildPosition(nextObject))) {
			wxTreeItemId newId = m_metaTreeWnd->InsertItem(parentItem,
				pos + 2,
				m_metaTreeWnd->GetItemText(nextItem),
				m_metaTreeWnd->GetItemImage(nextItem),
				m_metaTreeWnd->GetItemImage(nextItem),
				m_metaTreeWnd->GetItemData(nextItem)
			);

			auto tree = m_metaTreeWnd;
			std::function<void(CDataReportTreeWnd*, const wxTreeItemId&, const wxTreeItemId&)> swap = [&swap](CDataReportTreeWnd* tree, const wxTreeItemId& dst, const wxTreeItemId& src) {
				wxTreeItemIdValue coockie; wxTreeItemId nextId = tree->GetFirstChild(dst, coockie);
				while (nextId.IsOk()) {
					wxTreeItemId newId = tree->AppendItem(src,
						tree->GetItemText(nextId),
						tree->GetItemImage(nextId),
						tree->GetItemImage(nextId),
						tree->GetItemData(nextId)
					);
					if (tree->HasChildren(nextId)) {
						swap(tree, nextId, newId);
					}
					tree->SetItemData(nextId, nullptr);
					nextId = tree->GetNextChild(nextId, coockie);
				}
			};

			swap(tree, nextItem, newId);

			m_metaTreeWnd->SetItemData(nextItem, nullptr);
			m_metaTreeWnd->Delete(nextItem);

			//m_metaTreeWnd->Expand(newId);
		}
	}
	m_metaTreeWnd->Thaw();
}

void CDataReportTree::DownItem()
{
	if (appData->GetAppMode() != eRunMode::eDESIGNER_MODE)
		return;

	m_metaTreeWnd->Freeze();
	const wxTreeItemId& selection = m_metaTreeWnd->GetSelection();
	const wxTreeItemId& prevItem = m_metaTreeWnd->GetNextSibling(selection);
	IMetaObject* metaObject = GetMetaObject(selection);
	if (metaObject != nullptr && prevItem.IsOk()) {
		const wxTreeItemId& parentItem = m_metaTreeWnd->GetItemParent(prevItem);
		wxTreeItemIdValue coockie; wxTreeItemId nextId = m_metaTreeWnd->GetFirstChild(parentItem, coockie);
		size_t pos = 0;
		do {
			if (nextId == prevItem)
				break;
			nextId = m_metaTreeWnd->GetNextChild(nextId, coockie); pos++;
		} while (nextId.IsOk());
		IMetaObject* parentObject = metaObject->GetParent();
		IMetaObject* prevObject = GetMetaObject(prevItem);
		if (parentObject->ChangeChildPosition(metaObject, parentObject->GetChildPosition(prevObject))) {
			wxTreeItemId newId = m_metaTreeWnd->InsertItem(parentItem,
				pos - 1,
				m_metaTreeWnd->GetItemText(prevItem),
				m_metaTreeWnd->GetItemImage(prevItem),
				m_metaTreeWnd->GetItemImage(prevItem),
				m_metaTreeWnd->GetItemData(prevItem)
			);

			auto tree = m_metaTreeWnd;
			std::function<void(CDataReportTreeWnd*, const wxTreeItemId&, const wxTreeItemId&)> swap = [&swap](CDataReportTreeWnd* tree, const wxTreeItemId& dst, const wxTreeItemId& src) {
				wxTreeItemIdValue coockie; wxTreeItemId nextId = tree->GetFirstChild(dst, coockie);
				while (nextId.IsOk()) {
					wxTreeItemId newId = tree->AppendItem(src,
						tree->GetItemText(nextId),
						tree->GetItemImage(nextId),
						tree->GetItemImage(nextId),
						tree->GetItemData(nextId)
					);
					if (tree->HasChildren(nextId)) {
						swap(tree, nextId, newId);
					}
					tree->SetItemData(nextId, nullptr);
					nextId = tree->GetNextChild(nextId, coockie);
				}
			};

			swap(tree, prevItem, newId);

			m_metaTreeWnd->SetItemData(prevItem, nullptr);
			m_metaTreeWnd->Delete(prevItem);

			//m_metaTreeWnd->Expand(newId);
		}
	}
	m_metaTreeWnd->Thaw();
}

void CDataReportTree::SortItem()
{
	if (appData->GetAppMode() != eRunMode::eDESIGNER_MODE)
		return;
	m_metaTreeWnd->Freeze();
	const wxTreeItemId& selection = m_metaTreeWnd->GetSelection();
	IMetaObject* prevObject = GetMetaObject(selection);
	if (prevObject != nullptr && selection.IsOk()) {
		const wxTreeItemId& parentItem =
			m_metaTreeWnd->GetItemParent(selection);
		if (parentItem.IsOk()) {
			m_metaTreeWnd->SortChildren(parentItem);
		}
	}
	m_metaTreeWnd->Thaw();
}

void CDataReportTree::CommandItem(unsigned int id)
{
	if (appData->GetAppMode() != eRunMode::eDESIGNER_MODE)
		return;
	wxTreeItemId sel = m_metaTreeWnd->GetSelection();
	IMetaObject* metaObject = GetMetaObject(sel);
	if (!metaObject)
		return;
	metaObject->ProcessCommand(id);
}

#include <wx/artprov.h>

void CDataReportTree::PrepareContextMenu(wxMenu* defaultMenu, const wxTreeItemId& item)
{
	IMetaObject* metaObject = GetMetaObject(item);

	if (metaObject
		&& !metaObject->PrepareContextMenu(defaultMenu))
	{
		wxMenuItem* menuItem = defaultMenu->Append(ID_METATREE_NEW, _("New"));
		menuItem->SetBitmap(wxArtProvider::GetBitmap(wxART_PLUS, wxART_MENU));
		menuItem->Enable(!m_bReadOnly);
		menuItem = defaultMenu->Append(ID_METATREE_EDIT, _("Edit"));
		menuItem->SetBitmap(wxArtProvider::GetBitmap(wxART_EDIT, wxART_MENU));
		menuItem = defaultMenu->Append(ID_METATREE_REMOVE, _("Remove"));
		menuItem->SetBitmap(wxArtProvider::GetBitmap(wxART_DELETE, wxART_MENU));
		menuItem->Enable(!m_bReadOnly);
		defaultMenu->AppendSeparator();
		menuItem = defaultMenu->Append(ID_METATREE_PROPERTY, _("Properties"));
		menuItem->SetBitmap(wxArtProvider::GetBitmap(wxART_LIST_VIEW, wxART_MENU));
	}
	else if (!metaObject) {
		wxMenuItem* menuItem = defaultMenu->Append(ID_METATREE_NEW, _("New"));
		menuItem->SetBitmap(wxArtProvider::GetBitmap(wxART_PLUS, wxART_MENU));
		menuItem->Enable(!m_bReadOnly);
	}
}

void CDataReportTree::UpdateToolbar(IMetaObject* obj, const wxTreeItemId& item)
{
	m_metaTreeToolbar->EnableTool(ID_METATREE_NEW, item != m_metaTreeWnd->GetRootItem() && !m_bReadOnly);
	m_metaTreeToolbar->EnableTool(ID_METATREE_EDIT, obj != nullptr && item != m_metaTreeWnd->GetRootItem());
	m_metaTreeToolbar->EnableTool(ID_METATREE_REMOVE, obj != nullptr && item != m_metaTreeWnd->GetRootItem() && !m_bReadOnly);

	m_metaTreeToolbar->EnableTool(ID_METATREE_UP, obj != nullptr && item != m_metaTreeWnd->GetRootItem() && !m_bReadOnly);
	m_metaTreeToolbar->EnableTool(ID_METATREE_DOWM, obj != nullptr && item != m_metaTreeWnd->GetRootItem() && !m_bReadOnly);
	m_metaTreeToolbar->EnableTool(ID_METATREE_SORT, obj != nullptr && item != m_metaTreeWnd->GetRootItem() && !m_bReadOnly);

	m_metaTreeToolbar->Refresh();
}

void CDataReportTree::UpdateChoiceSelection()
{
	m_defaultFormValue->Clear();
	m_defaultFormValue->AppendString(_("<not selected>"));

	CMetaObjectReport* commonMetadata = m_metaData->GetReport();
	wxASSERT(commonMetadata);

	int defSelection = 0;

	for (auto metaForm : commonMetadata->GetObjectForms())
	{
		if (CMetaObjectReport::eFormReport != metaForm->GetTypeForm())
			continue;

		int selection_id = m_defaultFormValue->Append(metaForm->GetName(), reinterpret_cast<void*>(metaForm->GetMetaID()));

		if (commonMetadata->GetDefFormObject() == metaForm->GetMetaID()) {
			defSelection = selection_id;
		}
	}

	m_defaultFormValue->SetSelection(defSelection);
	m_defaultFormValue->SendSelectionChangedEvent(wxEVT_CHOICE);
}

bool CDataReportTree::RenameMetaObject(IMetaObject* obj, const wxString& sNewName)
{
	wxTreeItemId curItem = m_metaTreeWnd->GetSelection();

	if (!curItem.IsOk())
		return false;

	if (m_metaData->RenameMetaObject(obj, sNewName)) {
		CMetaDocument* currDocument = GetDocument(obj);
		if (currDocument != nullptr) {
			currDocument->SetTitle(obj->GetClassName() + wxT(": ") + sNewName);
			currDocument->OnChangeFilename(true);
		}
		//update choice if need
		UpdateChoiceSelection();

		m_metaTreeWnd->SetItemText(curItem, sNewName);
		return true;
	}

	return false;
}

void CDataReportTree::InitTree()
{
	m_treeREPORTS = AppendRootItem(g_metaReportCLSID, _("reports"));
	//������ ���������� 
	m_treeATTRIBUTES = AppendGroupItem(m_treeREPORTS, g_metaAttributeCLSID, objectAttributesName);
	//������ ��������� ������ 
	m_treeTABLES = AppendGroupItem(m_treeREPORTS, g_metaTableCLSID, objectTablesName);
	//�����
	m_treeFORM = AppendGroupItem(m_treeREPORTS, g_metaFormCLSID, objectFormsName);
	//�������
	m_treeTEMPLATES = AppendGroupItem(m_treeREPORTS, g_metaTemplateCLSID, objectTablesName);
}

void CDataReportTree::ClearTree()
{
	for (auto& doc : docManager->GetDocumentsVector()) {
		CMetaDocument* metaDoc = wxDynamicCast(doc, CMetaDocument);
		IMetaObject* metaObject = metaDoc->GetMetaObject();
		if (metaObject != nullptr && this == metaObject->GetMetaDataTree()) {
			doc->DeleteAllViews();
		}
	}

	//delete all child item
	if (m_treeATTRIBUTES.IsOk()) m_metaTreeWnd->DeleteChildren(m_treeATTRIBUTES);
	if (m_treeTABLES.IsOk()) m_metaTreeWnd->DeleteChildren(m_treeTABLES);
	if (m_treeFORM.IsOk()) m_metaTreeWnd->DeleteChildren(m_treeFORM);
	if (m_treeTEMPLATES.IsOk()) m_metaTreeWnd->DeleteChildren(m_treeTEMPLATES);

	//delete all items
	m_metaTreeWnd->DeleteAllItems();

	//Initialize tree
	InitTree();
}

void CDataReportTree::FillData()
{
	CMetaObjectReport* commonMetadata = m_metaData->GetReport();
	wxASSERT(commonMetadata);
	m_metaTreeWnd->SetItemText(m_treeREPORTS, commonMetadata->GetName());
	m_metaTreeWnd->SetItemData(m_treeREPORTS, new wxTreeItemMetaData(commonMetadata));

	//set value data
	m_nameValue->SetValue(commonMetadata->GetName());
	m_synonymValue->SetValue(commonMetadata->GetSynonym());
	m_commentValue->SetValue(commonMetadata->GetComment());

	//set default form value 
	m_defaultFormValue->Clear();
	//append default value 
	m_defaultFormValue->AppendString(_("<not selected>"));

	//������ ���������� 
	for (auto metaAttribute : commonMetadata->GetObjectAttributes()) {
		if (metaAttribute->IsDeleted())
			continue;
		if (metaAttribute->GetClassType() == g_metaDefaultAttributeCLSID)
			continue;
		wxTreeItemId hItem = AppendItem(m_treeATTRIBUTES, metaAttribute);
	}

	//������ ��������� ������ 
	for (auto metaTable : commonMetadata->GetObjectTables()) {
		if (metaTable->IsDeleted())
			continue;
		wxTreeItemId hItem = AppendGroupItem(m_treeTABLES, g_metaAttributeCLSID, metaTable);
		for (auto metaAttribute : metaTable->GetObjectAttributes()) {
			if (metaAttribute->IsDeleted())
				continue;
			if (metaAttribute->GetClassType() == g_metaDefaultAttributeCLSID)
				continue;
			wxTreeItemId hItemNew = AppendItem(hItem, metaAttribute);
		}
	}

	//�����
	for (auto metaForm : commonMetadata->GetObjectForms()) {
		if (metaForm->IsDeleted())
			continue;
		wxTreeItemId hItem = AppendItem(m_treeFORM, metaForm);
	}

	//�������
	for (auto metaTemplates : commonMetadata->GetObjectTemplates()) {
		if (metaTemplates->IsDeleted())
			continue;
		wxTreeItemId hItem = AppendItem(m_treeTEMPLATES, metaTemplates);
	}

	//update choice selection
	UpdateChoiceSelection();

	//set init flag
	m_initialized = true;

	//set modify 
	Modify(m_metaData->IsModified());

	//update toolbar 
	UpdateToolbar(nullptr, m_treeATTRIBUTES);
}

bool CDataReportTree::Load(CMetaDataReport* metaData)
{
	ClearTree();
	m_metaData = metaData;
	m_metaTreeWnd->Freeze();
	FillData(); //Fill all data from metaData
	m_metaData->SetMetaTree(this);
	m_metaTreeWnd->SelectItem(m_treeATTRIBUTES);
	m_metaTreeWnd->ExpandAll();
	m_metaTreeWnd->Thaw();
	return true;
}

bool CDataReportTree::Save()
{
	CMetaObjectReport* m_commonMetadata = m_metaData->GetReport();
	wxASSERT(m_commonMetadata);

	m_commonMetadata->SetName(m_nameValue->GetValue());
	m_commonMetadata->SetSynonym(m_synonymValue->GetValue());
	m_commonMetadata->SetComment(m_commentValue->GetValue());

	wxASSERT(m_metaData);

	if (m_metaData->IsModified())
		return m_metaData->SaveConfiguration();

	return false;
}