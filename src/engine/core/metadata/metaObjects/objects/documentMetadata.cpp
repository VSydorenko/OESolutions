////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : document metadata
////////////////////////////////////////////////////////////////////////////

#include "document.h"
#include "list/objectList.h"
#include "metadata/metadata.h"
#include "metadata/moduleManager/moduleManager.h"

#define objectModule wxT("objectModule")
#define managerModule wxT("managerModule")

wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumDocumentWriteMode, CValue);
wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumDocumentPostingMode, CValue);

wxIMPLEMENT_DYNAMIC_CLASS(CMetaObjectDocument, IMetaObjectRecordDataMutableRef);

///////////////////////////////////////////////////////////////////////////////////////////////

wxString wxVariantRecordData::MakeString() const
{
	wxString description;
	for (auto record_id : m_recordData) {
		IMetaObjectRegisterData* record =
			dynamic_cast<IMetaObjectRegisterData*>(m_metaData->GetMetaObject(record_id));
		if (record == NULL || !record->IsAllowed())
			continue;
		if (!record->HasRecorder())
			continue;
		if (description.IsEmpty()) {
			description = record->GetName();
		}
		else {
			description = description +
				", " + record->GetName();
		}
	}
	return description;
}

///////////////////////////////////////////////////////////////////////////////////////////////

#include "metadata/metadata.h"

bool CMetaObjectDocument::recordData_t::LoadData(CMemoryReader& dataReader)
{
	unsigned int count = dataReader.r_u32();

	for (unsigned int i = 0; i < count; i++) {
		meta_identifier_t record_id = dataReader.r_u32();
		m_recordData.insert(record_id);
	}

	return true;
}

