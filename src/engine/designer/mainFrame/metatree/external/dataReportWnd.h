#ifndef _DATAREPORT_WND_H__
#define _DATAREPORT_WND_H__

#include <wx/aui/aui.h>
#include <wx/aui/auibar.h>
#include <wx/treectrl.h>
#include <wx/statbox.h>
#include <wx/statline.h>

#include "mainFrame/metatree/metatreeWnd.h"
#include "backend/external/metadataReport.h"

class CDataReportTree : public IMetaDataTree {
	wxDECLARE_DYNAMIC_CLASS(CDataReportTree);
private:

	wxTreeItemId m_treeREPORTS;

	wxTreeItemId m_treeATTRIBUTES;
	wxTreeItemId m_treeTABLES;
	wxTreeItemId m_treeFORM;
	wxTreeItemId m_treeTEMPLATES;

private:

	bool m_initialized;

private:

	wxTreeItemId GetSelectionIdentifier() const {
		wxTreeItemId parentItem = m_metaTreeWnd->GetSelection();
		while (parentItem != nullptr) {
			wxTreeItemData* item = m_metaTreeWnd->GetItemData(parentItem);
			if (item != nullptr) {
				CTreeDataClassIdentifier* item_clsid = dynamic_cast<CTreeDataClassIdentifier*>(item);
				if (item_clsid != nullptr) return parentItem;
			}
			parentItem = m_metaTreeWnd->GetItemParent(parentItem);
		}
		return wxTreeItemId(nullptr);
	}

	class_identifier_t GetClassIdentifier() const {
		wxTreeItemData* item = m_metaTreeWnd->GetItemData(GetSelectionIdentifier());
		if (item != nullptr) {
			CTreeDataClassIdentifier* item_clsid = dynamic_cast<CTreeDataClassIdentifier*>(item);
			if (item_clsid != nullptr) return item_clsid->m_clsid;
		}
		return 0;
	}

	IMetaObject* GetMetaIdentifier() const {
		wxTreeItemId parentItem = GetSelectionIdentifier();
		wxTreeItemData* item = m_metaTreeWnd->GetItemData(parentItem);
		if (item != nullptr) {
			CTreeDataClassIdentifier* item_clsid = dynamic_cast<CTreeDataClassIdentifier*>(item);
			if (item_clsid != nullptr) {
				while (parentItem != nullptr) {
					wxTreeItemData* item = m_metaTreeWnd->GetItemData(parentItem);
					if (item != nullptr) {
						IMetaObject* metaParent = GetMetaObject(parentItem);
						if (metaParent != nullptr) return metaParent;
					}
					parentItem = m_metaTreeWnd->GetItemParent(parentItem);
				}
			}
		}
		return nullptr;
	}

protected:

	void OnEditCaptionName(wxCommandEvent& event);
	void OnEditCaptionSynonym(wxCommandEvent& event);
	void OnEditCaptionComment(wxCommandEvent& event);

	void OnChoiceDefForm(wxCommandEvent& event);

	void OnButtonModuleClicked(wxCommandEvent& event);

protected:

	wxStaticText* m_nameCaption;
	wxStaticText* m_synonymCaption;
	wxStaticText* m_commentCaption;
	wxStaticText* m_defaultForm;
	wxTextCtrl* m_nameValue;
	wxTextCtrl* m_synonymValue;
	wxTextCtrl* m_commentValue;
	wxChoice* m_defaultFormValue;

	wxButton* m_buttonModule;

	class CDataReportTreeWnd : public wxTreeCtrl {
		wxDECLARE_DYNAMIC_CLASS(CMetadataTree);
	private:
		CDataReportTree* m_ownerTree;
	public:

		CDataReportTreeWnd();
		CDataReportTreeWnd(wxWindow* parentWnd, CDataReportTree* ownerWnd);
		virtual ~CDataReportTreeWnd();

		// this function is called to compare 2 items and should return -1, 0
		// or +1 if the first item is less than, equal to or greater than the
		// second one. The base class version performs alphabetic comparison
		// of item labels (GetText)
		virtual int OnCompareItems(const wxTreeItemId& item1,
			const wxTreeItemId& item2) {
			int ret = wxStrcmp(GetItemText(item1), GetItemText(item2));
			CTreeDataMetaItem* data1 = dynamic_cast<CTreeDataMetaItem*>(GetItemData(item1));
			CTreeDataMetaItem* data2 = dynamic_cast<CTreeDataMetaItem*>(GetItemData(item2));
			if (data1 != nullptr && data2 != nullptr && ret > 0) {
				IMetaObject* metaObject1 = data1->m_metaObject;
				IMetaObject* metaObject2 = data2->m_metaObject;
				IMetaObject* parent = metaObject1->GetParent();
				wxASSERT(parent);
				return parent->ChangeChildPosition(metaObject2,
					parent->GetChildPosition(metaObject1)
				) ? ret : wxNOT_FOUND;
			}
			return ret;
		}

