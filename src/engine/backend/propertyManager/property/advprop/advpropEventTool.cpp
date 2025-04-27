#include "advpropEventTool.h"
#include "backend/propertyManager/propertyEditor.h"

#include "backend/stringUtils.h"

// -----------------------------------------------------------------------
// wxEventToolProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxEventToolProperty, wxStringProperty, ComboBoxAndButton)

wxEventToolProperty::wxEventToolProperty(const wxString& label, const wxString& strName,
	const wxPGChoices& choices,
	const wxString& value) : wxPGProperty(label, strName)
{
	SetChoices(choices);
	long action_id = wxNOT_FOUND;
	if (value.ToLong(&action_id)) {
		for (unsigned int i = 0; i < m_choices.GetCount(); i++) {
			int val = m_choices.GetValue(i);
			if (action_id == val) {
				m_flags &= ~(wxPG_PROP_ACTIVE_BTN);
				SetValue(m_choices.GetLabel(i));
				m_toolData.SetNumber(val);
				return;
			}
		}
		m_toolData.SetString(value);
		m_flags |= wxPG_PROP_ACTIVE_BTN; // Property button always enabled.
		SetValue(value);
	}
	else {
		m_toolData.SetString(value);
		m_flags |= wxPG_PROP_ACTIVE_BTN; // Property button always enabled.
		SetValue(value);
	}
}

wxEventToolProperty::~wxEventToolProperty()
{
}

wxString wxEventToolProperty::ValueToString(wxVariant& value, int argFlags) const
{
	long action_id = wxNOT_FOUND;
	if (value.Convert(&action_id)) {
		if (action_id == wxNOT_FOUND)
			return wxEmptyString;
		for (unsigned int i = 0; i < m_choices.GetCount(); i++) {
			const int sel_val = m_choices.GetValue(i);
			if (action_id == sel_val) {
				return m_choices.GetLabel(i);
			}
		}
	}

	return value;
}

bool wxEventToolProperty::StringToValue(wxVariant& variant,
	const wxString& text,
	int argFlags) const
{
	if (stringUtils::CheckCorrectName(text) > 0)
		return false;

	if (GetChildCount() && HasFlag(wxPG_PROP_COMPOSED_VALUE))
		return wxPGProperty::StringToValue(variant, text, argFlags);

	if (variant != text) {
		if (text.IsEmpty()) {
			m_toolData.SetString(text);
		}
		variant = text;
		return true;
	}

	return false;
}

bool wxEventToolProperty::IntToValue(wxVariant& value, int number, int argFlags) const
{
	value = (long)m_choices.GetValue(number);
	m_toolData.SetNumber(
		m_choices.GetValue(number)
	);
	return true;
}

void wxEventToolProperty::OnSetValue()
{
	if (m_toolData.IsCustomEvent()) {
		SetFlag(wxPG_PROP_ACTIVE_BTN); // Property button always enabled.
	}
	else {
		ClearFlag(wxPG_PROP_ACTIVE_BTN); // Property button always disabled.
	}
}

#include <wx/propgrid/advprops.h>

wxPGEditorDialogAdapter* wxEventToolProperty::GetEditorDialog() const
{
	class wxPGEditorEventDialogAdapter : public wxPGEditorDialogAdapter {
		wxString GetUncommittedPropertyValue(wxPropertyGrid* pg) {
			const wxString& eventName = pg->GetUncommittedPropertyValue();
			long action_id = wxNOT_FOUND;
			if (eventName.ToLong(&action_id)) {
				if (action_id == wxNOT_FOUND)
					return wxEmptyString;
			}
			return eventName;
		}
	public:
		virtual bool DoShowDialog(wxPropertyGrid* pg, wxPGProperty* prop) wxOVERRIDE
		{
			wxEventToolProperty* dlgProp = wxDynamicCast(prop, wxEventToolProperty);
			wxCHECK_MSG(dlgProp, false, "Function called for incompatible property");
			const wxString& eventName = GetUncommittedPropertyValue(pg);
			if (eventName.IsEmpty()) {
				wxPGProperty* pgProp = pg->GetPropertyByLabel(wxT("name"));
				prop->SetValueFromString(
					(pgProp ? pgProp->GetDisplayedString() : wxEmptyString) + wxT("_") + prop->GetLabel());
				SetValue(prop->GetValue());
			}
			else {
				SetValue(eventName);
			}
			return true;
		}
	};
	return new wxPGEditorEventDialogAdapter();
}