bool CMetaObjectDocument::recordData_t::SaveData(CMemoryWriter& dataWritter)
{
	dataWritter.w_u32(m_recordData.size());

	for (auto record_id : m_recordData) {
		dataWritter.w_u32(record_id);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////

#include "metadata/metadata.h"

bool CMetaObjectDocument::recordData_t::LoadFromVariant(const wxVariant& variant)
{
	wxVariantRecordData* list =
		dynamic_cast<wxVariantRecordData*>(variant.GetData());
	
	if (list == NULL)
		return false;

	m_recordData.clear();
	
	for (unsigned int idx = 0; idx < list->GetCount(); idx++) {
		m_recordData.insert(
			list->GetById(idx)
		);
	}
	
	return true;
}

void CMetaObjectDocument::recordData_t::SaveToVariant(wxVariant& variant, IMetadata* metaData) const
{
	wxVariantRecordData* list = new wxVariantRecordData(metaData);

	for (auto record_id : m_recordData) {
		list->AppendRecord(record_id);
	}

	variant = list;
}

//********************************************************************************************
//*                                      metadata                                            *
//********************************************************************************************

CMetaObjectDocument::CMetaObjectDocument() : IMetaObjectRecordDataMutableRef(),
m_defaultFormObject(wxNOT_FOUND), m_defaultFormList(wxNOT_FOUND), m_defaultFormSelect(wxNOT_FOUND)
{
	PropertyContainer* categoryForm = IObjectBase::CreatePropertyContainer("DefaultForms");

	categoryForm->AddProperty("default_object", PropertyType::PT_OPTION, &CMetaObjectDocument::GetFormObject);
	categoryForm->AddProperty("default_list", PropertyType::PT_OPTION, &CMetaObjectDocument::GetFormList);
	categoryForm->AddProperty("default_select", PropertyType::PT_OPTION, &CMetaObjectDocument::GetFormSelect);

	m_category->AddCategory(categoryForm);

	PropertyContainer* categoryData = IObjectBase::CreatePropertyContainer("Data");

	categoryData->AddProperty("register_records", PropertyType::PT_RECORD_SELECT);

	m_category->AddCategory(categoryData);

	//create default attributes
	m_attributeNumber = CMetaDefaultAttributeObject::CreateString(wxT("number"), _("Number"), wxEmptyString, 11, true);
	m_attributeNumber->SetClsid(g_metaDefaultAttributeCLSID);

	//set child/parent
	m_attributeNumber->SetParent(this);
	AddChild(m_attributeNumber);

	m_attributeDate = CMetaDefaultAttributeObject::CreateDate(wxT("date"), _("Date"), wxEmptyString, eDateFractions::eDateFractions_DateTime, true);
	m_attributeDate->SetClsid(g_metaDefaultAttributeCLSID);

	//set child/parent
	m_attributeDate->SetParent(this);
	AddChild(m_attributeDate);

	m_attributePosted = CMetaDefaultAttributeObject::CreateBoolean(wxT("posted"), _("Posted"), wxEmptyString);
	m_attributePosted->SetClsid(g_metaDefaultAttributeCLSID);

	//set child/parent
	m_attributePosted->SetParent(this);
	AddChild(m_attributePosted);

	//create module
	m_moduleObject = new CMetaModuleObject(objectModule);
	m_moduleObject->SetClsid(g_metaModuleCLSID);

	//set child/parent
	m_moduleObject->SetParent(this);
	AddChild(m_moduleObject);

	//set default proc
	m_moduleObject->SetDefaultProcedure("beforeWrite", eContentHelper::eProcedureHelper, { "cancel", "writeMode", "postingMode" });
	m_moduleObject->SetDefaultProcedure("onWrite", eContentHelper::eProcedureHelper, { "cancel" });
	m_moduleObject->SetDefaultProcedure("beforeDelete", eContentHelper::eProcedureHelper, { "cancel" });
	m_moduleObject->SetDefaultProcedure("onDelete", eContentHelper::eProcedureHelper, { "cancel" });

	m_moduleObject->SetDefaultProcedure("posting", eContentHelper::eProcedureHelper, { "cancel", "postingMode" });
	m_moduleObject->SetDefaultProcedure("undoPosting", eContentHelper::eProcedureHelper, { "cancel" });

	m_moduleObject->SetDefaultProcedure("filling", eContentHelper::eProcedureHelper, { "source" });
	m_moduleObject->SetDefaultProcedure("onCopy", eContentHelper::eProcedureHelper, { "source" });

	m_moduleManager = new CMetaManagerModuleObject(managerModule);
	m_moduleManager->SetClsid(g_metaManagerCLSID);

	//set child/parent
	m_moduleManager->SetParent(this);
	AddChild(m_moduleManager);
}

CMetaObjectDocument::~CMetaObjectDocument()
{
	wxDELETE(m_attributeNumber);
	wxDELETE(m_attributeDate);
	wxDELETE(m_attributePosted);

	wxDELETE(m_moduleObject);
	wxDELETE(m_moduleManager);
}

CMetaFormObject* CMetaObjectDocument::GetDefaultFormByID(const form_identifier_t &id)
{
	if (id == eFormObject
		&& m_defaultFormObject != wxNOT_FOUND) {
		for (auto obj : GetObjectForms()) {
			if (m_defaultFormObject == obj->GetMetaID()) {
				return obj;
			}
		}
	}
	else if (id == eFormList
		&& m_defaultFormList != wxNOT_FOUND) {
		for (auto obj : GetObjectForms()) {
			if (m_defaultFormList == obj->GetMetaID()) {
				return obj;
			}
		}
	}
	else if (id == eFormSelect
		&& m_defaultFormSelect != wxNOT_FOUND) {
		for (auto obj : GetObjectForms()) {
			if (m_defaultFormSelect == obj->GetMetaID()) {
				return obj;
			}
		}
	}

	return NULL;
}

ISourceDataObject* CMetaObjectDocument::CreateObjectData(IMetaFormObject* metaObject)
{
	switch (metaObject->GetTypeForm())
	{
	case eFormObject: return CreateObjectRefValue(); break;
	case eFormList: return new CListDataObjectRef(this, metaObject->GetTypeForm()); break;
	case eFormSelect: return new CListDataObjectRef(this, metaObject->GetTypeForm(), true); break;
	}

	return NULL;
}

#include "appData.h"

IRecordDataObjectRef* CMetaObjectDocument::CreateObjectRefValue()
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	IRecordDataObjectRef* pDataRef = NULL;

	if (appData->DesignerMode()) {
		if (!moduleManager->FindCompileModule(m_moduleObject, pDataRef))
			return new CObjectDocument(this);
	}
	else {
		pDataRef = new CObjectDocument(this);
	}

	return pDataRef;
}

IRecordDataObjectRef* CMetaObjectDocument::CreateObjectRefValue(const Guid& guid)
{
	return new CObjectDocument(this, guid);
}

#include "frontend/visualView/controls/form.h"
#include "utils/stringUtils.h"

CValueForm* CMetaObjectDocument::GetObjectForm(const wxString& formName, IValueFrame* ownerControl, const CUniqueKey& formGuid)
{
	CMetaFormObject* defList = NULL;

	if (!formName.IsEmpty()) {
		for (auto metaForm : GetObjectForms()) {
			if (CMetaObjectDocument::eFormObject == metaForm->GetTypeForm()
				&& StringUtils::CompareString(formName, metaForm->GetName())) {
				defList = metaForm; break;
			}
		}
		wxASSERT(defList);
	}
	else {
		defList = GetDefaultFormByID(CMetaObjectDocument::eFormObject);
	}

	if (defList == NULL) {
		IRecordDataObject* objectData = CreateObjectRefValue();
		CValueForm* valueForm = new CValueForm;
		valueForm->InitializeForm(ownerControl, NULL,
			objectData, formGuid
		);
		valueForm->ReadProperty();
		valueForm->BuildForm(CMetaObjectDocument::eFormObject);
		return valueForm;
	}

	return defList->GenerateFormAndRun(
		ownerControl, CreateObjectRefValue(), formGuid
	);
}

CValueForm* CMetaObjectDocument::GetListForm(const wxString& formName, IValueFrame* ownerControl, const CUniqueKey& formGuid)
{
	CMetaFormObject* defList = NULL;

	if (!formName.IsEmpty()) {
		for (auto metaForm : GetObjectForms()) {
			if (CMetaObjectDocument::eFormList == metaForm->GetTypeForm()
				&& StringUtils::CompareString(formName, metaForm->GetName())) {
				defList = metaForm; break;
			}
		}
		wxASSERT(defList);
	}
	else {
		defList = GetDefaultFormByID(CMetaObjectDocument::eFormList);
	}

	if (defList == NULL) {
		CValueForm* valueForm = new CValueForm();
		valueForm->InitializeForm(ownerControl, NULL,
			new CListDataObjectRef(this, CMetaObjectDocument::eFormList), formGuid
		);
		valueForm->ReadProperty();
		valueForm->BuildForm(CMetaObjectDocument::eFormList);
		return valueForm;
	}

	return defList->GenerateFormAndRun(
		ownerControl, new CListDataObjectRef(this, defList->GetTypeForm()), formGuid
	);
}

CValueForm* CMetaObjectDocument::GetSelectForm(const wxString& formName, IValueFrame* ownerControl, const CUniqueKey& formGuid)
{
	CMetaFormObject* defList = NULL;

	if (!formName.IsEmpty()) {
		for (auto metaForm : GetObjectForms()) {
			if (CMetaObjectDocument::eFormSelect == metaForm->GetTypeForm()
				&& StringUtils::CompareString(formName, metaForm->GetName())) {
				defList = metaForm; break;
			}
		}
		wxASSERT(defList);
	}
	else {
		defList = GetDefaultFormByID(CMetaObjectDocument::eFormSelect);
	}

	if (defList == NULL) {
		CValueForm* valueForm = new CValueForm();
		valueForm->InitializeForm(ownerControl, NULL,
			new CListDataObjectRef(this, CMetaObjectDocument::eFormSelect, true), formGuid
		);
		valueForm->ReadProperty();
		valueForm->BuildForm(CMetaObjectDocument::eFormSelect);
		return valueForm;
	}

	return defList->GenerateFormAndRun(
		ownerControl, new CListDataObjectRef(this, defList->GetTypeForm()), formGuid
	);
}

OptionList* CMetaObjectDocument::GetFormObject(Property*)
{
	OptionList* optlist = new OptionList();
	optlist->AddOption(_("<not selected>"), wxNOT_FOUND);

	for (auto formObject : GetObjectForms()) {
		if (eFormObject == formObject->GetTypeForm()) {
			optlist->AddOption(formObject->GetName(), formObject->GetMetaID());
		}
	}

	return optlist;
}

OptionList* CMetaObjectDocument::GetFormList(Property*)
{
	OptionList* optlist = new OptionList();
	optlist->AddOption(_("<not selected>"), wxNOT_FOUND);

	for (auto formObject : GetObjectForms()) {
		if (eFormList == formObject->GetTypeForm()) {
			optlist->AddOption(formObject->GetName(), formObject->GetMetaID());
		}
	}

	return optlist;
}

OptionList* CMetaObjectDocument::GetFormSelect(Property*)
{
	OptionList* optlist = new OptionList();
	optlist->AddOption(_("<not selected>"), wxNOT_FOUND);

	for (auto formObject : GetObjectForms()) {
		if (eFormSelect == formObject->GetTypeForm()) {
			optlist->AddOption(formObject->GetName(), formObject->GetMetaID());
		}
	}

	return optlist;
}

wxString CMetaObjectDocument::GetDescription(const IObjectValueInfo* objValue) const
{
	CValue vDate = objValue->GetValueByMetaID(m_attributeDate->GetMetaID());
	CValue vNumber = objValue->GetValueByMetaID(m_attributeNumber->GetMetaID());

	wxString decr;
	decr << m_metaName << wxT(" ") << vNumber.GetString() << wxT(" ") << vDate.GetString();
	return decr;
}

std::vector<IMetaAttributeObject*> CMetaObjectDocument::GetDefaultAttributes() const
{
	std::vector<IMetaAttributeObject*> attributes;

	attributes.push_back(m_attributeNumber);
	attributes.push_back(m_attributeDate);
	attributes.push_back(m_attributePosted);
	attributes.push_back(m_attributeReference);

	return attributes;
}

std::vector<IMetaAttributeObject*> CMetaObjectDocument::GetSearchedAttributes() const
{
	std::vector<IMetaAttributeObject*> attributes;
	
	attributes.push_back(m_attributeNumber);
	attributes.push_back(m_attributeDate);
	
	return attributes;
}

//////////////////////////////////////////////////////////////////////////////

bool CMetaObjectDocument::LoadFromVariant(const wxVariant& variant)
{
	std::set<meta_identifier_t> prevRecords = m_recordData.GetData();
	
	if (m_recordData.LoadFromVariant(variant)) {

		std::set<meta_identifier_t> records = m_recordData.GetData();

		for (auto record : prevRecords) {

			auto foundedIt = records.find(record);
			if (foundedIt != records.end())
				continue;

			IMetaObjectRegisterData* registerData = wxDynamicCast(
				m_metaData->GetMetaObject(record), IMetaObjectRegisterData
			);

			if (registerData != NULL) {
				CMetaDefaultAttributeObject* infoRecorder = registerData->GetRegisterRecorder();
				infoRecorder->ClearMetatype(
					m_attributeReference->GetClsids()
				);
			}
		}
		
		for (auto record : records) {

			auto foundedIt = prevRecords.find(record);
			if (foundedIt != prevRecords.end())
				continue;
			
			IMetaObjectRegisterData *registerData = wxDynamicCast(
				m_metaData->GetMetaObject(record), IMetaObjectRegisterData
			);

			if (registerData != NULL) {		
				CMetaDefaultAttributeObject *infoRecorder = registerData->GetRegisterRecorder(); 
				infoRecorder->SetMetatype(
					m_attributeReference->GetClsids()
				);
			}
		}

		return true;
	}
	
	return false; 
}

void CMetaObjectDocument::SaveToVariant(wxVariant& variant, IMetadata* metaData) const
{
	m_recordData.SaveToVariant(variant, metaData);
}

//***************************************************************************
//*                       Save & load metadata                              *
//***************************************************************************

bool CMetaObjectDocument::LoadData(CMemoryReader& dataReader)
{
	//load default attributes:
	m_attributeNumber->LoadMeta(dataReader);
	m_attributeDate->LoadMeta(dataReader);
	m_attributePosted->LoadMeta(dataReader);

	//load object module
	m_moduleObject->LoadMeta(dataReader);
	m_moduleManager->LoadMeta(dataReader);

	m_defaultFormObject = dataReader.r_u32();
	m_defaultFormList = dataReader.r_u32();
	m_defaultFormSelect = dataReader.r_u32();

	if (!m_recordData.LoadData(dataReader))
		return false;

	return IMetaObjectRecordDataMutableRef::LoadData(dataReader);
}

bool CMetaObjectDocument::SaveData(CMemoryWriter& dataWritter)
{
	//save default attributes:
	m_attributeNumber->SaveMeta(dataWritter);
	m_attributeDate->SaveMeta(dataWritter);
	m_attributePosted->SaveMeta(dataWritter);

	//Save object module
	m_moduleObject->SaveMeta(dataWritter);
	m_moduleManager->SaveMeta(dataWritter);

	//save default form 
	dataWritter.w_u32(m_defaultFormObject);
	dataWritter.w_u32(m_defaultFormList);
	dataWritter.w_u32(m_defaultFormSelect);

	if (!m_recordData.SaveData(dataWritter))
		return false;

	//create or update table:
	return IMetaObjectRecordDataMutableRef::SaveData(dataWritter);
}

//***********************************************************************
//*                           read & save events                        *
//***********************************************************************

#include "appData.h"

bool CMetaObjectDocument::OnCreateMetaObject(IMetadata* metaData)
{
	if (!IMetaObjectRecordDataMutableRef::OnCreateMetaObject(metaData))
		return false;

	return m_attributeNumber->OnCreateMetaObject(metaData) &&
		m_attributeDate->OnCreateMetaObject(metaData) &&
		m_attributePosted->OnCreateMetaObject(metaData) &&
		m_moduleManager->OnCreateMetaObject(metaData) &&
		m_moduleObject->OnCreateMetaObject(metaData);
}

bool CMetaObjectDocument::OnLoadMetaObject(IMetadata* metaData)
{
	if (!m_attributeNumber->OnLoadMetaObject(metaData))
		return false;

	if (!m_attributeDate->OnLoadMetaObject(metaData))
		return false;

	if (!m_attributePosted->OnLoadMetaObject(metaData))
		return false;

	if (!m_moduleManager->OnLoadMetaObject(metaData))
		return false;

	if (!m_moduleObject->OnLoadMetaObject(metaData))
		return false;

	return IMetaObjectRecordDataMutableRef::OnLoadMetaObject(metaData);
}

bool CMetaObjectDocument::OnSaveMetaObject()
{
	if (!m_attributeNumber->OnSaveMetaObject())
		return false;

	if (!m_attributeDate->OnSaveMetaObject())
		return false;

	if (!m_attributePosted->OnSaveMetaObject())
		return false;

	if (!m_moduleManager->OnSaveMetaObject())
		return false;

	if (!m_moduleObject->OnSaveMetaObject())
		return false;

	return IMetaObjectRecordDataMutableRef::OnSaveMetaObject();
}

bool CMetaObjectDocument::OnDeleteMetaObject()
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (!m_attributeNumber->OnDeleteMetaObject())
		return false;

	if (!m_attributeDate->OnDeleteMetaObject())
		return false;

	if (!m_attributePosted->OnDeleteMetaObject())
		return false;

	if (!m_moduleManager->OnDeleteMetaObject())
		return false;

	if (!m_moduleObject->OnDeleteMetaObject())
		return false;

	return IMetaObjectRecordDataMutableRef::OnDeleteMetaObject();
}

