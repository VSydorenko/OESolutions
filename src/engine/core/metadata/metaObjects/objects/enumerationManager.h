#ifndef _ENUMERATION_MANAGER_H__
#define _ENUMERATION_MANAGER_H__

#include "enumeration.h"
#include "common/managerInfo.h"

class CEnumerationManager : public CValue,
	public IMetaManagerInfo {
	wxDECLARE_DYNAMIC_CLASS(CEnumerationManager);
public:

	CEnumerationManager(CMetaObjectEnumeration *metaObject = NULL);
	virtual ~CEnumerationManager();

	virtual CMetaCommonModuleObject *GetModuleManager() const;

	virtual CMethods* GetPMethods() const { PrepareNames(); return m_methods; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t &aParams);//����� ������

	virtual void SetAttribute(attributeArg_t &aParams, CValue &cVal);        //��������� ��������
	virtual CValue GetAttribute(attributeArg_t &aParams);                   //�������� ��������

	//Get ref class 
	virtual CLASS_ID GetClassType() const;

	//types 
	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

protected:
	//methods 
	CMethods *m_methods;
	CMetaObjectEnumeration *m_metaObject;
};

#endif 