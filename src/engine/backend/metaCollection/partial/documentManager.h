#ifndef _MANAGER_DOCUMENT_H__
#define _MANAGER_DOCUMENT_H__

#include "document.h"
#include "backend/managerInfo.h"

class CDocumentManager : public CValue,
	public IMetaManagerInfo {
	wxDECLARE_DYNAMIC_CLASS(CDocumentManager);

public:

	CDocumentManager(CMetaObjectDocument* metaObject = nullptr);
	virtual ~CDocumentManager();

	virtual CMetaObjectCommonModule* GetModuleManager() const;

	CReferenceDataObject* FindByNumber(const CValue& vCode, const CValue& vPeriod);
	CReferenceDataObject* EmptyRef();

	virtual CMethodHelper* GetPMethods() const { // get a reference to the class helper for parsing attribute and method names
		//PrepareNames(); 
		return m_methodHelper;
	}
	virtual void PrepareNames() const; // this method is automatically called to initialize attribute and method names.
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//method call

	//Get ref class 
	virtual class_identifier_t GetClassType() const;

	//types 
	virtual wxString GetClassName() const;
	virtual wxString GetString() const;

protected:

	//methods 
	CMethodHelper* m_methodHelper;
	CMetaObjectDocument* m_metaObject;
};

#endif 