#ifndef _ATTRIBUTE_CONTROL_H__
#define _ATTRIBUTE_CONTROL_H__

#include "backend/compiler/value/value.h"

class BACKEND_API IMetaData;

class BACKEND_API IMetaObject;
class BACKEND_API IMetaObjectGenericData;

class ISourceDataObject;

#include "backend/srcObject.h"
#include "backend/wrapper/typeInfo.h"

class CValueForm;

///////////////////////////////////////////////////////////////////////////

class ITypeControlAttribute :
	public ITypeAttribute {
protected:
	virtual void DoSetFromMetaId(const meta_identifier_t& id);
public:

	enum eSelectMode GetSelectMode() const;

	virtual class_identifier_t GetFirstClsid() const;
	virtual std::set<class_identifier_t> GetClsids() const;

	IMetaObject* GetMetaSource() const;
	IMetaObject* GetMetaObjectById(const class_identifier_t& clsid) const;

	void SetSourceId(const meta_identifier_t& id);
	meta_identifier_t GetSourceId() const;

	void ResetSource();

	//ctor
	ITypeControlAttribute(const eValueTypes& defType = eValueTypes::TYPE_STRING) :
		ITypeAttribute(defType) {
	}

	ITypeControlAttribute(const class_identifier_t& clsid) :
		ITypeAttribute(clsid) {
	}

	ITypeControlAttribute(const std::set<class_identifier_t>& clsids) :
		ITypeAttribute(clsids) {
	}

	ITypeControlAttribute(const std::set<class_identifier_t>& clsids, const typeDescription_t::typeData_t& descr) :
		ITypeAttribute(clsids, descr) {
	}

	//////////////////////////////////////////////////

	meta_identifier_t GetIdByGuid(const Guid& guid) const;
	Guid GetGuidByID(const meta_identifier_t& id) const;

	//////////////////////////////////////////////////

	virtual bool LoadTypeData(CMemoryReader& dataReader);
	virtual bool LoadFromVariant(const wxVariant& variant);
	virtual bool SaveTypeData(CMemoryWriter& dataWritter);
	virtual void SaveToVariant(wxVariant& variant, IMetaData* metadata) const;

	//////////////////////////////////////////////////

	static bool SimpleChoice(class IControlFrame* ownerValue, const class_identifier_t& clsid, wxWindow* parent);

	static bool QuickChoice(class IControlFrame* ownerValue, const class_identifier_t& clsid, wxWindow* parent);
	static void QuickChoice(class IControlFrame* controlValue, CValue& newValue, wxWindow* parent, const wxString& strData);

	static class_identifier_t ShowSelectType(IMetaData* metadata, const typeDescription_t& typeDescription);

	//////////////////////////////////////////////////

	//Get source object 
	virtual ISourceObject* GetSourceObject() const = 0;

	//Get source object 
	virtual CValueForm* GetOwnerForm() const = 0;

	//Get meta object
	virtual IMetaObjectGenericData* GetMetaObject() const;

	//Create value by selected type
	virtual CValue CreateValue() const;
	virtual CValue* CreateValueRef() const;

	//Get data type 
	virtual class_identifier_t GetDataType() const;

protected:

	Guid m_dataSource;
};

///////////////////////////////////////////////////////////////////////////

class wxVariantSourceAttributeData : public wxVariantAttributeData {
	void UpdateSourceAttribute();
public:
	wxVariantSourceAttributeData(IMetaData* metaData, CValueForm* formData, const meta_identifier_t& id = wxNOT_FOUND)
		: wxVariantAttributeData(metaData), m_formCollectionData(formData)
	{
		DoSetFromMetaId(id);
	}

	wxVariantSourceAttributeData(const wxVariantSourceAttributeData& srcData)
		: wxVariantAttributeData(srcData), m_formCollectionData(srcData.m_formCollectionData)
	{
		UpdateSourceAttribute();
	}

protected:
	virtual void DoSetFromMetaId(const meta_identifier_t& id);
protected:
	CValueForm* m_formCollectionData;
};

///////////////////////////////////////////////////////////////////////////

class wxVariantSourceData :
	public wxVariantData {
	wxString MakeString() const;
public:

	wxVariantSourceAttributeData* GetAttributeData() const {
		return m_typeDescription;
	}

	void SetSourceId(const meta_identifier_t& id) {
		m_srcId = id;
	}

	meta_identifier_t GetSourceId() const {
		return m_srcId;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void RefreshData() {
		if (m_typeDescription != nullptr) {
			m_typeDescription->RefreshData(m_srcId);
		}
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	wxVariantSourceData(IMetaData* metadata, CValueForm* formData, const meta_identifier_t& id = wxNOT_FOUND) : wxVariantData(),
		m_typeDescription(new wxVariantSourceAttributeData(metadata, formData, id)), m_srcId(id), m_formCollectionData(formData), m_metaData(metadata) {
	}

	wxVariantSourceData(const wxVariantSourceData& srcData) : wxVariantData(),
		m_typeDescription(new wxVariantSourceAttributeData(*srcData.m_typeDescription)), m_srcId(srcData.m_srcId), m_formCollectionData(srcData.m_formCollectionData), m_metaData(srcData.m_metaData) {
	}

	virtual ~wxVariantSourceData() {
		wxDELETE(m_typeDescription);
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	virtual bool Eq(wxVariantData& data) const {
		wxVariantSourceData* srcData = dynamic_cast<wxVariantSourceData*>(&data);
		if (srcData != nullptr) {
			wxVariantAttributeData* srcAttr = srcData->GetAttributeData();
			return m_srcId == srcData->GetSourceId()
				&& srcAttr->Eq(*m_typeDescription);
		}
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
		return wxT("wxVariantSourceData");
	}

	CValueForm* GetOwnerForm() const {
		return m_formCollectionData;
	}

	IMetaData* GetMetaData() const {
		return m_metaData;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

protected:

	IMetaData* m_metaData;

	CValueForm* m_formCollectionData;
	wxVariantSourceAttributeData* m_typeDescription;

	meta_identifier_t m_srcId;
};

#endif