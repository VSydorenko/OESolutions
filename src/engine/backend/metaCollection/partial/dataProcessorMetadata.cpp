////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : dataProcessor - metaData
////////////////////////////////////////////////////////////////////////////

#include "dataProcessor.h"
#include "backend/metaData.h"

wxIMPLEMENT_DYNAMIC_CLASS(CMetaObjectDataProcessor, IMetaObjectRecordDataExt)
wxIMPLEMENT_DYNAMIC_CLASS(CMetaObjectDataProcessorExternal, CMetaObjectDataProcessor)

//********************************************************************************************
//*                                      metaData                                            *
//********************************************************************************************

CMetaObjectDataProcessor::CMetaObjectDataProcessor(int objMode) : IMetaObjectRecordDataExt(objMode)
{
}

CMetaObjectDataProcessor::~CMetaObjectDataProcessor()
{
}

CMetaObjectForm* CMetaObjectDataProcessor::GetDefaultFormByID(const form_identifier_t& id)
{
	if (id == eFormDataProcessor
		&& m_propertyDefFormObject->GetValueAsInteger() != wxNOT_FOUND) {
		for (auto& obj : GetObjectForms()) {
			if (m_propertyDefFormObject->GetValueAsInteger() == obj->GetMetaID()) {
				return obj;
			}
		}
	}

	return nullptr;
}

ISourceDataObject* CMetaObjectDataProcessor::CreateObjectData(IMetaObjectForm* metaObject)
{
	switch (metaObject->GetTypeForm())
	{
	case eFormDataProcessor:
		return CreateObjectValue(); 
	}

	return nullptr;
}

#include "backend/appData.h"

IRecordDataObjectExt* CMetaObjectDataProcessor::CreateObjectExtValue()
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);
	CRecordDataObjectDataProcessor* pDataRef = nullptr;
	if (appData->DesignerMode()) {
		if (m_objMode == METAOBJECT_NORMAL) {
			if (!moduleManager->FindCompileModule(m_propertyModuleObject->GetMetaObject(), pDataRef))
				return m_metaData->CreateAndConvertObjectValueRef<CRecordDataObjectDataProcessor>(this);
		}
		else {
			return dynamic_cast<IRecordDataObjectExt*>(moduleManager->GetObjectValue());
		}
	}
	else {
		if (m_objMode == METAOBJECT_NORMAL) {
			pDataRef = m_metaData->CreateAndConvertObjectValueRef<CRecordDataObjectDataProcessor>(this);
		}
		else {
			return dynamic_cast<IRecordDataObjectExt*>(moduleManager->GetObjectValue());
		}
	}

	return pDataRef;
}

