#include "toolbar.h"
#include "form.h"

inline void ParseElements(IValueFrame* element, CPropertyList* property)
{
	if (element->GetClassName() == wxT("tablebox")) {
		property->AppendItem(element->GetControlName(), element->GetControlID(), element);
	}

	for (unsigned int i = 0; i < element->GetChildCount(); i++) {
		ParseElements(element->GetChild(i), property);
	}
}

bool CValueToolbar::GetActionSource(CPropertyList* property)
{
	property->AppendItem(_("<not selected>"), wxNOT_FOUND, wxEmptyValue);
	property->AppendItem(_("form"), FORM_ACTION, (IValueFrame *)m_formOwner);

	ParseElements(GetOwnerForm(), property);
	return true;
}