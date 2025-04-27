#ifndef __ADVPROP_PICTURE_H__
#define __ADVPROP_PICTURE_H__

#include <wx/propgrid/propgrid.h>
#include "backend/backend_core.h"

// -----------------------------------------------------------------------
// wxPGPictureProperty
// -----------------------------------------------------------------------

class BACKEND_API wxPGPictureProperty : public wxPGProperty {
	WX_PG_DECLARE_PROPERTY_CLASS(wxPGPictureProperty)
public:
	wxPGPictureProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL,
		const wxString& value = wxString());

	virtual ~wxPGPictureProperty() {}

	virtual wxString ValueToString(wxVariant& value, int argFlags = 0) const override { return value.GetString(); }
	virtual bool StringToValue(wxVariant& variant,
		const wxString& text,
		int argFlags = 0) const override {
		return false;
	}

	wxPGProperty* CreatePropertySource(int sourceIndex = 0);

	wxPGProperty* CreatePropertyArtId();
	wxPGProperty* CreatePropertyArtClient();

	wxString SetupImage(const wxString& imgPath = wxEmptyString);
	wxString SetupResource(const wxString& resName = wxEmptyString);

	void SetPrevSource(int src) { m_prevSrc = src; }

	virtual wxVariant ChildChanged(wxVariant& thisValue, int childIndex,
		wxVariant& childValue) const override;

	virtual void OnSetValue() override;
	void CreateChildren();

	void UpdateChildValues(const wxString& value);

	virtual void RefreshChildren() override;

protected:

	void GetChildValues(const wxString& parentValue, wxArrayString& childValues) const;

	static wxArrayString m_ids;
	static wxArrayString m_clients;
	
	wxArrayString m_strings;

	int m_prevSrc;
};

#endif