#include "propertyPicture.h"

void CPropertyPicture::SetValue(const wxBitmap& bmp)
{
	m_propValue = wxT("Load From Art Provider;;");
}

void CPropertyPicture::SetValue(const wxString& bmp)
{
	m_propValue = bmp; 
}

wxBitmap CPropertyPicture::GetValueAsBitmap() const
{
	return typeConv::StringToBitmap(m_propValue);
}

wxString CPropertyPicture::GetValueAsString() const
{
	return m_propValue.GetString();
}

//base property for "picture"
bool CPropertyPicture::SetDataValue(const CValue& varPropVal)
{
	return false;
}

bool CPropertyPicture::GetDataValue(CValue& pvarPropVal) const
{
	pvarPropVal = _("picture_data");
	return true;
}

bool CPropertyPicture::LoadData(CMemoryReader& reader)
{
	CPropertyPicture::SetValue(reader.r_stringZ());
	return true;
}

bool CPropertyPicture::SaveData(CMemoryWriter& writer)
{
	writer.w_stringZ(CPropertyPicture::GetValueAsString());
	return true;
}
