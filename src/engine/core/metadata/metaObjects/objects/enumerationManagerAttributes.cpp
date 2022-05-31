////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : enumeration manager - attributes
////////////////////////////////////////////////////////////////////////////

#include "enumerationManager.h"
#include "reference/reference.h"
#include "appData.h"

//****************************************************************************
//*                              Override attribute                          *
//****************************************************************************
void CEnumerationManager::SetAttribute(attributeArg_t &aParams, CValue &cValue) {}       //��������� ��������

#include "compiler/methods.h"

CValue CEnumerationManager::GetAttribute(attributeArg_t &aParams)                     //�������� ��������
{
	auto enums = m_metaObject->GetObjectEnums();
	CMetaEnumerationObject *m_enum = enums.at(aParams.GetIndex());

	return CReferenceDataObject::Create(m_metaObject, m_enum->GetGuid());
}