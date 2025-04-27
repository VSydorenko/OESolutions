#ifndef __ADVPROP_STRING_H__
#define __ADVPROP_STRING_H__

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

#include "backend/backend_core.h"

// -----------------------------------------------------------------------
// wxGeneralStringProperty
// -----------------------------------------------------------------------

class BACKEND_API wxGeneralStringProperty : public wxStringProperty {
	WX_PG_DECLARE_PROPERTY_CLASS(wxGeneralStringProperty)
public:

	wxGeneralStringProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wxString& value = wxEmptyString) : wxStringProperty(label, name, value) {
	}

	virtual wxString ValueToString(wxVariant& value, int argFlags = 0) const override;
	virtual bool StringToValue(wxVariant& variant,
		const wxString& text,
		int argFlags = 0) const override;
};

#endif