bool CMetaObjectDocument::OnReloadMetaObject()
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (appData->DesignerMode()) {
		CObjectDocument* pDataRef = NULL;
		if (!moduleManager->FindCompileModule(m_moduleObject, pDataRef)) {
			return true;
		}

		if (pDataRef->InitializeObject()) {
			pDataRef->UpdateRecordSet();
			return true;
		}

		return false;
	}

	return true;
}

#include "metadata/singleMetaTypes.h"

bool CMetaObjectDocument::OnBeforeRunMetaObject(int flags)
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (!m_attributeNumber->OnBeforeRunMetaObject(flags))
		return false;

	if (!m_attributeDate->OnBeforeRunMetaObject(flags))
		return false;

	if (!m_attributePosted->OnBeforeRunMetaObject(flags))
		return false;

	if (!m_moduleManager->OnBeforeRunMetaObject(flags))
		return false;

	if (!m_moduleObject->OnBeforeRunMetaObject(flags))
		return false;

	registerSelection();

	return IMetaObjectRecordDataMutableRef::OnBeforeRunMetaObject(flags);
}

bool CMetaObjectDocument::OnAfterRunMetaObject(int flags)
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (appData->DesignerMode()) {

		if (IMetaObjectRecordDataMutableRef::OnAfterRunMetaObject(flags))
			return moduleManager->AddCompileModule(m_moduleObject, CreateObjectRefValue());

		return false;
	}

	return IMetaObjectRecordDataMutableRef::OnAfterRunMetaObject(flags);
}

