#ifndef _MANAGER_CATALOG_H__
#define _MANAGER_CATALOG_H__

#include "catalog.h"
#include "backend/wrapper/managerInfo.h"

class CCatalogManager : public CValue,
	public IMetaManagerInfo {
	wxDECLARE_DYNAMIC_CLASS(CCatalogManager);
public:

	CCatalogManager(CMetaObjectCatalog* metaObject = nullptr);
	virtual ~CCatalogManager();

	virtual CMetaObjectCommonModule* GetModuleManager() const;

	CReferenceDataObject* FindByCode(const CValue& vCode);
	CReferenceDataObject* FindByName(const CValue& vName);

	CReferenceDataObject* EmptyRef();

	virtual CMethodHelper* GetPMethods() const { // get a reference to the class helper for parsing attribute and method names
		//PrepareNames(); 
		return m_methodHelper;
	}
	virtual void PrepareNames() const;                         // this method is automatically called to initialize attribute and method names.
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//method call

	//Get ref class 
	virtual class_identifier_t GetClassType() const;

	//types 
	virtual wxString GetClassName() const;
	virtual wxString GetString() const;

protected:
	//methods 
	CMethodHelper* m_methodHelper;
	CMetaObjectCatalog* m_metaObject;
};

#endif 