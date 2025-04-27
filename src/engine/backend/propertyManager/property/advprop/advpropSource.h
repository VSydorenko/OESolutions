#ifndef __ADVPROP_SOURCE_H__
#define __ADVPROP_SOURCE_H__

#include "backend/propertyManager/property/advprop/advpropType.h"

class BACKEND_API IPropertyObject;

// -----------------------------------------------------------------------
// wxPGSourceDataProperty
// -----------------------------------------------------------------------

class BACKEND_API wxPGSourceDataProperty : public wxPGProperty {
	WX_PG_DECLARE_PROPERTY_CLASS(wxPGSourceDataProperty)
public:

	IPropertyObject* GetPropertyObject() const { 
		return m_typeSelector->GetPropertyObject(); 
	}

	wxPGSourceDataProperty(IPropertyObject* property = nullptr, const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wxVariant& value = wxNullVariant);

	virtual wxString ValueToString(wxVariant& value, int argFlags = 0) const override;
	virtual bool StringToValue(wxVariant& variant,
		const wxString& text,
		int argFlags = 0) const override;

	virtual wxVariant ChildChanged(wxVariant& thisValue,
		int childIndex,
		wxVariant& childValue) const override;

	virtual void RefreshChildren() override;
	virtual wxPGEditorDialogAdapter* GetEditorDialog() const override;

protected:

	wxPGTypeProperty* m_typeSelector;
};

#endif