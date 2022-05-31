////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : dataProcessor - metadata
////////////////////////////////////////////////////////////////////////////

#include "dataProcessor.h"
#include "metadata/metadata.h"

#define objectModule wxT("objectModule")
#define managerModule wxT("managerModule")

wxIMPLEMENT_DYNAMIC_CLASS(CMetaObjectDataProcessor, IMetaObjectRecordData)
wxIMPLEMENT_DYNAMIC_CLASS(CMetaObjectDataProcessorExternal, CMetaObjectDataProcessor)

//********************************************************************************************
//*                                      metadata                                            *
//********************************************************************************************

CMetaObjectDataProcessor::CMetaObjectDataProcessor() : IMetaObjectRecordData(),
m_defaultFormObject(wxNOT_FOUND), m_objMode(METAOBJECT_NORMAL)
{
	PropertyContainer* categoryForm = IObjectBase::CreatePropertyContainer("DefaultForms");
	categoryForm->AddProperty("default_object", PropertyType::PT_OPTION, &CMetaObjectDataProcessor::GetFormObject);
	m_category->AddCategory(categoryForm);

	//create module
	m_moduleObject = new CMetaModuleObject(objectModule);
	m_moduleObject->SetClsid(g_metaModuleCLSID);

	//set child/parent
	m_moduleObject->SetParent(this);
	AddChild(m_moduleObject);

	m_moduleManager = new CMetaManagerModuleObject(managerModule);
	m_moduleManager->SetClsid(g_metaManagerCLSID);

	//set child/parent
	m_moduleManager->SetParent(this);
	AddChild(m_moduleManager);
}


CMetaObjectDataProcessor::CMetaObjectDataProcessor(int objMode) : IMetaObjectRecordData(),
m_defaultFormObject(wxNOT_FOUND), m_objMode(objMode)
{
	PropertyContainer* categoryForm = IObjectBase::CreatePropertyContainer("DefaultForms");
	categoryForm->AddProperty("default_object", PropertyType::PT_OPTION, &CMetaObjectDataProcessor::GetFormObject);
	m_category->AddCategory(categoryForm);

	//create module
	m_moduleObject = new CMetaModuleObject(objectModule);
	m_moduleObject->SetClsid(g_metaModuleCLSID);

	//set child/parent
	m_moduleObject->SetParent(this);
	AddChild(m_moduleObject);

	m_moduleManager = new CMetaManagerModuleObject(managerModule);
	m_moduleManager->SetClsid(g_metaManagerCLSID);

	//set child/parent
	m_moduleManager->SetParent(this);
	AddChild(m_moduleManager);
}

CMetaObjectDataProcessor::~CMetaObjectDataProcessor()
{
	wxDELETE(m_moduleObject);
	wxDELETE(m_moduleManager);
}

CMetaFormObject* CMetaObjectDataProcessor::GetDefaultFormByID(const form_identifier_t &id)
{
	if (id == eFormDataProcessor
		&& m_defaultFormObject != wxNOT_FOUND) {
		for (auto obj : GetObjectForms()) {
			if (m_defaultFormObject == obj->GetMetaID()) {
				return obj;
			}
		}
	}

	return NULL;
}

ISourceDataObject* CMetaObjectDataProcessor::CreateObjectData(IMetaFormObject* metaObject)
{
	switch (metaObject->GetTypeForm())
	{
	case eFormDataProcessor: return CreateObjectValue(); break;
	}

	return NULL;
}

#include "appData.h"

IRecordDataObject* CMetaObjectDataProcessor::CreateObjectValue()
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	IRecordDataObject* pDataRef = NULL;

	if (appData->DesignerMode()) {
		if (m_objMode == METAOBJECT_NORMAL) {
			if (!moduleManager->FindCompileModule(m_moduleObject, pDataRef))
				return new CObjectDataProcessor(this);
		}
		else {
			return moduleManager->GetObjectValue();
		}
	}
	else {
		if (m_objMode == METAOBJECT_NORMAL) {
			pDataRef = new CObjectDataProcessor(this);
		}
		else {
			return moduleManager->GetObjectValue();
		}
	}

	return pDataRef;
}

