#ifndef _MANAGER_CATALOG_H__
#define _MANAGER_CATALOG_H__

#include "catalog.h"
#include "common/managerInfo.h"

class CCatalogManager : public CValue,
	public IMetaManagerInfo {
	wxDECLARE_DYNAMIC_CLASS(CCatalogManager);
public:

	CCatalogManager(CMetaObjectCatalog* metaObject = NULL);
	virtual ~CCatalogManager();

	virtual CMetaCommonModuleObject* GetModuleManager() const;

	CReferenceDataObject* FindByCode(const CValue& vCode);
	CReferenceDataObject* FindByName(const CValue& vName);

	CReferenceDataObject* EmptyRef();

	virtual CMethods* GetPMethods() const { PrepareNames();  return m_methods; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
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
	CMetaObjectCatalog* m_metaObject;
};

#endif 