bool CMetaObjectDocument::OnBeforeCloseMetaObject()
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (appData->DesignerMode()) {

		if (IMetaObjectRecordDataMutableRef::OnBeforeCloseMetaObject())
			return moduleManager->RemoveCompileModule(m_moduleObject);

		return false;
	}

	return IMetaObjectRecordDataMutableRef::OnBeforeCloseMetaObject();
}

bool CMetaObjectDocument::OnAfterCloseMetaObject()
{
	if (!m_attributePosted->OnAfterCloseMetaObject())
		return false;

	if (!m_attributePosted->OnAfterCloseMetaObject())
		return false;

	if (!m_attributePosted->OnAfterCloseMetaObject())
		return false;

	if (!m_moduleManager->OnAfterCloseMetaObject())
		return false;

	if (!m_moduleObject->OnAfterCloseMetaObject())
		return false;

	unregisterSelection();

	/////////////////////////////////////////////////////////////

	for (auto record : m_recordData.GetData()) {

		IMetaObjectRegisterData* registerData = wxDynamicCast(
			m_metaData->GetMetaObject(record), IMetaObjectRegisterData
		);

		if (registerData != NULL) {
			CMetaDefaultAttributeObject* infoRecorder = registerData->GetRegisterRecorder();
			infoRecorder->ClearMetatype(
				m_attributeReference->GetClsids()
			);
		}
	}

	/////////////////////////////////////////////////////////////

	return IMetaObjectRecordDataMutableRef::OnAfterCloseMetaObject();
}

