#ifndef _MANAGER_DATAPROCESSOR_H__
#define _MANAGER_DATAPROCESSOR_H__

#include "dataProcessor.h"
#include "backend/wrapper/managerInfo.h"

class CDataProcessorManager : public CValue,
	public IMetaManagerInfo {
	wxDECLARE_DYNAMIC_CLASS(CDataProcessorManager);
public:

	virtual CMetaObjectCommonModule* GetModuleManager() const;

	CDataProcessorManager(CMetaObjectDataProcessor* metaObject = nullptr);
	virtual ~CDataProcessorManager();

	virtual CMethodHelper* GetPMethods() const {  //�������� ������ �� ����� �������� ������� ���� ��������� � �������
		//PrepareNames();
		return m_methodHelper;
	}
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//����� ������

	//Get ref class 
	virtual class_identifier_t GetClassType() const;

	//types 
	virtual wxString GetClassName() const;
	virtual wxString GetString() const;

protected:

	CMethodHelper* m_methodHelper;
	CMetaObjectDataProcessor* m_metaObject;
};

class CManagerExternalDataProcessorValue : public CValue {
	wxDECLARE_DYNAMIC_CLASS(CManagerExternalDataProcessorValue);
public:
	CManagerExternalDataProcessorValue();
	virtual ~CManagerExternalDataProcessorValue();

	virtual CMethodHelper* GetPMethods() const { //�������� ������ �� ����� �������� ������� ���� ��������� � �������
		//PrepareNames();
		return m_methodHelper; 
	} 
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//����� ������

protected:
	CMethodHelper* m_methodHelper;
};

#endif 