		//events:
		void OnLeftDClick(wxMouseEvent& event);
		void OnLeftUp(wxMouseEvent& event);
		void OnLeftDown(wxMouseEvent& event);
		void OnRightUp(wxMouseEvent& event);
		void OnRightDClick(wxMouseEvent& event);
		void OnRightDown(wxMouseEvent& event);
		void OnKeyUp(wxKeyEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnMouseMove(wxMouseEvent& event);

		void OnCreateItem(wxCommandEvent& event);
		void OnEditItem(wxCommandEvent& event);
		void OnRemoveItem(wxCommandEvent& event);
		void OnPropertyItem(wxCommandEvent& event);

		void OnUpItem(wxCommandEvent& event);
		void OnDownItem(wxCommandEvent& event);

		void OnSortItem(wxCommandEvent& event);

		void OnCommandItem(wxCommandEvent& event);

		void OnCopyItem(wxCommandEvent& event);
		void OnPasteItem(wxCommandEvent& event);

		//void OnDebugEvent(wxDebugEvent& event);

		void OnSelecting(wxTreeEvent& event);
		void OnSelected(wxTreeEvent& event);

		void OnCollapsing(wxTreeEvent& event);
		void OnExpanding(wxTreeEvent& event);

	protected:

		wxDECLARE_EVENT_TABLE();
	};

	CDataReportTreeWnd* m_metaTreeWnd;
	CMetaDataReport* m_metaData;

private:

	wxTreeItemId AppendRootItem(const class_identifier_t& clsid, const wxString& name = wxEmptyString) const {
		IAbstractTypeCtor* typeCtor = CValue::GetAvailableCtor(clsid);
		wxASSERT(typeCtor);
		wxImageList* imageList = m_metaTreeWnd->GetImageList();
		wxASSERT(imageList);
		int imageIndex = imageList->Add(typeCtor->GetClassIcon());
		return m_metaTreeWnd->AddRoot(name.IsEmpty() ? typeCtor->GetClassName() : name,
			imageIndex,
			imageIndex,
			nullptr
		);
	}

	wxTreeItemId AppendGroupItem(const wxTreeItemId& parent,
		const class_identifier_t& clsid, const wxString& name = wxEmptyString) const {
		IAbstractTypeCtor* typeCtor = CValue::GetAvailableCtor(clsid);
		wxASSERT(typeCtor);
		wxImageList* imageList = m_metaTreeWnd->GetImageList();
		wxASSERT(imageList);
		int imageIndex = imageList->Add(typeCtor->GetClassIcon());
		return m_metaTreeWnd->AppendItem(parent, name.IsEmpty() ? typeCtor->GetClassName() : name,
			imageIndex,
			imageIndex,
			new wxTreeItemClsidData(clsid)
		);
	}

	wxTreeItemId AppendGroupItem(const wxTreeItemId& parent,
		const class_identifier_t& clsid, IMetaObject* metaObject) const {
		IAbstractTypeCtor* typeCtor = CValue::GetAvailableCtor(metaObject->GetClassType());
		wxASSERT(typeCtor);
		wxImageList* imageList = m_metaTreeWnd->GetImageList();
		wxASSERT(imageList);
		int imageIndex = imageList->Add(typeCtor->GetClassIcon());
		return m_metaTreeWnd->AppendItem(parent, metaObject->GetName(),
			imageIndex,
			imageIndex,
			new wxTreeItemClsidMetaData(clsid, metaObject)
		);
	}

	wxTreeItemId AppendItem(const wxTreeItemId& parent,
		IMetaObject* metaObject) const {
		wxImageList* imageList = m_metaTreeWnd->GetImageList();
		wxASSERT(imageList);
		int imageIndex = imageList->Add(metaObject->GetIcon());
		return m_metaTreeWnd->AppendItem(parent, metaObject->GetName(),
			imageIndex,
			imageIndex,
			new wxTreeItemMetaData(metaObject)
		);
	}

	void ActivateItem(const wxTreeItemId& item);

	IMetaObject* NewItem(const class_identifier_t& clsid, IMetaObject* metaParent);
	IMetaObject* CreateItem(bool showValue = true);

	wxTreeItemId FillItem(IMetaObject* metaItem, const wxTreeItemId& item);

	void EditItem();
	void RemoveItem();
	void EraseItem(const wxTreeItemId& item);
	void SelectItem();
	void PropertyItem();

	void Collapse();
	void Expand();

	void UpItem();
	void DownItem();

	void SortItem();

	void CommandItem(unsigned int id);
	void PrepareContextMenu(wxMenu* menu, const wxTreeItemId& item);

	void FillData();

	IMetaObject* GetMetaObject(const wxTreeItemId& item) const
	{
		if (!item.IsOk())
			return nullptr;
		CTreeDataMetaItem* data =
			dynamic_cast<CTreeDataMetaItem*>(m_metaTreeWnd->GetItemData(item));
		if (data == nullptr)
			return nullptr;
		return data->m_metaObject;
	}

	void UpdateToolbar(IMetaObject* obj, const wxTreeItemId& item);

public:

	virtual void UpdateChoiceSelection();

public:

	bool RenameMetaObject(IMetaObject* obj, const wxString& sNewName);

public:

	virtual IMetaData* GetMetaData() const { return m_metaData; }

	CDataReportTree() { }
	CDataReportTree(CMetaDocument* docParent, wxWindow* parent, wxWindowID id = wxID_ANY);
	virtual ~CDataReportTree();

	void InitTree();

	bool Load(CMetaDataReport* metaData);
	bool Save();

	void ClearTree();
};

#endif 