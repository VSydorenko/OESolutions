#ifndef __ADVPROP_RECORD_H__
#define __ADVPROP_RECORD_H__

#include <wx/propgrid/propgrid.h>
#include "backend/backend_type.h"

class BACKEND_API IPropertyObject;

// -----------------------------------------------------------------------
// wxPGRecordProperty
// -----------------------------------------------------------------------

class BACKEND_API wxPGRecordProperty : public wxPGProperty {
	WX_PG_DECLARE_PROPERTY_CLASS(wxPGRecordProperty);
private:
	void FillByClsid(const class_identifier_t& clsid);
public:

	IPropertyObject* GetPropertyObject() const { return m_ownerProperty; }

	wxPGRecordProperty(IPropertyObject* property = nullptr, const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL, const wxVariant& value = wxNullVariant);

	virtual wxString ValueToString(wxVariant& value, int argFlags = 0) const override;
	virtual bool StringToValue(wxVariant& variant,
		const wxString& text,
		int argFlags = 0) const override;

	virtual bool IntToValue(wxVariant& value,
		int number,
		int argFlags = 0) const override;

	virtual wxPGEditorDialogAdapter* GetEditorDialog() const override;

protected:
	IPropertyObject* m_ownerProperty = nullptr;
};

#endif