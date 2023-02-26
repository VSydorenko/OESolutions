#ifndef _MANAGER_CATALOG_H__
#define _MANAGER_CATALOG_H__

#include "catalog.h"
#include "core/common/managerInfo.h"

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

	virtual CMethodHelper* GetPMethods() const { PrepareNames();  return m_methodHelper; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//����� ������

	//Get ref class 
	virtual CLASS_ID GetTypeClass() const;

	//types 
	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

protected:
	//methods 
	CMethodHelper* m_methodHelper;
	CMetaObjectCatalog* m_metaObject;
};

#endif 