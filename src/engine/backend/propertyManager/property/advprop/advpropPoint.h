#ifndef __ADVPROP_POINT_H__
#define __ADVPROP_POINT_H__

#include <wx/propgrid/propgrid.h>
#include "backend/backend_core.h"

// -----------------------------------------------------------------------
// wxPGPointProperty
// -----------------------------------------------------------------------

class BACKEND_API wxPGPointProperty : public wxPGProperty {
	WX_PG_DECLARE_PROPERTY_CLASS(wxPGPointProperty)
public:
	wxPGPointProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wxPoint& value = wxPoint());
	virtual ~wxPGPointProperty() override;

	virtual wxVariant ChildChanged(wxVariant& thisValue, int childIndex,
		wxVariant& childValue) const override;

	void RefreshChildren() override;

protected:
	void DoSetValue(const wxPoint& value) { m_value = WXVARIANT(value); }
};

#endif