#include "interfaceEditorProperty.h"
#include "frontend/mainFrame/mainFrame.h"

wxIMPLEMENT_DYNAMIC_CLASS(CValueEnumMenuType, CValue);

CPropertyInterfaceItem::~CPropertyInterfaceItem() {
	if (objectInspector->GetSelectedObject() == this) {
		objectInspector->ClearProperty();
	}
}

void CPropertyInterfaceItem::OnPropertyRefresh(wxPropertyGridManager* pg,
	wxPGProperty* pgProperty, IProperty* property)
{
	const eMenuType menuType = (eMenuType)m_propertyMenuType->GetValueAsInteger();
	if (menuType == eMenuType::eMenu) {
		if (m_propertySubMenu == property)
			pg->HideProperty(pgProperty, true);
		if (m_propertyAction == property)
			pg->HideProperty(pgProperty, false);
		if (m_propertyCaption == property)
			pg->HideProperty(pgProperty, false);
		if (m_propertyToolTip == property)
			pg->HideProperty(pgProperty, false);
		if (m_propertyDescription == property)
			pg->HideProperty(pgProperty, false);
		if (m_propertyPicture == property)
			pg->HideProperty(pgProperty, false);

	}
	else if (menuType == eMenuType::eSubMenu) {
		if (m_propertySubMenu == property)
			pg->HideProperty(pgProperty, false);
		if (m_propertyAction == property)
			pg->HideProperty(pgProperty, true);
		if (m_propertyCaption == property)
			pg->HideProperty(pgProperty, false);
		if (m_propertyToolTip == property)
			pg->HideProperty(pgProperty, false);
		if (m_propertyDescription == property)
			pg->HideProperty(pgProperty, false);
		if (m_propertyPicture == property)
			pg->HideProperty(pgProperty, false);
	}
	else if (menuType == eMenuType::eSeparator) {
		if (m_propertySubMenu == property)
			pg->HideProperty(pgProperty, true);
		if (m_propertyAction == property)
			pg->HideProperty(pgProperty, true);
		if (m_propertyCaption == property)
			pg->HideProperty(pgProperty, true);
		if (m_propertyToolTip == property)
			pg->HideProperty(pgProperty, true);
		if (m_propertyDescription == property)
			pg->HideProperty(pgProperty, true);
		if (m_propertyPicture == property)
			pg->HideProperty(pgProperty, true);
	}
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

//add new enumeration
ENUM_TYPE_REGISTER(CValueEnumMenuType, "menuType", string_to_clsid("EN_METY"));