//***********************************************************************
//*                             form events                             *
//***********************************************************************

void CMetaObjectDocument::OnCreateMetaForm(IMetaFormObject* metaForm)
{
	if (metaForm->GetTypeForm() == CMetaObjectDocument::eFormObject
		&& m_defaultFormObject == wxNOT_FOUND)
	{
		m_defaultFormObject = metaForm->GetMetaID();
	}
	else if (metaForm->GetTypeForm() == CMetaObjectDocument::eFormList
		&& m_defaultFormList == wxNOT_FOUND)
	{
		m_defaultFormList = metaForm->GetMetaID();
	}
	else if (metaForm->GetTypeForm() == CMetaObjectDocument::eFormSelect
		&& m_defaultFormSelect == wxNOT_FOUND)
	{
		m_defaultFormSelect = metaForm->GetMetaID();
	}
}

void CMetaObjectDocument::OnRemoveMetaForm(IMetaFormObject* metaForm)
{
	if (metaForm->GetTypeForm() == CMetaObjectDocument::eFormObject
		&& m_defaultFormObject == metaForm->GetMetaID())
	{
		m_defaultFormObject = wxNOT_FOUND;
	}
	else if (metaForm->GetTypeForm() == CMetaObjectDocument::eFormList
		&& m_defaultFormList == metaForm->GetMetaID())
	{
		m_defaultFormList = wxNOT_FOUND;
	}
	else if (metaForm->GetTypeForm() == CMetaObjectDocument::eFormSelect
		&& m_defaultFormSelect == metaForm->GetMetaID())
	{
		m_defaultFormSelect = wxNOT_FOUND;
	}
}