#include "frontend/visualView/controls/form.h"
#include "utils/stringUtils.h"

CValueForm* CMetaObjectDataProcessor::GetObjectForm(const wxString& formName, IValueFrame* ownerControl, const CUniqueKey& formGuid)
{
	CMetaFormObject* defList = NULL;

	if (!formName.IsEmpty()) {
		for (auto metaForm : GetObjectForms()) {
			if (CMetaObjectDataProcessor::eFormDataProcessor == metaForm->GetTypeForm()
				&& StringUtils::CompareString(formName, metaForm->GetName())) {
				return defList->GenerateFormAndRun(
					ownerControl, CreateObjectValue()
				);
			}
		}
		wxASSERT(defList);
	}
	else {
		defList = GetDefaultFormByID(CMetaObjectDataProcessor::eFormDataProcessor);
		if (defList) {
			return defList->GenerateFormAndRun(
				ownerControl, CreateObjectValue(), formGuid
			);
		}
	}

	if (defList == NULL) {
		IRecordDataObject* objectData = CreateObjectValue();
		CValueForm* valueForm = new CValueForm;
		valueForm->InitializeForm(ownerControl, NULL,
			objectData, formGuid
		);
		valueForm->ReadProperty();
		valueForm->BuildForm(CMetaObjectDataProcessor::eFormDataProcessor);
		return valueForm;
	}

	return defList->GenerateFormAndRun();
}

OptionList* CMetaObjectDataProcessor::GetFormObject(Property*)
{
	OptionList* optlist = new OptionList;
	optlist->AddOption(_("<not selected>"), wxNOT_FOUND);

	for (auto formObject : GetObjectForms()) {
		if (eFormDataProcessor == formObject->GetTypeForm()) {
			optlist->AddOption(formObject->GetName(), formObject->GetMetaID());
		}
	}

	return optlist;
}

//***************************************************************************
//*                       Save & load metadata                              *
//***************************************************************************

bool CMetaObjectDataProcessor::LoadData(CMemoryReader& dataReader)
{
	//Load object module
	m_moduleObject->LoadMeta(dataReader);
	m_moduleManager->LoadMeta(dataReader);

	//Load default form 
	m_defaultFormObject = dataReader.r_u32();

	return IMetaObjectRecordData::LoadData(dataReader);
}

bool CMetaObjectDataProcessor::SaveData(CMemoryWriter& dataWritter)
{
	//Save object module
	m_moduleObject->SaveMeta(dataWritter);
	m_moduleManager->SaveMeta(dataWritter);

	//Save default form 
	dataWritter.w_u32(m_defaultFormObject);

	return IMetaObjectRecordData::SaveData(dataWritter);
}

//***********************************************************************
//*                           read & save events                        *
//***********************************************************************

#include "appData.h"

bool CMetaObjectDataProcessor::OnCreateMetaObject(IMetadata* metaData)
{
	if (!IMetaObjectRecordData::OnCreateMetaObject(metaData))
		return false;

	return (m_objMode == METAOBJECT_NORMAL ? m_moduleManager->OnCreateMetaObject(metaData) : true) &&
		m_moduleObject->OnCreateMetaObject(metaData);
}

bool CMetaObjectDataProcessor::OnLoadMetaObject(IMetadata* metaData)
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (m_objMode == METAOBJECT_NORMAL) {

		if (!m_moduleManager->OnLoadMetaObject(metaData))
			return false;

		if (!m_moduleObject->OnLoadMetaObject(metaData))
			return false;
	}
	else {

		if (!m_moduleObject->OnLoadMetaObject(metaData))
			return false;
	}

	return IMetaObjectRecordData::OnLoadMetaObject(metaData);
}

bool CMetaObjectDataProcessor::OnSaveMetaObject()
{
	if (m_objMode == METAOBJECT_NORMAL) {
		if (!m_moduleManager->OnSaveMetaObject())
			return false;
	}

	if (!m_moduleObject->OnSaveMetaObject())
		return false;

	return IMetaObjectRecordData::OnSaveMetaObject();
}

