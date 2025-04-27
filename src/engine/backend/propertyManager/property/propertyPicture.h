#ifndef __PROPERTY_PICTURE_H__
#define __PROPERTY_PICTURE_H__

#include "backend/propertyManager/propertyObject.h"
#include "backend/propertyManager/property/advprop/advpropPicture.h"

//base property for "picture"
class BACKEND_API CPropertyPicture : public IProperty {
public:

	void SetValue(const wxBitmap& bmp);
	void SetValue(const wxString& bmp);
	wxBitmap GetValueAsBitmap() const;
	wxString GetValueAsString() const;

	CPropertyPicture(CPropertyCategory* cat, const wxString& name, const wxImage& img = wxNullImage)
		: IProperty(cat, name, wxT("Load From Art Provider;;"))
	{
	}

	CPropertyPicture(CPropertyCategory* cat, const wxString& name, const wxString& label, const wxImage& img = wxNullImage)
		: IProperty(cat, name, label, wxT("Load From Art Provider;;"))
	{
	}

	CPropertyPicture(CPropertyCategory* cat, const wxString& name, const wxString& label, const wxString& helpString, const wxImage& img = wxNullImage)
		: IProperty(cat, name, label, helpString, wxT("Load From Art Provider;;"))
	{
	}

	//get property for grid 
	virtual wxPGProperty* GetPGProperty() const {
		return new wxPGPictureProperty(m_propLabel, m_propName, m_propValue);
	}

	//set/Get property data
	virtual bool SetDataValue(const CValue& varPropVal);
	virtual bool GetDataValue(CValue& pvarPropVal) const;

	//load & save object in control 
	virtual bool LoadData(CMemoryReader& reader);
	virtual bool SaveData(CMemoryWriter& writer);
};

#endif