#include "tableBox.h"
#include "frontend/visualView/dvc/dvc.h"

//***********************************************************************************
//*                           IMPLEMENT_DYNAMIC_CLASS                               *
//***********************************************************************************

wxIMPLEMENT_DYNAMIC_CLASS(CValueTableBoxColumn, IValueControl);

//****************************************************************************
#include "backend/metaData.h"
#include "backend/objCtor.h"


OptionList* CValueTableBoxColumn::GetChoiceForm(PropertyOption* property)
{
	OptionList* optList = new OptionList;
	optList->AddOption(_("default"), wxNOT_FOUND);

	IMetaData* metaData = GetMetaData();
	if (metaData != nullptr) {
		IMetaObjectRecordDataRef* metaObjectRefValue = nullptr;
		if (m_dataSource.isValid()) {

			IMetaObjectGenericData* metaObjectValue =
				m_formOwner->GetMetaObject();

			if (metaObjectValue != nullptr) {
				IMetaObject* metaobject =
					metaObjectValue->FindMetaObjectByID(m_dataSource);

				IMetaObjectAttribute* metaAttribute = wxDynamicCast(
					metaobject, IMetaObjectAttribute
				);
				if (metaAttribute != nullptr) {

					IMetaValueTypeCtor* so = metaData->GetTypeCtor(metaAttribute->GetFirstClsid());
					if (so != nullptr) {
						metaObjectRefValue = wxDynamicCast(so->GetMetaObject(), IMetaObjectRecordDataRef);
					}
				}
				else
				{
					metaObjectRefValue = wxDynamicCast(
						metaobject, IMetaObjectRecordDataRef);
				}
			}
		}
		else {
			IMetaValueTypeCtor* so = metaData->GetTypeCtor(ITypeControlAttribute::GetFirstClsid());
			if (so != nullptr) {
				metaObjectRefValue = wxDynamicCast(so->GetMetaObject(), IMetaObjectRecordDataRef);
			}
		}

		if (metaObjectRefValue != nullptr) {
			for (auto form : metaObjectRefValue->GetObjectForms()) {
				optList->AddOption(
					form->GetSynonym(),
					form->GetMetaID()
				);
			}
		}
	}

	return optList;
}

ISourceObject* CValueTableBoxColumn::GetSourceObject() const
{
	return GetOwner();
}

//***********************************************************************************
//*                            CValueTableBoxColumn                                 *
//***********************************************************************************

CValueTableBoxColumn::CValueTableBoxColumn() :
	IValueControl(), ITypeControlAttribute()
{
}

IMetaData* CValueTableBoxColumn::GetMetaData() const
{
	return m_formOwner ?
		m_formOwner->GetMetaData() : nullptr;
}

wxObject* CValueTableBoxColumn::Create(wxWindow* wxparent, IVisualHost* visualHost)
{
	�DataViewColumnContainer* columnObject = new �DataViewColumnContainer(this, m_propertyCaption->GetValueAsString(),
		m_dataSource.isValid() ? GetIdByGuid(m_dataSource) : m_controlId, m_propertyWidth->GetValueAsInteger(),
		(wxAlignment)m_propertyAlign->GetValueAsInteger(),
		wxDATAVIEW_COL_REORDERABLE
	);

	columnObject->SetControl(this);
	columnObject->SetControlID(m_controlId);

	columnObject->SetTitle(m_propertyCaption->GetValueAsString());
	columnObject->SetWidth(m_propertyWidth->GetValueAsInteger());
	columnObject->SetAlignment((wxAlignment)m_propertyAlign->GetValueAsInteger());

	columnObject->SetBitmap(m_propertyIcon->GetValueAsBitmap());
	columnObject->SetHidden(!m_propertyVisible->GetValueAsBoolean());
	columnObject->SetSortable(false);
	columnObject->SetResizeable(m_propertyResizable->GetValueAsBoolean());

	columnObject->SetColModel(m_dataSource.isValid() ? GetIdByGuid(m_dataSource) : m_controlId);

	CValueViewRenderer* colRenderer = columnObject->GetRenderer();
	wxASSERT(colRenderer);
	return columnObject;
}

void CValueTableBoxColumn::OnCreated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost, bool first�reated)
{
	wxDataViewCtrl* tableCtrl = dynamic_cast<wxDataViewCtrl*>(wxparent);
	wxASSERT(tableCtrl);
	�DataViewColumnContainer* columnObject = dynamic_cast<�DataViewColumnContainer*>(wxobject);
	wxASSERT(columnObject);

	wxHeaderCtrl* headerCtrl = tableCtrl->GenericGetHeader();
	if (headerCtrl != nullptr)
		headerCtrl->ResetColumnsOrder();
	tableCtrl->AppendColumn(columnObject);
}

