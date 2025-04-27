#include "advpropString.h"

// -----------------------------------------------------------------------
// wxGeneralStringProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxGeneralStringProperty, wxStringProperty, TextCtrl)

wxString wxGeneralStringProperty::ValueToString(wxVariant& value, int argFlags) const
{
	wxString s = value.GetString();

	if (GetChildCount() && HasFlag(wxPG_PROP_COMPOSED_VALUE))
	{
		// Value stored in m_value is non-editable, non-full value
		if ((argFlags & wxPG_FULL_VALUE) ||
			(argFlags & wxPG_EDITABLE_VALUE) ||
			s.empty())
		{
			// Calling this under incorrect conditions will fail
			wxASSERT_MSG(argFlags & wxPG_VALUE_IS_CURRENT,
				wxS("Sorry, currently default wxPGProperty::ValueToString() ")
				wxS("implementation only works if value is m_value."));

			DoGenerateComposedValue(s, argFlags);
		}

		return s;
	}

	// If string is password and value is for visual purposes,
	// then return asterisks instead the actual string.
	if ((m_flags & wxPG_PROP_PASSWORD) && !(argFlags & (wxPG_FULL_VALUE | wxPG_EDITABLE_VALUE)))
		return wxString(wxS('*'), s.Length());

	return s;
}

bool wxGeneralStringProperty::StringToValue(wxVariant& variant,
	const wxString& text,
	int argFlags) const
{
	if (stringUtils::CheckCorrectName(text) >= 0) {
		wxMessageBox(wxT("You can enter only numbers, letters and the symbol \"_\""), wxT("Error entering value"));
		return false;
	}

	if (GetChildCount() && HasFlag(wxPG_PROP_COMPOSED_VALUE))
		return wxPGProperty::StringToValue(variant, text, argFlags);

	if (variant != text) {
		variant = text;
		return text.length() > 0;
	}

	return false;
}