bool CMetaObjectDataProcessor::OnDeleteMetaObject()
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (m_objMode == METAOBJECT_NORMAL) {
		if (!m_moduleManager->OnDeleteMetaObject())
			return false;
	}

	if (!m_moduleObject->OnDeleteMetaObject())
		return false;

	return IMetaObjectRecordData::OnDeleteMetaObject();
}

bool CMetaObjectDataProcessor::OnReloadMetaObject()
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (appData->DesignerMode()) {
		CObjectDataProcessor* pDataRef = NULL;
		if (!moduleManager->FindCompileModule(m_moduleObject, pDataRef)) {
			return true;
		}
		return pDataRef->InitializeObject();
	}

	return true;
}

bool CMetaObjectDataProcessor::OnBeforeRunMetaObject(int flags)
{
	if (m_objMode == METAOBJECT_NORMAL) {
		if (!m_moduleManager->OnBeforeRunMetaObject(flags)) {
			return false;
		}
	}

	if (!m_moduleObject->OnBeforeRunMetaObject(flags)) {
		return false;
	}

	return IMetaObjectRecordData::OnBeforeRunMetaObject(flags);
}

bool CMetaObjectDataProcessor::OnAfterRunMetaObject(int flags)
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (appData->DesignerMode()) {
		if (IMetaObjectRecordData::OnAfterRunMetaObject(flags))
			return moduleManager->AddCompileModule(m_moduleObject, CreateObjectValue());
		return false;
	}

	return IMetaObjectRecordData::OnAfterRunMetaObject(flags);
}

bool CMetaObjectDataProcessor::OnBeforeCloseMetaObject()
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (appData->DesignerMode()) {
		if (IMetaObjectRecordData::OnBeforeCloseMetaObject())
			return moduleManager->RemoveCompileModule(m_moduleObject);
		return false;
	}

	return IMetaObjectRecordData::OnBeforeCloseMetaObject();
}

bool CMetaObjectDataProcessor::OnAfterCloseMetaObject()
{
	if (m_objMode == METAOBJECT_NORMAL) {
		if (!m_moduleManager->OnAfterCloseMetaObject())
			return false;
	}

	if (!m_moduleObject->OnAfterCloseMetaObject()) {
		return false;
	}

	return IMetaObjectRecordData::OnAfterCloseMetaObject();
}

//***********************************************************************
//*                             form events                             *
//***********************************************************************

void CMetaObjectDataProcessor::OnCreateMetaForm(IMetaFormObject* metaForm)
{
	if (metaForm->GetTypeForm() == CMetaObjectDataProcessor::eFormDataProcessor
		&& m_defaultFormObject == wxNOT_FOUND)
	{
		m_defaultFormObject = metaForm->GetMetaID();
	}
}

void CMetaObjectDataProcessor::OnRemoveMetaForm(IMetaFormObject* metaForm)
{
	if (metaForm->GetTypeForm() == CMetaObjectDataProcessor::eFormDataProcessor
		&& m_defaultFormObject == metaForm->GetMetaID())
	{
		m_defaultFormObject = wxNOT_FOUND;
	}
}

//***********************************************************************
//*                           read & save property                      *
//***********************************************************************

void CMetaObjectDataProcessor::ReadProperty()
{
	IMetaObjectRecordData::ReadProperty();

	IObjectBase::SetPropertyValue("default_object", m_defaultFormObject);
}

void CMetaObjectDataProcessor::SaveProperty()
{
	IMetaObjectRecordData::SaveProperty();

	IObjectBase::GetPropertyValue("default_object", m_defaultFormObject);
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

METADATA_REGISTER(CMetaObjectDataProcessor, "dataProcessor", g_metaDataProcessorCLSID);
METADATA_REGISTER(CMetaObjectDataProcessorExternal, "externalDataProcessor", g_metaExternalDataProcessorCLSID);