#include "sourceVariant.h"
#include "backend/metaData.h"

////////////////////////////////////////////////////////////////////////////

void wxVariantDataAttributeSource::UpdateSourceAttribute()
{
    if (m_ownerProperty != nullptr) {
        const IMetaData* metaData = m_ownerProperty->GetMetaData();
        wxASSERT(metaData);
        std::set<class_identifier_t> clear_list;
        for (auto clsid : m_typeDesc.GetClsidList()) {
            if (!metaData->IsRegisterCtor(clsid)) clear_list.insert(clsid);
        }
        for (auto clsid : clear_list) {
            m_typeDesc.ClearMetaType(clsid);
        }
    }
}

void wxVariantDataAttributeSource::DoSetFromMetaId(const meta_identifier_t& id)
{
    if (m_ownerSrcProperty != nullptr && id != wxNOT_FOUND) {
        const ISourceObject* srcData = m_ownerSrcProperty->GetSourceObject();
        if (srcData != nullptr) {
            const IMetaObjectSourceData* metaObject = srcData->GetSourceMetaObject();
            if (metaObject != nullptr && metaObject->IsAllowed() && id == metaObject->GetMetaID()) {
                m_typeDesc.SetDefaultMetaType(srcData->GetSourceClassType());
                return;
            }
        }
    }

    wxVariantDataAttribute::DoSetFromMetaId(id);
}

////////////////////////////////////////////////////////////////////////////

wxString wxVariantDataSource::MakeString() const
{
    if (!m_dataSource.isValid()) return _("<not selected>");
    if (m_ownerProperty != nullptr) {
        const ISourceObject* sourceObject = m_ownerProperty->GetSourceObject();
        if (sourceObject != nullptr) {
            IMetaObjectSourceData* genericObject = sourceObject->GetSourceMetaObject();
            //wxASSERT(genericObject);
            IMetaObject* metaObject = genericObject ? genericObject->FindMetaObjectByID(m_dataSource) : nullptr;
            if (metaObject != nullptr && !metaObject->IsAllowed()) return _("<not selected>");
            else if (metaObject == nullptr) return _("<not selected>");
            return metaObject->GetName();
        }
    }
    return _("<not selected>");
}

////////////////////////////////////////////////////////////////////////////

meta_identifier_t wxVariantDataSource::GetIdByGuid(const Guid& guid) const
{
    const ISourceObject* sourceObject = m_ownerProperty->GetSourceObject();
    if (guid.isValid() && sourceObject != nullptr) {
        const IMetaObjectSourceData* genericObject = sourceObject->GetSourceMetaObject();
        //wxASSERT(genericObject);
        const IMetaObject* metaObject = genericObject ? genericObject->FindMetaObjectByID(guid) : nullptr;
        //wxASSERT(metaObject);
        return metaObject != nullptr && metaObject->IsAllowed() ? metaObject->GetMetaID() : wxNOT_FOUND;
    }
    return wxNOT_FOUND;
}

Guid wxVariantDataSource::GetGuidByID(const meta_identifier_t& id) const
{
    const ISourceObject* sourceObject = m_ownerProperty->GetSourceObject();
    if (id != wxNOT_FOUND && sourceObject != nullptr) {
        const IMetaObjectSourceData* genericObject = sourceObject->GetSourceMetaObject();
        //wxASSERT(objMetaValue);
        IMetaObject* metaObject = genericObject != nullptr && genericObject->IsAllowed() ?
            genericObject->FindMetaObjectByID(id) : nullptr;
        //wxASSERT(metaObject);
        return metaObject != nullptr && metaObject->IsAllowed() ? metaObject->GetGuid() : wxNullGuid;

    }
    return wxNullGuid;
}

////////////////////////////////////////////////////////////////////////////

IMetaObjectAttribute* wxVariantDataSource::GetSourceAttributeObject() const
{
    const ISourceObject* sourceObject = m_ownerProperty->GetSourceObject();
    if (m_dataSource.isValid() && sourceObject != nullptr) {
        const IMetaObjectSourceData* genericObject = sourceObject->GetSourceMetaObject();
        //wxASSERT(genericObject);
        return genericObject != nullptr && genericObject->IsAllowed() ?
            wxDynamicCast(genericObject->FindMetaObjectByID(m_dataSource), IMetaObjectAttribute) : nullptr;
    }

    return nullptr;
}

////////////////////////////////////////////////////////////////////////////

void wxVariantDataSource::SetSource(const meta_identifier_t& id, bool fillTypeDesc)
{
    const ISourceObject* sourceObject = m_ownerProperty->GetSourceObject();
    if (id != wxNOT_FOUND && sourceObject != nullptr) {
        const IMetaObjectSourceData* genericObject = sourceObject->GetSourceMetaObject();
        //wxASSERT(genericObject);
        IMetaObject* metaObject = genericObject->FindMetaObjectByID(id);
        wxASSERT(metaObject);
        m_dataSource = metaObject != nullptr && metaObject->IsAllowed() ? metaObject->GetGuid() : wxNullGuid;
    }
    else {
        m_dataSource.reset();
    }

    if (fillTypeDesc) m_attributeSource->SetFromMetaId(id);
}

meta_identifier_t wxVariantDataSource::GetSource() const
{
    return GetIdByGuid(m_dataSource);
}

////////////////////////////////////////////////////////////////////////////

void wxVariantDataSource::SetSourceGuid(const Guid& guid, bool fillTypeDesc)
{
    const meta_identifier_t& id = GetIdByGuid(guid);
    m_dataSource = guid;
    if (fillTypeDesc) m_attributeSource->SetFromMetaId(id);
}

Guid wxVariantDataSource::GetSourceGuid() const
{
    const ISourceObject* sourceObject = m_ownerProperty->GetSourceObject();
    if (m_dataSource.isValid() && sourceObject != nullptr) {
        const IMetaObjectSourceData* genericObject = sourceObject->GetSourceMetaObject();
        if (genericObject == nullptr) return wxNullGuid;
        IMetaObject* metaObject = genericObject ? genericObject->FindMetaObjectByID(m_dataSource) : nullptr;
        wxASSERT(metaObject);
        return metaObject != nullptr && metaObject->IsAllowed() ? m_dataSource : wxNullGuid;
    }
    return wxNullGuid;
}

////////////////////////////////////////////////////////////////////////////

void wxVariantDataSource::ResetSource()
{
    if (m_dataSource.isValid()) {
        wxASSERT(m_attributeSource->GetTypeDesc().GetClsidCount() > 0);
        m_dataSource.reset();
    }
}

////////////////////////////////////////////////////////////////////////////

bool wxVariantDataSource::IsPropAllowed() const
{
    const ISourceObject* sourceObject = m_ownerProperty->GetSourceObject();
    if (m_dataSource.isValid() && sourceObject != nullptr) {
        const IMetaObjectSourceData* genericObject = sourceObject->GetSourceMetaObject();
        if (genericObject == nullptr) return true;
        IMetaObject* metaObject = genericObject ? genericObject->FindMetaObjectByID(m_dataSource) : nullptr;
        wxASSERT(metaObject);
        if (metaObject != nullptr) 
            return !metaObject->IsAllowed();
        return true;
    }
    return true;
}
