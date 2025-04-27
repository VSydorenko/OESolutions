#ifndef __PROPERTY_BOOLEAN_H__
#define __PROPERTY_BOOLEAN_H__

#include "backend/propertyManager/propertyObject.h"

//base property for "bool"
class BACKEND_API CPropertyBoolean : public IProperty {
public:

	bool GetValueAsBoolean() const { return typeConv::StringToBool(m_propValue); }
	void SetValue(const bool boolean) { m_propValue = typeConv::BoolToString(boolean); }

	CPropertyBoolean(CPropertyCategory* cat, const wxString& name,
		const bool& value = false) : IProperty(cat, name, typeConv::BoolToString(value))
	{
	}

	CPropertyBoolean(CPropertyCategory* cat, const wxString& name, const wxString& label,
		const bool& value = false) : IProperty(cat, name, label, typeConv::BoolToString(value))
	{
	}

	CPropertyBoolean(CPropertyCategory* cat, const wxString& name, const wxString& label, const wxString& helpString,
		const bool& value = false) : IProperty(cat, name, label, helpString, typeConv::BoolToString(value))
	{
	}

	//get property for grid 
	virtual wxPGProperty* GetPGProperty() const {
		return new wxBoolProperty(m_propLabel, m_propName, GetValueAsBoolean());
	}

	// set/get property data
	virtual bool SetDataValue(const CValue& varPropVal);
	virtual bool GetDataValue(CValue& pvarPropVal) const;

	//load & save object in control 
	virtual bool LoadData(CMemoryReader& reader);
	virtual bool SaveData(CMemoryWriter& writer);
};

#endif