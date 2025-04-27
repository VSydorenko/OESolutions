#ifndef __PROPERTY_NUMBER_H__
#define __PROPERTY_NUMBER_H__

#include "backend/propertyManager/propertyObject.h"
#include "backend/propertyManager/property/advprop/advpropNumber.h"

class BACKEND_API IPropertyNumber : public IProperty {
public:

	int GetValueAsInteger() const { return GetValueAsNumber().ToInt(); }
	unsigned int GetValueAsUInteger() const { return GetValueAsNumber().ToUInt(); }
	number_t GetValueAsNumber() const { return typeConv::StringToNumber(m_propValue); }

	void SetValue(const signed int integer) { m_propValue = stringUtils::IntToStr(integer); }
	void SetValue(const unsigned int integer) { m_propValue = stringUtils::UIntToStr(integer); }
	void SetValue(const number_t& val) { m_propValue = typeConv::NumberToString(val); }

	IPropertyNumber(CPropertyCategory* cat, const wxString& name,
		const number_t& value) : IProperty(cat, name, typeConv::NumberToString(value))
	{
	}

	IPropertyNumber(CPropertyCategory* cat, const wxString& name, const wxString& label,
		const number_t& value) : IProperty(cat, name, label, typeConv::NumberToString(value))
	{
	}

	IPropertyNumber(CPropertyCategory* cat, const wxString& name, const wxString& label, const wxString& helpString,
		const number_t& value) : IProperty(cat, name, label, helpString, typeConv::NumberToString(value))
	{
	}

	virtual bool IsEmptyProperty() const { return GetValueAsNumber().IsZero(); }

	// set/get property data
	virtual bool SetDataValue(const CValue& varPropVal);
	virtual bool GetDataValue(CValue& pvarPropVal) const;

	//load & save object in control 
	virtual bool LoadData(CMemoryReader& reader);
	virtual bool SaveData(CMemoryWriter& writer);
};

//base property for "number"
class BACKEND_API CPropertyNumber : public IPropertyNumber {
public:

	CPropertyNumber(CPropertyCategory* cat, const wxString& name,
		const number_t& value = 0) : IPropertyNumber(cat, name, value)
	{
	}

	CPropertyNumber(CPropertyCategory* cat, const wxString& name, const wxString& label,
		const number_t& value = 0) : IPropertyNumber(cat, name, label, value)
	{
	}

	CPropertyNumber(CPropertyCategory* cat, const wxString& name, const wxString& label, const wxString& helpString,
		const number_t& value = 0) : IPropertyNumber(cat, name, label, helpString, value)
	{
	}

	//get property for grid 
	virtual wxPGProperty* GetPGProperty() const {
		return new wxNumberProperty(m_propLabel, m_propName, GetValueAsNumber());
	};
};

class CPropertyInteger : public IPropertyNumber {
public:

	CPropertyInteger(CPropertyCategory* cat, const wxString& name,
		const int& value = 0) : IPropertyNumber(cat, name, value)
	{
	}

	CPropertyInteger(CPropertyCategory* cat, const wxString& name, const wxString& label,
		const int& value = 0) : IPropertyNumber(cat, name, label, value)
	{
	}

	CPropertyInteger(CPropertyCategory* cat, const wxString& name, const wxString& label, const wxString& helpString,
		const int& value = 0) : IPropertyNumber(cat, name, label, helpString, value)
	{
	}

	//get property for grid 
	virtual wxPGProperty* GetPGProperty() const {
		return new wxIntProperty(m_propLabel, m_propName, GetValueAsInteger());
	};
};

class CPropertyUInteger : public IPropertyNumber {
public:

	CPropertyUInteger(CPropertyCategory* cat, const wxString& name,
		const unsigned int& value = 0) : IPropertyNumber(cat, name, value)
	{
	}

	CPropertyUInteger(CPropertyCategory* cat, const wxString& name, const wxString& label,
		const unsigned int& value = 0) : IPropertyNumber(cat, name, label, value)
	{
	}

	CPropertyUInteger(CPropertyCategory* cat, const wxString& name, const wxString& label, const wxString& helpString,
		const unsigned int& value = 0) : IPropertyNumber(cat, name, label, helpString, value)
	{
	}

	//get property for grid 
	virtual wxPGProperty* GetPGProperty() const {
		return new wxUIntProperty(m_propLabel, m_propName, GetValueAsUInteger());
	};
};

#endif