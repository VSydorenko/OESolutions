////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : metaobject common metaData
////////////////////////////////////////////////////////////////////////////

#include "metaObjectMetadata.h"
#include "metaModuleObject.h"

//*****************************************************************************************
//*                         metaData													  * 
//*****************************************************************************************

wxIMPLEMENT_DYNAMIC_CLASS(CMetaObjectConfiguration, IMetaObject);

//*****************************************************************************************
//*                                  MetadataObject                                       *
//*****************************************************************************************

CMetaObjectConfiguration::CMetaObjectConfiguration() : IMetaObject(configurationDefaultName)
{
	//set default proc
	m_commonModule->SetDefaultProcedure("beforeStart", eContentHelper::eProcedureHelper, { "cancel" });
	m_commonModule->SetDefaultProcedure("onStart", eContentHelper::eProcedureHelper);
	m_commonModule->SetDefaultProcedure("beforeExit", eContentHelper::eProcedureHelper, { "cancel" });
	m_commonModule->SetDefaultProcedure("onExit", eContentHelper::eProcedureHelper);

	//set def metaid
	m_metaId = defaultMetaID;
}

CMetaObjectConfiguration::~CMetaObjectConfiguration()
{
	wxDELETE(m_commonModule);
}

bool CMetaObjectConfiguration::LoadData(CMemoryReader& dataReader)
{
	m_propertyVersion->SetValue(dataReader.r_s32());
	return m_commonModule->LoadMeta(dataReader);
}

bool CMetaObjectConfiguration::SaveData(CMemoryWriter& dataWritter)
{
	dataWritter.w_s32(m_propertyVersion->GetValueAsInteger());
	return m_commonModule->SaveMeta(dataWritter);
}

//***********************************************************************
//*                          common value object                        *
//***********************************************************************

#include "backend/metaData.h"

bool CMetaObjectConfiguration::OnCreateMetaObject(IMetaData* metaData)
{
	if (!m_commonModule->OnCreateMetaObject(metaData)) {
		return false;
	}

	return IMetaObject::OnCreateMetaObject(metaData);
}

bool CMetaObjectConfiguration::OnLoadMetaObject(IMetaData* metaData)
{
	if (!m_commonModule->OnLoadMetaObject(metaData)) {
		return false;
	}

	return IMetaObject::OnLoadMetaObject(metaData);
}

bool CMetaObjectConfiguration::OnSaveMetaObject()
{
	if (!m_commonModule->OnSaveMetaObject()) {
		return false;
	}

	return IMetaObject::OnSaveMetaObject();
}

bool CMetaObjectConfiguration::OnDeleteMetaObject()
{
	if (!m_commonModule->OnDeleteMetaObject()) {
		return false;
	}

	return IMetaObject::OnDeleteMetaObject();
}

bool CMetaObjectConfiguration::OnBeforeRunMetaObject(int flags)
{
	if (!m_commonModule->OnBeforeRunMetaObject(flags))
		return false;

	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (!moduleManager->AddCompileModule(m_commonModule, moduleManager))
		return false;

	return IMetaObject::OnBeforeRunMetaObject(flags);
}

bool CMetaObjectConfiguration::OnAfterCloseMetaObject()
{
	if (!m_commonModule->OnAfterCloseMetaObject())
		return false;

	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (!moduleManager->RemoveCompileModule(m_commonModule))
		return false;

	return IMetaObject::OnAfterCloseMetaObject();
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

METADATA_TYPE_REGISTER(CMetaObjectConfiguration, "commonMetadata", g_metaCommonMetadataCLSID);