IBackendValueForm* CMetaObjectDataProcessor::GetObjectForm(const wxString& formName, IBackendControlFrame* ownerControl, const CUniqueKey& formGuid)
{
	CMetaObjectForm* defList = nullptr;

	if (!formName.IsEmpty()) {
		for (auto metaForm : GetObjectForms()) {
			if (CMetaObjectDataProcessor::eFormDataProcessor == metaForm->GetTypeForm()
				&& stringUtils::CompareString(formName, metaForm->GetName())) {
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

	if (defList == nullptr) {
		IRecordDataObject* objectData = CreateObjectValue();
		IBackendValueForm* valueForm = IBackendValueForm::CreateNewForm(ownerControl, nullptr,
			objectData, formGuid
		);
		valueForm->BuildForm(CMetaObjectDataProcessor::eFormDataProcessor);
		return valueForm;
	}

	return defList->GenerateFormAndRun();
}

bool CMetaObjectDataProcessor::GetFormObject(CPropertyList* prop)
{
	prop->AppendItem(_("<not selected>"), wxNOT_FOUND, wxEmptyValue);
	for (auto formObject : GetObjectForms()) {
		if (eFormDataProcessor == formObject->GetTypeForm()) {
			prop->AppendItem(formObject->GetName(), formObject->GetMetaID(), formObject);
		}
	}
	return true;
}

//***************************************************************************
//*                       Save & load metaData                              *
//***************************************************************************

bool CMetaObjectDataProcessor::LoadData(CMemoryReader& dataReader)
{
	//Load object module
	m_propertyModuleObject->GetMetaObject()->LoadMeta(dataReader);
	m_propertyModuleManager->GetMetaObject()->LoadMeta(dataReader);

	//Load default form 
	m_propertyDefFormObject->SetValue(GetIdByGuid(dataReader.r_stringZ()));

	return IMetaObjectRecordData::LoadData(dataReader);
}

bool CMetaObjectDataProcessor::SaveData(CMemoryWriter& dataWritter)
{
	//Save object module
	m_propertyModuleObject->GetMetaObject()->SaveMeta(dataWritter);
	m_propertyModuleManager->GetMetaObject()->SaveMeta(dataWritter);

	//Save default form 
	dataWritter.w_stringZ(GetGuidByID(m_propertyDefFormObject->GetValueAsInteger()));

	return IMetaObjectRecordData::SaveData(dataWritter);
}

//***********************************************************************
//*                           read & save events                        *
//***********************************************************************

#include "backend/appData.h"

bool CMetaObjectDataProcessor::OnCreateMetaObject(IMetaData* metaData, int flags)
{
	if (!IMetaObjectRecordData::OnCreateMetaObject(metaData, flags))
		return false;

	return (m_objMode == METAOBJECT_NORMAL ? m_propertyModuleManager->GetMetaObject()->OnCreateMetaObject(metaData, flags) : true) &&
		m_propertyModuleObject->GetMetaObject()->OnCreateMetaObject(metaData, flags);
}

bool CMetaObjectDataProcessor::OnLoadMetaObject(IMetaData* metaData)
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (m_objMode == METAOBJECT_NORMAL) {

		if (!m_propertyModuleManager->GetMetaObject()->OnLoadMetaObject(metaData))
			return false;

		if (!m_propertyModuleObject->GetMetaObject()->OnLoadMetaObject(metaData))
			return false;
	}
	else {

		if (!m_propertyModuleObject->GetMetaObject()->OnLoadMetaObject(metaData))
			return false;
	}

	return IMetaObjectRecordData::OnLoadMetaObject(metaData);
}

bool CMetaObjectDataProcessor::OnSaveMetaObject()
{
	if (m_objMode == METAOBJECT_NORMAL) {
		if (!m_propertyModuleManager->GetMetaObject()->OnSaveMetaObject())
			return false;
	}

	if (!m_propertyModuleObject->GetMetaObject()->OnSaveMetaObject())
		return false;

	return IMetaObjectRecordData::OnSaveMetaObject();
}

bool CMetaObjectDataProcessor::OnDeleteMetaObject()
{
	if (m_objMode == METAOBJECT_NORMAL) {
		if (!m_propertyModuleManager->GetMetaObject()->OnDeleteMetaObject())
			return false;
	}

	if (!m_propertyModuleObject->GetMetaObject()->OnDeleteMetaObject())
		return false;

	return IMetaObjectRecordData::OnDeleteMetaObject();
}

bool CMetaObjectDataProcessor::OnReloadMetaObject()
{
	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (appData->DesignerMode()) {
		CRecordDataObjectDataProcessor* pDataRef = nullptr;
		if (!moduleManager->FindCompileModule(m_propertyModuleObject->GetMetaObject(), pDataRef)) {
			return true;
		}
		return pDataRef->InitializeObject();
	}

	return true;
}

bool CMetaObjectDataProcessor::OnBeforeRunMetaObject(int flags)
{
	if (m_objMode == METAOBJECT_NORMAL) {
		if (!m_propertyModuleManager->GetMetaObject()->OnBeforeRunMetaObject(flags)) {
			return false;
		}
	}

	if (!m_propertyModuleObject->GetMetaObject()->OnBeforeRunMetaObject(flags)) {
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
			return moduleManager->AddCompileModule(m_propertyModuleObject->GetMetaObject(), CreateObjectValue());
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
			return moduleManager->RemoveCompileModule(m_propertyModuleObject->GetMetaObject());
		return false;
	}

	return IMetaObjectRecordData::OnBeforeCloseMetaObject();
}

bool CMetaObjectDataProcessor::OnAfterCloseMetaObject()
{
	if (m_objMode == METAOBJECT_NORMAL) {
		if (!m_propertyModuleManager->GetMetaObject()->OnAfterCloseMetaObject())
			return false;
	}

	if (!m_propertyModuleObject->GetMetaObject()->OnAfterCloseMetaObject()) {
		return false;
	}

	return IMetaObjectRecordData::OnAfterCloseMetaObject();
}

//***********************************************************************
//*                             form events                             *
//***********************************************************************

void CMetaObjectDataProcessor::OnCreateFormObject(IMetaObjectForm* metaForm)
{
	if (metaForm->GetTypeForm() == CMetaObjectDataProcessor::eFormDataProcessor
		&& m_propertyDefFormObject->GetValueAsInteger() == wxNOT_FOUND)
	{
		m_propertyDefFormObject->SetValue(metaForm->GetMetaID());
	}
}

void CMetaObjectDataProcessor::OnRemoveMetaForm(IMetaObjectForm* metaForm)
{
	if (metaForm->GetTypeForm() == CMetaObjectDataProcessor::eFormDataProcessor
		&& m_propertyDefFormObject->GetValueAsInteger() == metaForm->GetMetaID())
	{
		m_propertyDefFormObject->SetValue(metaForm->GetMetaID());
	}
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

METADATA_TYPE_REGISTER(CMetaObjectDataProcessor, "dataProcessor", g_metaDataProcessorCLSID);
METADATA_TYPE_REGISTER(CMetaObjectDataProcessorExternal, "externalDataProcessor", g_metaExternalDataProcessorCLSID);