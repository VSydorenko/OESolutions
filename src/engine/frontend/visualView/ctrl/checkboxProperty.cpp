#include "widgets.h"

void CValueCheckbox::OnPropertyCreated(IProperty* property)
{
	//if (m_propertySource == property) {
	//	CValueCheckbox::SaveToVariant(m_propertySource->GetValue(), GetMetaData());
	//}
}

bool CValueCheckbox::OnPropertyChanging(IProperty* property, const wxVariant& newValue)
{
	//if (m_propertySource == property && !CValueCheckbox::LoadFromVariant(newValue))
	//	return false;
	return IValueControl::OnPropertyChanging(property, newValue);
}