#include "advpropOwner.h"
#include "backend/propertyManager/property/variant/ownerVariant.h"
#include "backend/propertyManager/propertyEditor.h"

#define icon_size 16

// -----------------------------------------------------------------------
// wxPGOwnerProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxPGOwnerProperty, wxPGProperty, ComboBoxAndButton)

#include "backend/metadata.h"

void wxPGOwnerProperty::FillByClsid(const class_identifier_t& clsid)
{
	IMetaObjectGenericData* metaGenericData = dynamic_cast<IMetaObjectGenericData*>(m_ownerProperty);
	if (metaGenericData != nullptr) {
		IMetaData* metaData = metaGenericData->GetMetaData();
		wxASSERT(metaData);
		for (auto metaOwner : metaData->GetMetaObject(clsid)) {
			m_choices.Add(metaOwner->GetSynonym(), metaOwner->GetIcon(), metaOwner->GetMetaID());
		}
	}
}

wxPGOwnerProperty::wxPGOwnerProperty(IPropertyObject* property, const wxString& label, const wxString& strName, const wxVariant& value)
	: wxPGProperty(label, strName), m_ownerProperty(property)
{
	FillByClsid(g_metaCatalogCLSID);

	//m_flags |= wxPG_PROP_READONLY;
	m_flags |= wxPG_PROP_ACTIVE_BTN; // Property button always enabled.

	SetValue(value);
}

wxString wxPGOwnerProperty::ValueToString(wxVariant& value, int argFlags) const
{
	return value.GetString();
}

bool wxPGOwnerProperty::StringToValue(wxVariant& variant,
	const wxString& text,
	int argFlags) const
{
	return false;
}

bool wxPGOwnerProperty::IntToValue(wxVariant& value, int number, int argFlags) const
{
	wxVariantDataOwner* dataOwner = property_cast(value, wxVariantDataOwner);
	if (dataOwner != nullptr) {
		wxVariantDataOwner* newDataOwner = dataOwner->Clone();
		wxASSERT(newDataOwner);
		CMetaDescription& md = newDataOwner->GetMetaDesc();
		md.SetDefaultMetaType(m_choices.GetValue(number));
		value = newDataOwner;
		return true;
	}
	return false;
}

#include "backend/propertyManager/property/advprop/ctrl/checktree.h"

