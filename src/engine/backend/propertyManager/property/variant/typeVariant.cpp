#include "typeVariant.h"
#include "backend/metaData.h"

wxString wxVariantDataAttribute::MakeString() const
{
	wxString strDescr;
	if (m_ownerProperty != nullptr) {
		IMetaData* metaData = m_ownerProperty->GetMetaData();
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

void wxVariantDataAttribute::DoSetFromMetaId(const meta_identifier_t& id)
{
	if (m_ownerProperty != nullptr && id != wxNOT_FOUND) {
		IMetaData* metaData = m_ownerProperty->GetMetaData();
		wxASSERT(metaData);
		IMetaObjectAttribute* metaAttribute = dynamic_cast<IMetaObjectAttribute*>(metaData->GetMetaObject(id));
		if (metaAttribute != nullptr && metaAttribute->IsAllowed()) {
			m_typeDesc.SetDefaultMetaType(metaAttribute->GetTypeDesc());
			return;
		}
		CMetaObjectTableData* metaTable = dynamic_cast<CMetaObjectTableData*>(metaData->GetMetaObject(id));
		if (metaTable != nullptr && metaTable->IsAllowed()) {
			m_typeDesc.SetDefaultMetaType(metaTable->GetTypeDesc());
			return;
		}
		m_typeDesc.SetDefaultMetaType(eValueTypes::TYPE_STRING);
	}
}