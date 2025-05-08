#include "typeVariant.h"
#include "backend/metaData.h"

wxString wxVariantDataAttribute::MakeString() const
{
	wxString strDescr;
	if (m_ownerProperty != nullptr) {
		const IMetaData* metaData = m_ownerProperty->GetMetaData();
		wxASSERT(metaData);
		for (auto clsid : m_typeDesc.GetClsidList()) {
			if (metaData->IsRegisterCtor(clsid) && strDescr.IsEmpty()) {
				strDescr = metaData->GetNameObjectFromID(clsid);
			}
			else if (metaData->IsRegisterCtor(clsid)) {
				strDescr = strDescr +
					wxT(", ") + metaData->GetNameObjectFromID(clsid);
			}
		}
	}
	return strDescr;
}

#include "backend/system/value/valueTable.h"

void wxVariantDataAttribute::DoSetDefaultMetaType() {
	if (m_ownerProperty != nullptr) {
		if (eSelectorDataType::eSelectorDataType_boolean == m_ownerProperty->GetFilterDataType()) {
			m_typeDesc.SetDefaultMetaType(g_valueBooleanCLSID);
		}
		else if (eSelectorDataType::eSelectorDataType_reference == m_ownerProperty->GetFilterDataType()) {
			m_typeDesc.SetDefaultMetaType(g_valueStringCLSID);
		}
		else if (eSelectorDataType::eSelectorDataType_resource == m_ownerProperty->GetFilterDataType()) {
			m_typeDesc.SetDefaultMetaType(g_valueNumberCLSID);
		}
		else if (eSelectorDataType::eSelectorDataType_table == m_ownerProperty->GetFilterDataType()) {
			m_typeDesc.SetDefaultMetaType(g_valueTableCLSID);
		}
		else {
			m_typeDesc.SetDefaultMetaType(g_valueStringCLSID);
		}
	}
}

void wxVariantDataAttribute::DoSetFromMetaId(const meta_identifier_t& id)
{
	if (m_ownerProperty != nullptr && id != wxNOT_FOUND) {

		const IMetaData* metaData = m_ownerProperty->GetMetaData();
		wxASSERT(metaData);

		IMetaObjectAttribute* metaAttribute = nullptr;
		if (metaData->GetMetaObject(metaAttribute, id) && metaAttribute->IsAllowed()) {
			m_typeDesc.SetDefaultMetaType(metaAttribute->GetTypeDesc());
			return;
		}

		CMetaObjectTableData* metaTable = nullptr;
		if (metaData->GetMetaObject(metaTable, id) && metaTable->IsAllowed()) {
			m_typeDesc.SetDefaultMetaType(metaTable->GetTypeDesc());
			return;
		}

		SetDefaultMetaType();
	}
}

void wxVariantDataAttribute::DoSetFromTypeId(const CTypeDescription& td)
{
	m_typeDesc = td; 
	RefreshTypeDesc();
}

void wxVariantDataAttribute::DoRefreshTypeDesc()
{ 
	if (m_ownerProperty != nullptr) {
		const IMetaData* metaData = m_ownerProperty->GetMetaData();
		wxASSERT(metaData);
		std::set<class_identifier_t> clear_list;
		for (auto clsid : m_typeDesc.GetClsidList()) {
			if (!metaData->IsRegisterCtor(clsid)) clear_list.insert(clsid);
		}
		for (auto clsid : clear_list) { m_typeDesc.ClearMetaType(clsid); }
		if (!m_typeDesc.IsOk()) SetDefaultMetaType();
	}
}