#include "backend/appData.h"

void CValueTableBoxColumn::OnUpdated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost)
{
	IValueFrame* parentControl = GetParent(); int idx = wxNOT_FOUND;

	for (unsigned int i = 0; i < parentControl->GetChildCount(); i++) {
		CValueTableBoxColumn* child = dynamic_cast<CValueTableBoxColumn*>(parentControl->GetChild(i));
		wxASSERT(child);
		if (m_dataSource.isValid() && m_dataSource == child->m_dataSource) { idx = i; break; }
		else if (!m_dataSource.isValid() && m_controlId == child->m_controlId) { idx = i; break; }
	}

	wxDataViewCtrl* tableCtrl = dynamic_cast<wxDataViewCtrl*>(wxparent);
	wxASSERT(tableCtrl);
	�DataViewColumnContainer* columnObject = dynamic_cast<�DataViewColumnContainer*>(wxobject);
	wxASSERT(columnObject);

	columnObject->SetControl(this);

	wxString textCaption = m_propertyName->GetValueAsString();
	if (m_dataSource.isValid()) {
		IMetaObject* metaObject = GetMetaSource();
		if (metaObject != nullptr) {
			textCaption = metaObject->GetSynonym();
		}
	}

	columnObject->SetTitle(!m_propertyCaption->IsOk() ?
		textCaption : m_propertyCaption->GetValueAsString());
	columnObject->SetWidth(m_propertyWidth->GetValueAsInteger());
	columnObject->SetAlignment((wxAlignment)m_propertyAlign->GetValueAsInteger());

	IValueModel* modelValue = GetOwner()->GetModel();
	sortOrder_t::sortData_t* sort = modelValue != nullptr ? modelValue->GetSortByID(m_dataSource.isValid() ? GetIdByGuid(m_dataSource) : m_controlId) : false;

	columnObject->SetBitmap(m_propertyIcon->GetValueAsBitmap());
	columnObject->SetHidden(!m_propertyVisible->GetValueAsBoolean());
	columnObject->SetSortable(sort != nullptr && !appData->DesignerMode());
	columnObject->SetResizeable(m_propertyResizable->GetValueAsBoolean());

	if (sort != nullptr && sort->m_sortEnable && !sort->m_sortSystem && !appData->DesignerMode())
		columnObject->SetSortOrder(sort->m_sortAscending);

	columnObject->SetColModel(m_dataSource.isValid() ? GetIdByGuid(m_dataSource) : m_controlId);

	wxHeaderCtrl* headerCtrl = tableCtrl->GenericGetHeader();
	if (headerCtrl != nullptr) {
		unsigned int model_index = tableCtrl->GetColumnIndex(columnObject);
		unsigned int col_header_index = headerCtrl->GetColumnPos(model_index);
		if (col_header_index != idx && tableCtrl->DeleteColumn(columnObject))
			tableCtrl->InsertColumn(idx, columnObject);
	}
	else {
		unsigned int model_index = tableCtrl->GetColumnIndex(columnObject);
		if (model_index != idx && tableCtrl->DeleteColumn(columnObject))
			tableCtrl->InsertColumn(idx, columnObject);
	}
}

void CValueTableBoxColumn::Cleanup(wxObject* obj, IVisualHost* visualHost)
{
	wxDataViewCtrl* tableCtrl = dynamic_cast<wxDataViewCtrl*>(visualHost->GetWxObject(GetOwner()));
	wxASSERT(tableCtrl);
	�DataViewColumnContainer* columnObject = dynamic_cast<�DataViewColumnContainer*>(obj);
	wxASSERT(columnObject);
	wxHeaderCtrl* headerCtrl = tableCtrl->GenericGetHeader();
	if (headerCtrl != nullptr) {
		columnObject->SetHidden(false);
		headerCtrl->ResetColumnsOrder();
	}
	tableCtrl->DeleteColumn(columnObject);
}

bool CValueTableBoxColumn::CanDeleteControl() const
{
	return m_parent->GetChildCount() > 1;
}

#include "backend/metaCollection/partial/object.h"

//*******************************************************************
//*							 Control value	                        *
//*******************************************************************