//***********************************************************************
//*                           read & save property                      *
//***********************************************************************

void CMetaObjectDocument::ReadProperty()
{
	IMetaObjectRecordDataMutableRef::ReadProperty();

	IObjectBase::SetPropertyValue("default_object", m_defaultFormObject);
	IObjectBase::SetPropertyValue("default_list", m_defaultFormList);
	IObjectBase::SetPropertyValue("default_select", m_defaultFormSelect);

	CMetaObjectDocument::SaveToVariant(
		GetPropertyAsVariant("register_records"), m_metaData
	);
}

void CMetaObjectDocument::SaveProperty()
{
	IMetaObjectRecordDataMutableRef::SaveProperty();

	IObjectBase::GetPropertyValue("default_object", m_defaultFormObject);
	IObjectBase::GetPropertyValue("default_list", m_defaultFormList);
	IObjectBase::GetPropertyValue("default_select", m_defaultFormSelect);

	CMetaObjectDocument::LoadFromVariant(
		GetPropertyAsVariant("register_records")
	);
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

METADATA_REGISTER(CMetaObjectDocument, "document", g_metaDocumentCLSID);

ENUM_REGISTER(CValueEnumDocumentWriteMode, "documentWriteMode", TEXT2CLSID("EN_WRMO"));
ENUM_REGISTER(CValueEnumDocumentPostingMode, "documentPostingMode", TEXT2CLSID("EN_POMO"));