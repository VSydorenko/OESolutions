#ifndef _CONSTANTS_MANAGER_H__
#define _CONSTANTS_MANAGER_H__

#include "constant.h"

class CConstantManager : public CValue {
	wxDECLARE_DYNAMIC_CLASS(CConstantManager);
protected:
	CMetaConstantObject *m_metaConst;
public:

	CConstantManager(CMetaConstantObject *metaConst = NULL);
	virtual ~CConstantManager();

	virtual CMethods* GetPMethods() const { PrepareNames();  return &m_methods; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t &aParams);//����� ������

	//Get ref class 
	virtual CLASS_ID GetClassType() const;

	//types 
	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

protected:

	//methods 
	static CMethods m_methods;
};


#endif // !_CONSTANTS_MANAGER_H__
