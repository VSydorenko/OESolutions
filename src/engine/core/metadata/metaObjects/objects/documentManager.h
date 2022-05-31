#ifndef _MANAGER_DOCUMENT_H__
#define _MANAGER_DOCUMENT_H__

#include "document.h"
#include "common/managerInfo.h"

class CDocumentManager : public CValue,
	public IMetaManagerInfo {
	wxDECLARE_DYNAMIC_CLASS(CDocumentManager);

public:

	CDocumentManager(CMetaObjectDocument* metaObject = NULL);
	virtual ~CDocumentManager();

	virtual CMetaCommonModuleObject* GetModuleManager() const;

	CReferenceDataObject* FindByNumber(const CValue& vCode, const CValue& vPeriod);
	CReferenceDataObject* EmptyRef();

	virtual CMethods* GetPMethods() const { PrepareNames(); return m_methods; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t& aParams);//����� ������

	//Get ref class 
	virtual CLASS_ID GetClassType() const;

	//types 
	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

protected:

	//methods 
	CMethods* m_methods;
	CMetaObjectDocument* m_metaObject;
};

#endif 