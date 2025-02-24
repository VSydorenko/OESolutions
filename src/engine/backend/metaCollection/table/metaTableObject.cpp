////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : meta-tables
////////////////////////////////////////////////////////////////////////////

#include "metaTableObject.h"
#include "backend/metaData.h"

wxIMPLEMENT_DYNAMIC_CLASS(CMetaObjectTableData, IMetaObjectContextData)

//***********************************************************************
//*                         Attributes                                  * 
//***********************************************************************

#include "backend/objCtor.h"
#include "backend/metaCollection/partial/object.h"

typeDescription_t CMetaObjectTableData::GetTypeDescription() const
{
	IMetaValueTypeCtor* typeCtor =
		m_metaData->GetTypeCtor(this, eCtorMetaType::eCtorMetaType_TabularSection);
	wxASSERT(typeCtor);
	if (typeCtor != nullptr)
		return typeDescription_t(typeCtor->GetClassType());
	return typeDescription_t();
}

////////////////////////////////////////////////////////////////////////////

CMetaObjectTableData::CMetaObjectTableData() : IMetaObjectContextData()
{
	m_numberLine = IMetaObjectContextData::CreateNumber("numberLine", _("N"), wxEmptyString, 6, 0);
}

CMetaObjectTableData::~CMetaObjectTableData()
{
	wxDELETE(m_numberLine);
}

bool CMetaObjectTableData::LoadData(CMemoryReader& dataReader)
{
	m_propertyUse->SetValue(dataReader.r_u16());

	//load default attributes:
	m_numberLine->LoadMeta(dataReader);
	return true;
}

bool CMetaObjectTableData::SaveData(CMemoryWriter& dataWritter)
{
	dataWritter.w_u16(m_propertyUse->GetValueAsInteger());

	//save default attributes:
	m_numberLine->SaveMeta(dataWritter);
	return true;
}

//***********************************************************************
//*								Events								    *
//***********************************************************************

bool CMetaObjectTableData::OnCreateMetaObject(IMetaData* metaData)
{
	if (!IMetaObject::OnCreateMetaObject(metaData))
		return false;
	if (!m_numberLine->OnCreateMetaObject(metaData)) {
		return false;
	}
	return true;
}

bool CMetaObjectTableData::OnLoadMetaObject(IMetaData* metaData)
{
	if (!m_numberLine->OnLoadMetaObject(metaData))
		return false;

	return IMetaObject::OnLoadMetaObject(metaData);
}

bool CMetaObjectTableData::OnSaveMetaObject()
{
	if (!m_numberLine->OnDeleteMetaObject())
		return false;

	return IMetaObject::OnSaveMetaObject();
}

bool CMetaObjectTableData::OnDeleteMetaObject()
{
	if (!m_numberLine->OnDeleteMetaObject())
		return false;

	return IMetaObject::OnDeleteMetaObject();
}

bool CMetaObjectTableData::OnReloadMetaObject()
{
	IMetaObject* metaObject = GetParent();
	wxASSERT(metaObject);
	if (metaObject->OnReloadMetaObject())
		return IMetaObject::OnReloadMetaObject();
	return false;
}

bool CMetaObjectTableData::OnBeforeRunMetaObject(int flags)
{
	if (!m_numberLine->OnBeforeRunMetaObject(flags))
		return false;
	IMetaObjectRecordData* metaObject = wxDynamicCast(GetParent(), IMetaObjectRecordData);
	wxASSERT(metaObject);
	if (metaObject != nullptr) {
		registerTabularSection();
		registerTabularSection_String();
	}

	return IMetaObject::OnBeforeRunMetaObject(flags);
}

bool CMetaObjectTableData::OnAfterRunMetaObject(int flags)
{
	if ((flags & newObjectFlag) != 0) OnReloadMetaObject();
	return IMetaObject::OnAfterRunMetaObject(flags);
}

bool CMetaObjectTableData::OnAfterCloseMetaObject()
{
	if (!m_numberLine->OnAfterCloseMetaObject())
		return false;
	IMetaObjectRecordData* metaObject = wxDynamicCast(GetParent(), IMetaObjectRecordData);
	wxASSERT(metaObject);
	if (metaObject != nullptr) {
		unregisterTabularSection();
		unregisterTabularSection_String();
	}
	return IMetaObject::OnAfterCloseMetaObject();
}

//***********************************************************************
//*                           System metaData                           *
//***********************************************************************

std::vector<IMetaObjectAttribute*> CMetaObjectTableData::GetObjectAttributes() const
{
	std::vector<IMetaObjectAttribute*> tableAttributes;
	tableAttributes.push_back(m_numberLine);
	for (auto metaObject : m_listMetaObject) {
		if (metaObject->GetClassType() == g_metaAttributeCLSID) {
			tableAttributes.push_back(
				dynamic_cast<IMetaObjectAttribute*>(metaObject)
			);
		}
	}
	return tableAttributes;
}

IMetaObjectAttribute* CMetaObjectTableData::FindProp(const meta_identifier_t& id) const
{
	for (auto metaObject : m_listMetaObject) {
		if (metaObject->GetClassType() == g_metaAttributeCLSID && metaObject->GetMetaID() == id) {
			return dynamic_cast<IMetaObjectAttribute*>(metaObject);
		}
	}

	if (m_numberLine->GetMetaID() == id) {
		return m_numberLine;
	}

	return nullptr;
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

METADATA_TYPE_REGISTER(CMetaObjectTableData, "tabularSection", g_metaTableCLSID);