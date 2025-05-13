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
	m_propertyModuleConfiguration->GetMetaObject()->SetDefaultProcedure("beforeStart", eContentHelper::eProcedureHelper, {"cancel"});
	m_propertyModuleConfiguration->GetMetaObject()->SetDefaultProcedure("onStart", eContentHelper::eProcedureHelper);
	m_propertyModuleConfiguration->GetMetaObject()->SetDefaultProcedure("beforeExit", eContentHelper::eProcedureHelper, { "cancel" });
	m_propertyModuleConfiguration->GetMetaObject()->SetDefaultProcedure("onExit", eContentHelper::eProcedureHelper);

	//set def metaid
	m_metaId = defaultMetaID;
}

CMetaObjectConfiguration::~CMetaObjectConfiguration()
{
}

bool CMetaObjectConfiguration::LoadData(CMemoryReader& dataReader)
{
	m_propertyVersion->SetValue(dataReader.r_s32());
	return m_propertyModuleConfiguration->GetMetaObject()->LoadMeta(dataReader);
}

bool CMetaObjectConfiguration::SaveData(CMemoryWriter& dataWritter)
{
	dataWritter.w_s32(m_propertyVersion->GetValueAsInteger());
	return m_propertyModuleConfiguration->GetMetaObject()->SaveMeta(dataWritter);
}

//***********************************************************************
//*                          common value object                        *
//***********************************************************************

#include "backend/metaData.h"

bool CMetaObjectConfiguration::OnCreateMetaObject(IMetaData* metaData, int flags)
{
	if (!m_propertyModuleConfiguration->GetMetaObject()->OnCreateMetaObject(metaData, flags)) {
		return false;
	}

	return IMetaObject::OnCreateMetaObject(metaData, flags);
}

bool CMetaObjectConfiguration::OnLoadMetaObject(IMetaData* metaData)
{
	if (!m_propertyModuleConfiguration->GetMetaObject()->OnLoadMetaObject(metaData)) {
		return false;
	}

	return IMetaObject::OnLoadMetaObject(metaData);
}

bool CMetaObjectConfiguration::OnSaveMetaObject()
{
	if (!m_propertyModuleConfiguration->GetMetaObject()->OnSaveMetaObject()) {
		return false;
	}

	return IMetaObject::OnSaveMetaObject();
}

bool CMetaObjectConfiguration::OnDeleteMetaObject()
{
	if (!m_propertyModuleConfiguration->GetMetaObject()->OnDeleteMetaObject()) {
		return false;
	}

	return IMetaObject::OnDeleteMetaObject();
}

bool CMetaObjectConfiguration::OnBeforeRunMetaObject(int flags)
{
	if (!m_propertyModuleConfiguration->GetMetaObject()->OnBeforeRunMetaObject(flags))
		return false;

	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (!moduleManager->AddCompileModule(m_propertyModuleConfiguration->GetMetaObject(), moduleManager))
		return false;

	return IMetaObject::OnBeforeRunMetaObject(flags);
}

bool CMetaObjectConfiguration::OnAfterCloseMetaObject()
{
	if (!m_propertyModuleConfiguration->GetMetaObject()->OnAfterCloseMetaObject())
		return false;

	IModuleManager* moduleManager = m_metaData->GetModuleManager();
	wxASSERT(moduleManager);

	if (!moduleManager->RemoveCompileModule(m_propertyModuleConfiguration->GetMetaObject()))
		return false;

	return IMetaObject::OnAfterCloseMetaObject();
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

METADATA_TYPE_REGISTER(CMetaObjectConfiguration, "commonMetadata", g_metaCommonMetadataCLSID);
