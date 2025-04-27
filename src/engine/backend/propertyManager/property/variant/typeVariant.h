#ifndef __TYPE_VARIANT_H__
#define __TYPE_VARIANT_H__

#include "backend/metaCollection/metaObject.h"
#include "backend/backend_type.h"

class BACKEND_API wxVariantDataAttribute : public wxVariantData {
	wxString MakeString() const;
protected:
	virtual void DoSetFromMetaId(const meta_identifier_t& id);
public:

	eSelectorDataType GetFilterDataType() const {
		return m_ownerProperty->GetFilterDataType();
	}

	CTypeDescription& GetTypeDesc() { return m_typeDesc; }
	const CTypeDescription& GetTypeDesc() const { return m_typeDesc; }

	/////////////////////////////////////////////////////////////////////////////////////////
	void SetFromMetaId(const meta_identifier_t& id) { DoSetFromMetaId(id); }
	void SetFromTypeId(const CTypeDescription& typeDesc) { m_typeDesc = typeDesc; }
	/////////////////////////////////////////////////////////////////////////////////////////

	wxVariantDataAttribute(const IBackendTypeConfigFactory* prop) : wxVariantData(), m_ownerProperty(prop) {}

	wxVariantDataAttribute(const IBackendTypeConfigFactory* prop, const eValueTypes type) : wxVariantData(), m_ownerProperty(prop) { m_typeDesc.SetDefaultMetaType(type); }
	wxVariantDataAttribute(const IBackendTypeConfigFactory* prop, const class_identifier_t& clsid) : wxVariantData(), m_ownerProperty(prop) { m_typeDesc.SetDefaultMetaType(clsid); }

	wxVariantDataAttribute(const IBackendTypeConfigFactory* prop, const meta_identifier_t& id) : wxVariantData(), m_ownerProperty(prop) { DoSetFromMetaId(id); }
	wxVariantDataAttribute(const IBackendTypeConfigFactory* prop, const CTypeDescription& typeDesc) : wxVariantData(), m_ownerProperty(prop), m_typeDesc(typeDesc) {}

	wxVariantDataAttribute(const wxVariantDataAttribute& list) : wxVariantData(), m_ownerProperty(list.m_ownerProperty), m_typeDesc(list.m_typeDesc) {}

	virtual wxVariantDataAttribute* Clone() const {
		return new wxVariantDataAttribute(*this);
	}

	virtual bool Eq(wxVariantData& data) const {
		wxVariantDataAttribute* srcAttr = dynamic_cast<wxVariantDataAttribute*>(&data);
		if (srcAttr != nullptr)
			return srcAttr->m_typeDesc == m_typeDesc;
		return false;
	}

#if wxUSE_STD_IOSTREAM
	virtual bool Write(wxSTD ostream& str) const {
		str << MakeString();
		return true;
	}
#endif
	virtual bool Write(wxString& str) const {
		str = MakeString();
		return true;
	}

	virtual wxString GetType() const {
		return wxT("wxVariantDataAttribute");
	}

protected:

	const IBackendTypeConfigFactory* m_ownerProperty = nullptr;

	CTypeDescription m_typeDesc;
};

#endif // !__TYPE_VARIANT_H__
