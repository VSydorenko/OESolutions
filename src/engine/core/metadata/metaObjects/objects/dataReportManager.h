#ifndef _MANAGER_REPORT_H__
#define _MANAGER_REPORT_H__

#include "dataReport.h"
#include "common/managerInfo.h"

class CReportManager : public CValue,
	public IMetaManagerInfo {
	wxDECLARE_DYNAMIC_CLASS(CReportManager);
public:

	CReportManager(CMetaObjectReport *metaObject = NULL);
	virtual ~CReportManager();

	virtual CMetaCommonModuleObject *GetModuleManager() const;

	virtual CMethods* GetPMethods() const { PrepareNames(); return m_methods; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t &aParams);//����� ������

	//Get ref class 
	virtual CLASS_ID GetClassType() const;

	//types 
	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

protected:
	CMethods *m_methods;
	CMetaObjectReport *m_metaObject;
};

class CManagerExternalReport : public CValue {
	wxDECLARE_DYNAMIC_CLASS(CManagerExternalReport);
public:
	CManagerExternalReport();
	virtual ~CManagerExternalReport();

	virtual CMethods* GetPMethods() const { PrepareNames(); return m_methods; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t &aParams);//����� ������

	//types 
	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

protected:
	CMethods *m_methods;
};

#endif 