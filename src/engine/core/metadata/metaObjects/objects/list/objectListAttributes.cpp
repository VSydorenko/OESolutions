////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : list attrubutes 
////////////////////////////////////////////////////////////////////////////

#include "objectList.h"

//****************************************************************************
//*                              Override attribute                          *
//****************************************************************************

enum {
	enChoiceMode
};

void CListDataObjectRef::SetAttribute(attributeArg_t& aParams, CValue& value) //��������� ��������
{
}

CValue CListDataObjectRef::GetAttribute(attributeArg_t& aParams) //�������� ��������
{
	if (aParams.GetIndex() == enChoiceMode) {
		return m_choiceMode;
	}

	return CValue();
}

void CListRegisterObject::SetAttribute(attributeArg_t& aParams, CValue& value) //��������� ��������
{
}

CValue CListRegisterObject::GetAttribute(attributeArg_t& aParams) //�������� ��������
{
	return CValue();
}