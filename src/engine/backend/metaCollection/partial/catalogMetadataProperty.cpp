#include "catalog.h"
#include "backend/metadata.h"
#include "backend/objCtor.h"

void CMetaObjectCatalog::OnPropertyCreated(IProperty* property)
{
	IMetaObjectRecordDataMutableRef::OnPropertyCreated(property);
}

bool CMetaObjectCatalog::OnPropertyChanging(IProperty* property, const wxVariant& newValue)
{
	return IMetaObjectRecordDataMutableRef::OnPropertyChanging(property, newValue);
}

void CMetaObjectCatalog::OnPropertyChanged(IProperty* property, const wxVariant& oldValue, const wxVariant& newValue)
{
	if (m_propertyOwner == property) {
		const CMetaDescription& metaDesc = m_propertyOwner->GetValueAsMetaDesc(); CTypeDescription typeDesc;
		for (unsigned int idx = 0; idx < metaDesc.GetTypeCount(); idx++) {
			const IMetaObject* catalog = m_metaData->GetMetaObject(metaDesc.GetByIdx(idx));
			if (catalog != nullptr) {
				IMetaValueTypeCtor* so = m_metaData->GetTypeCtor(catalog, eCtorMetaType::eCtorMetaType_Reference);
				wxASSERT(so);
				typeDesc.AppendMetaType(so->GetClassType());
			}
		}
		m_attributeOwner->SetDefaultMetaType(typeDesc);
	}
	if (m_attributeOwner->GetClsidCount() > 0) m_attributeOwner->ClearFlag(metaDisableFlag);
	else m_attributeOwner->SetFlag(metaDisableFlag);
	IMetaObjectRecordDataMutableRef::OnPropertyChanged(property, oldValue, newValue);
}