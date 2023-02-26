#ifndef _ENUMERATION_MANAGER_H__
#define _ENUMERATION_MANAGER_H__

#include "enumeration.h"
#include "core/common/managerInfo.h"

class CEnumerationManager : public CValue,
	public IMetaManagerInfo {
	wxDECLARE_DYNAMIC_CLASS(CEnumerationManager);
public:

	CEnumerationManager(CMetaObjectEnumeration *metaObject = NULL);
	virtual ~CEnumerationManager();

	virtual CMetaCommonModuleObject *GetModuleManager() const;

	virtual CMethodHelper* GetPMethods() const { PrepareNames(); return m_methodHelper; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//����� ������

	virtual bool SetPropVal(const long lPropNum, CValue &varPropVal);        //��������� ��������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);                   //�������� ��������

	//Get ref class 
	virtual CLASS_ID GetTypeClass() const;

	//types 
	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

protected:
	//methods 
	CMethodHelper *m_methodHelper;
	CMetaObjectEnumeration *m_metaObject;
};

#endif 