#ifndef __ADVPROP_FONT_H__
#define __ADVPROP_FONT_H__

#include <wx/propgrid/propgrid.h>
#include "backend/fontcontainer.h"

#include "backend/backend_core.h"

// -----------------------------------------------------------------------
// wxPGFontProperty
// -----------------------------------------------------------------------

class BACKEND_API wxPGFontProperty : public wxPGProperty {
	WX_PG_DECLARE_PROPERTY_CLASS(wxPGFontProperty)
public:

	wxPGFontProperty(const wxString& label = wxPG_LABEL, const wxString& name = wxPG_LABEL, const wxFontContainer& value = *wxNORMAL_FONT);
	virtual ~wxPGFontProperty() override;

	wxVariant ChildChanged(wxVariant& thisValue, int childIndex,
		wxVariant& childValue) const override;

	void RefreshChildren() override;

	void OnSetValue() override;
	wxString GetValueAsString(int argFlags = 0) const override;

	bool OnEvent(wxPropertyGrid* propgrid, wxWindow* primary, wxEvent& event) override;
};

#endif