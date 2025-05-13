#ifndef __PROPERTY_TEMPLATE_H__
#define __PROPERTY_TEMPLATE_H__

#include "backend/propertyManager/propertyObject.h"
#include "backend/propertyManager/property/advprop/advpropHyperLink.h"

//base property for "template"
class BACKEND_API CPropertyTempalate : public IProperty {
	wxVariantData* CreateVariantData();
public:

	CPropertyTempalate(CPropertyCategory* cat, const wxString& name)
		: IProperty(cat, name, CreateVariantData())
	{
	}

	CPropertyTempalate(CPropertyCategory* cat, const wxString& name, const wxString& label)
		: IProperty(cat, name, label, CreateVariantData())
	{
	}

	CPropertyTempalate(CPropertyCategory* cat, const wxString& name, const wxString& label, const wxString& helpString)
		: IProperty(cat, name, label, helpString, CreateVariantData())
	{
	}

	//get property for grid 
	virtual wxPGProperty* GetPGProperty() const {
		return new wxPGHyperLinkProperty(m_owner, m_propLabel, m_propName, m_propValue);
	}

	// set/get property data
	virtual bool SetDataValue(const CValue& varPropVal);
	virtual bool GetDataValue(CValue& pvarPropVal) const;

	//load & save object in control 
	virtual bool LoadData(CMemoryReader& reader);
	virtual bool SaveData(CMemoryWriter& writer);
};

#endif