bool CValueTableBoxColumn::FilterSource(const CSourceExplorer& src, const meta_identifier_t& id)
{
	return GetIdByGuid(GetOwner()->m_dataSource) == id;
}

#include "frontend/visualView/dvc/dvc.h"
#include "frontend/win/ctrls/textEditor.h"

bool CValueTableBoxColumn::SetControlValue(const CValue& varControlVal)
{
	IValueTable::IValueModelReturnLine* retLine = GetReturnLine();
	if (retLine != nullptr) {
		retLine->SetValueByMetaID(
			m_dataSource.isValid() ? GetIdByGuid(m_dataSource) : m_controlId, varControlVal
		);
	}
	�DataViewColumnContainer* columnObject =
		dynamic_cast<�DataViewColumnContainer*>(GetWxObject());
	if (columnObject != nullptr) {
		CValueViewRenderer* renderer = columnObject->GetRenderer();
		wxASSERT(renderer);
		wxTextContainerCtrl* textEditor = dynamic_cast<wxTextContainerCtrl*>(renderer->GetEditorCtrl());
		if (textEditor != nullptr) {
			textEditor->SetTextValue(varControlVal.GetString());
			textEditor->SetInsertionPointEnd();
			textEditor->SetFocus();
		}
		else {
			renderer->FinishSelecting();
		}
	}

	m_formOwner->RefreshForm();
	return true;
}

bool CValueTableBoxColumn::GetControlValue(CValue& pvarControlVal) const
{
	IValueTable::IValueModelReturnLine* retLine = GetReturnLine();
	if (retLine != nullptr) {
		return retLine->GetValueByMetaID(
			m_dataSource.isValid() ? GetIdByGuid(m_dataSource) : m_controlId, pvarControlVal
		);
	}
	return false;
}

//***********************************************************************************
//*                                  Data											*
//***********************************************************************************

bool CValueTableBoxColumn::LoadData(CMemoryReader& reader)
{
	wxString caption; reader.r_stringZ(caption);
	m_propertyCaption->SetValue(caption);

	m_propertyPasswordMode->SetValue(reader.r_u8());
	m_propertyMultilineMode->SetValue(reader.r_u8());
	m_propertyTexteditMode->SetValue(reader.r_u8());

	m_propertySelectButton->SetValue(reader.r_u8());
	m_propertyOpenButton->SetValue(reader.r_u8());
	m_propertyClearButton->SetValue(reader.r_u8());

	m_propertyAlign->SetValue(reader.r_s32());
	m_propertyWidth->SetValue(reader.r_s32());
	m_propertyVisible->SetValue(reader.r_u8());
	m_propertyResizable->SetValue(reader.r_u8());
	//m_propertySortable->SetValue(reader.r_u8());
	m_propertyReorderable->SetValue(reader.r_u8());

	m_propertyChoiceForm->SetValue(reader.r_s32());

	if (!ITypeControlAttribute::LoadTypeData(reader))
		return false;

	return IValueControl::LoadData(reader);
}

bool CValueTableBoxColumn::SaveData(CMemoryWriter& writer)
{
	writer.w_stringZ(m_propertyCaption->GetValueAsString());

	writer.w_u8(m_propertyPasswordMode->GetValueAsBoolean());
	writer.w_u8(m_propertyMultilineMode->GetValueAsBoolean());
	writer.w_u8(m_propertyTexteditMode->GetValueAsBoolean());

	writer.w_u8(m_propertySelectButton->GetValueAsBoolean());
	writer.w_u8(m_propertyOpenButton->GetValueAsBoolean());
	writer.w_u8(m_propertyClearButton->GetValueAsBoolean());

	writer.w_s32(m_propertyAlign->GetValueAsInteger());
	writer.w_s32(m_propertyWidth->GetValueAsInteger());
	writer.w_u8(m_propertyVisible->GetValueAsBoolean());
	writer.w_u8(m_propertyResizable->GetValueAsBoolean());
	//writer.w_u8(m_propertySortable->GetValueAsBoolean());
	writer.w_u8(m_propertyReorderable->GetValueAsBoolean());

	writer.w_s32(m_propertyChoiceForm->GetValueAsInteger());

	if (!ITypeControlAttribute::SaveTypeData(writer))
		return false;

	return IValueControl::SaveData(writer);
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

S_CONTROL_TYPE_REGISTER(CValueTableBoxColumn, "tableboxColumn", "tableboxColumn", g_controlTableBoxColumnCLSID);