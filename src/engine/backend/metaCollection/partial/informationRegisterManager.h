#ifndef _INFO_REGISTER_MANAGER_H__
#define _INFO_REGISTER_MANAGER_H__

#include "informationRegister.h"
#include "backend/wrapper/managerInfo.h"

class CInformationRegisterManager : public CValue,
	public IMetaManagerInfo {
	wxDECLARE_DYNAMIC_CLASS(CInformationRegisterManager);
public:

	CValue Get(const CValue& cFilter = CValue());
	CValue Get(const CValue& cPeriod, const CValue& cFilter);

	CValue GetFirst(const CValue& cPeriod, const CValue& cFilter = CValue());
	CValue GetLast(const CValue& cPeriod, const CValue& cFilter = CValue());

	CValue SliceFirst(const CValue& cPeriod, const CValue& cFilter = CValue());
	CValue SliceLast(const CValue& cPeriod, const CValue& cFilter = CValue());

	CInformationRegisterManager(CMetaObjectInformationRegister* metaObject = nullptr);
	virtual ~CInformationRegisterManager();

	virtual CMetaObjectCommonModule* GetModuleManager() const;

	virtual CMethodHelper* GetPMethods() const { //�������� ������ �� ����� �������� ������� ���� ��������� � �������
		//PrepareNames(); 
		return m_methodHelper;
	}
	virtual void PrepareNames() const; //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray); //����� ������

	//Get ref class 
	virtual class_identifier_t GetClassType() const;

	//types 
	virtual wxString GetClassName() const;
	virtual wxString GetString() const;

protected:
	//methods 
	CMethodHelper* m_methodHelper;
	CMetaObjectInformationRegister* m_metaObject;
};

#endif 