wxPGEditorDialogAdapter* wxPGOwnerProperty::GetEditorDialog() const
{
	class wxPGOwnerEventAdapter : public wxPGEditorDialogAdapter {
		
		class wxTreeItemOptionData : public wxTreeItemData {
			IMetaObject* m_metaObject;
		public:
			wxTreeItemOptionData(IMetaObject* opt) : wxTreeItemData(), m_metaObject(opt) {}
			meta_identifier_t GetMetaID() const { return m_metaObject->GetMetaID(); }
		};

		void FillByClsid(IMetaData* metaData, const class_identifier_t& clsid,
			wxPropertyCheckTree* tc, wxVariantDataOwner* data) {

			wxImageList* imageList = tc->GetImageList();
			wxASSERT(imageList);
			IAbstractTypeCtor* so = CValue::GetAvailableCtor(clsid);
			int groupIcon = imageList->Add(so->GetClassIcon());
			const wxTreeItemId& parentID = tc->AppendItem(tc->GetRootItem(), so->GetClassName(),
				groupIcon, groupIcon);
			for (auto metaObject : metaData->GetMetaObject(clsid)) {
				IMetaObjectRecordDataRef* registerData = dynamic_cast<IMetaObjectRecordDataRef*>(metaObject);
				if (registerData != nullptr) {
					{
						int icon = imageList->Add(registerData->GetIcon());
						wxTreeItemOptionData* itemData = new wxTreeItemOptionData(metaObject);
						wxTreeItemId newItem = tc->AppendItem(parentID, registerData->GetSynonym(),
							icon, icon,
							itemData);

						if (data != nullptr) {
							const CMetaDescription& md = data->GetMetaDesc();
							tc->SetItemState(newItem, md.ContainMetaType(registerData->GetMetaID()) ? wxPropertyCheckTree::CHECKED : wxPropertyCheckTree::UNCHECKED);
							tc->Check(newItem, md.ContainMetaType(registerData->GetMetaID()));
						}
						else {
							tc->SetItemState(newItem, wxPropertyCheckTree::UNCHECKED);
							tc->Check(newItem, false);
						}
					}
				}
			}
		}

	public:

		virtual bool DoShowDialog(wxPropertyGrid* pg, wxPGProperty* prop) wxOVERRIDE
		{
			wxPGOwnerProperty* dlgProp = wxDynamicCast(prop, wxPGOwnerProperty);
			wxCHECK_MSG(dlgProp, false, "Function called for incompatible property");

			wxVariantDataOwner* data = property_cast(dlgProp->GetValue(), wxVariantDataOwner);
			if (data == nullptr) return false;
			IMetaObjectGenericData* metaGenericData = dynamic_cast<IMetaObjectGenericData*>(dlgProp->GetPropertyObject());
			if (metaGenericData == nullptr) return false;

			// launch editor dialog
			wxDialog* dlg = new wxDialog(pg, wxID_ANY, _("Choice owner"), wxDefaultPosition, wxDefaultSize,
				wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCLIP_CHILDREN);

			dlg->SetFont(pg->GetFont()); // To allow entering chars of the same set as the propGrid

			// Multi-line text editor dialog.
			const int spacing = wxPropertyGrid::IsSmallScreen() ? 4 : 8;
			wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
			wxBoxSizer* rowsizer = new wxBoxSizer(wxHORIZONTAL);

			wxPropertyCheckTree* tc = new wxPropertyCheckTree(dlg, wxID_ANY,
				wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_NO_LINES | wxTR_HIDE_ROOT | wxPR_MULTIPLE_CHECK | wxPR_EMPTY_CHECK | wxSUNKEN_BORDER);

			wxTreeItemId rootItem = tc->AddRoot(wxEmptyString);

			rowsizer->Add(tc, wxSizerFlags(1).Expand().Border(wxALL, spacing));
			topsizer->Add(rowsizer, wxSizerFlags(1).Expand());

			tc->SetDoubleBuffered(true);
			tc->Enable(!dlgProp->HasFlag(wxPG_PROP_READONLY));

			wxStdDialogButtonSizer* buttonSizer = dlg->CreateStdDialogButtonSizer(wxOK | wxCANCEL);
			topsizer->Add(buttonSizer, wxSizerFlags(0).Right().Border(wxBOTTOM | wxRIGHT, spacing));

			dlg->SetSizer(topsizer);
			topsizer->SetSizeHints(dlg);

			if (!wxPropertyGrid::IsSmallScreen()) {
				dlg->SetSize(400, 300);
				dlg->Move(pg->GetGoodEditorDialogPosition(prop, dlg->GetSize()));
			}

			tc->SetFocus();

			// Make an state image list containing small icons
			tc->SetImageList(
				new wxImageList(icon_size, icon_size)
			);

			IMetaData* metaData = metaGenericData->GetMetaData();
			wxASSERT(metaData);
			if (metaData != nullptr) {
				FillByClsid(metaData, g_metaCatalogCLSID, tc, data);
			}

			tc->ExpandAll(); int res = dlg->ShowModal();

			wxVariantDataOwner* clone = data->Clone();
			{
				CMetaDescription& metaDesc = clone->GetMetaDesc(); metaDesc.ClearMetaType();
				wxArrayTreeItemIds ids;
				unsigned int selCount = tc->GetSelections(ids);
				for (const wxTreeItemId& selItem : ids) {
					if (selItem.IsOk()) {
						wxTreeItemData* dataItem = tc->GetItemData(selItem);
						if (dataItem && res == wxID_OK) {
							wxTreeItemOptionData* item = dynamic_cast<wxTreeItemOptionData*>(dataItem);
							wxASSERT(item);
							metaDesc.AppendMetaType(item->GetMetaID());
						}
					}
				}
			}
			SetValue(clone);

			dlg->Destroy();
			return res == wxID_OK;
		}
	};

	return new wxPGOwnerEventAdapter();
}