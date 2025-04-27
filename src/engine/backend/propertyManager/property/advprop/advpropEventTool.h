#ifndef __ADVPROP_EVENT_TOOL_H__
#define __ADVPROP_EVENT_TOOL_H__

#include <wx/propgrid/propgrid.h>
#include "backend/backend_core.h"

class BACKEND_API IPropertyObject;

// -----------------------------------------------------------------------
// wxEventToolProperty
// -----------------------------------------------------------------------

class wxEventToolProperty : public wxPGProperty {
	WX_PG_DECLARE_PROPERTY_CLASS(wxEventToolProperty)
private:
	mutable class toolData_t {
	public:
		void SetNumber(long number) {
			m_customEvent = false; m_toolID = number; m_strEvent = wxEmptyString;
		}
		int GetNumber() const { return m_toolID; }
		void SetString(const wxString& string) {
			m_customEvent = true; m_strEvent = string; m_toolID = wxNOT_FOUND;
		}
		wxString GetString() const { return m_strEvent; }
		bool IsCustomEvent() const { return m_customEvent; }
	private:
		int m_toolID = wxNOT_FOUND;
		bool m_customEvent = false;
		wxString m_strEvent = wxEmptyString;
	} m_toolData;
public:

	bool IsCustomEvent() const {
		return m_toolData.IsCustomEvent();
	}

	int GetNumber() const { return m_toolData.GetNumber(); }
	wxString GetString() const { return m_toolData.GetString(); }

	wxEventToolProperty(const wxString& label = wxPG_LABEL,
		const wxString& name = wxPG_LABEL, const wxPGChoices& choices = wxPGChoices(),
		const wxString& value = wxEmptyString);

	virtual ~wxEventToolProperty();

	virtual wxString ValueToString(wxVariant& value, int argFlags = 0) const override;
	virtual bool StringToValue(wxVariant& variant,
		const wxString& text,
		int argFlags = 0) const override;
	virtual bool IntToValue(wxVariant& value,
		int number,
		int argFlags = 0) const override;

	virtual void OnSetValue() override;
	virtual wxPGEditorDialogAdapter* GetEditorDialog() const override;

protected:
	IPropertyObject* m_ownerProperty;
};

#endif