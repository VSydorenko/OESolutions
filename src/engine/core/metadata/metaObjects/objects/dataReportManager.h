#ifndef _MANAGER_REPORT_H__
#define _MANAGER_REPORT_H__

#include "dataReport.h"
#include "core/common/managerInfo.h"

class CReportManager : public CValue,
	public IMetaManagerInfo {
	wxDECLARE_DYNAMIC_CLASS(CReportManager);
public:

	CReportManager(CMetaObjectReport* metaObject = NULL);
	virtual ~CReportManager();

	virtual CMetaCommonModuleObject* GetModuleManager() const;

	virtual CMethodHelper* GetPMethods() const { PrepareNames(); return m_methodHelper; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//����� ������

	//Get ref class 
	virtual CLASS_ID GetTypeClass() const;

	//types 
	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

protected:
	CMethodHelper* m_methodHelper;
	CMetaObjectReport* m_metaObject;
};

class CManagerExternalReport : public CValue {
	wxDECLARE_DYNAMIC_CLASS(CManagerExternalReport);
public:
	CManagerExternalReport();
	virtual ~CManagerExternalReport();

	virtual CMethodHelper* GetPMethods() const { PrepareNames(); return m_methodHelper; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//����� ������

	//types 
	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

protected:
	CMethodHelper* m_methodHelper;
};

#endif 