#ifndef _BASE_OBJECT_H__
#define _BASE_OBJECT_H__

#include "reference/reference.h"
#include "metadata/metaObjectsDefines.h"
#include "compiler/valueGuid.h"

#include "common/tableAttributes.h"

#include "common/actionInfo.h"
#include "common/moduleInfo.h"
#include "common/valueInfo.h"
#include "common/tableInfo.h"

//********************************************************************************************
//*                                     Defines                                              *
//********************************************************************************************

class CMetaConstantObject;

class IMetaTypeObjectValueSingle;

class IMetaObjectWrapperData;
class IMetaObjectRecordData;
class IMetaObjectRecordDataRef;
class IMetaObjectRegisterData;

class ISourceDataObject;

class IRecordDataObject;
class IRecordDataObjectRef;

class CRecordKeyObject;
class IRecordManagerObject;
class IRecordSetObject;

class CSourceExplorer;

//special names 
#define guidName wxT("UUID")
#define guidRef wxT("UUIDREF")

//********************************************************************************************
//*                                  Factory & metadata                                      *
//********************************************************************************************

enum
{
	METAOBJECT_NORMAL = 1,
	METAOBJECT_EXTERNAL,
};

class IMetaCommandData {
public:
	//get default form 
	virtual CValueForm* GetDefaultCommandForm() = 0;
};

class IMetaObjectWrapperData
	: public IMetaObject, public IMetaCommandData, public ITableAttribute {
	wxDECLARE_ABSTRACT_CLASS(IMetaObjectWrapperData);
public:

	//runtime support:
	IMetaTypeObjectValueSingle* GetTypeObject(enum eMetaObjectType refType);

	//find in current metaObject
	IMetaObject* FindMetaObjectByID(const meta_identifier_t& id);

	//get data selector 
	virtual eSelectorDataType GetSelectorDataType() const {
		return eSelectorDataType::eSelectorDataType_reference;
	}

	//override base objects 
	virtual std::vector<IMetaAttributeObject*> GetGenericAttributes() const = 0;
	virtual std::vector<CMetaFormObject*> GetGenericForms() const = 0;

	//find attributes, tables etc 
	virtual IMetaAttributeObject* FindGenericAttributeByName(const wxString& docPath) const
	{
		for (auto obj : GetGenericAttributes()) {
			if (docPath == obj->GetDocPath())
				return obj;
		}
		return NULL;
	}

	//find in current metaObject
	virtual IMetaAttributeObject* FindGenericAttribute(const meta_identifier_t& id) const;

	//form events 
	virtual void OnCreateMetaForm(IMetaFormObject* metaForm) {}
	virtual void OnRemoveMetaForm(IMetaFormObject* metaForm) {}

	//Get form type
	virtual OptionList* GetFormType() = 0;

	//create form with data 
	virtual CValueForm* CreateObjectValue(IMetaFormObject* metaForm) = 0; //create with form

	//support form 
	virtual CValueForm* GetGenericForm(const wxString& formName = wxEmptyString, IValueFrame* ownerControl = NULL, const CUniqueKey& formGuid = Guid());

protected:

	//support form 
	virtual CValueForm* GetGenericForm(const meta_identifier_t& id, IValueFrame* ownerControl, const CUniqueKey& formGuid = Guid());
};

class IMetaObjectRecordData
	: public IMetaObjectWrapperData {
	wxDECLARE_ABSTRACT_CLASS(IMetaObjectRecordData);
protected:

	std::vector<IMetaObject*> m_aMetaObjects;

public:

	IMetaObjectRecordData();
	virtual ~IMetaObjectRecordData();

	//get data selector 
	virtual eSelectorDataType GetSelectorDataType() const {
		return eSelectorDataType::eSelectorDataType_any;
	}

	//meta events
	virtual bool OnLoadMetaObject(IMetadata* metaData);
	virtual bool OnSaveMetaObject();
	virtual bool OnDeleteMetaObject();

	//module manager is started or exit 
	virtual bool OnBeforeRunMetaObject(int flags);
	virtual bool OnAfterCloseMetaObject();

	//base objects 
	virtual std::vector<IMetaObject*> GetObjects(const CLASS_ID& clsid) const { return IMetaObject::GetObjects(clsid); }
	virtual std::vector<IMetaObject*> GetObjects() const { return m_aMetaObjects; }

	//override base objects 
	virtual std::vector<IMetaAttributeObject*> GetGenericAttributes() const;
	virtual std::vector<CMetaFormObject*> GetGenericForms() const;

	//get default attributes
	virtual std::vector<IMetaAttributeObject*> GetDefaultAttributes() const {
		return std::vector<IMetaAttributeObject*>();
	}

	//get attributes, form etc.. 
	virtual std::vector<IMetaAttributeObject*> GetObjectAttributes() const;
	virtual std::vector<CMetaTableObject*> GetObjectTables() const;
	virtual std::vector<CMetaFormObject*> GetObjectForms() const;
	virtual std::vector<CMetaGridObject*> GetObjectTemplates() const;

	//find attributes, tables etc 
	virtual IMetaAttributeObject* FindDefAttributeByName(const wxString& docPath) const
	{
		for (auto obj : GetDefaultAttributes()) {
			if (docPath == obj->GetDocPath())
				return obj;
		}
		return NULL;
	}

	virtual IMetaAttributeObject* FindAttributeByName(const wxString& docPath) const
	{
		for (auto obj : GetObjectAttributes()) {
			if (docPath == obj->GetDocPath())
				return obj;
		}
		return NULL;
	}

	virtual CMetaTableObject* FindTableByName(const wxString& docPath) const
	{
		for (auto obj : GetObjectTables()) {
			if (docPath == obj->GetDocPath())
				return obj;
		}
		return NULL;
	}

	//get module object in compose object 
	virtual CMetaModuleObject* GetModuleObject() const { return NULL; }
	virtual CMetaCommonModuleObject* GetModuleManager() const { return NULL; }

	//Get metaObject by def id
	virtual CMetaFormObject* GetDefaultFormByID(const form_identifier_t& id) = 0;

	//create associate value 
	virtual IRecordDataObject* CreateObjectValue() = 0; //create object 

	//create in this metaObject 
	virtual void AppendChild(IMetaObject* child) { m_aMetaObjects.push_back(child); }
	virtual void RemoveChild(IMetaObject* child) {
		auto itFounded = std::find(m_aMetaObjects.begin(), m_aMetaObjects.end(), child);
		if (itFounded != m_aMetaObjects.end()) m_aMetaObjects.erase(itFounded);
	}

	//get default form 
	virtual CValueForm* GetDefaultCommandForm() {
		return GetObjectForm();
	};

	//support form 
	virtual CValueForm* GetObjectForm(const wxString& formName = wxEmptyString, IValueFrame* ownerControl = NULL, const CUniqueKey& formGuid = Guid()) = 0;

protected:

	//support form 
	virtual CValueForm* GetObjectForm(const meta_identifier_t& id, IValueFrame* ownerControl, const CUniqueKey& formGuid = Guid());

protected:

	friend class CMetaFormObject;
	friend class IRecordDataObject;
};

//metaObject with reference 
class IMetaObjectRecordDataRef : public IMetaObjectRecordData {
	wxDECLARE_ABSTRACT_CLASS(IMetaObjectRecordDataRef);
public:

	virtual CMetaDefaultAttributeObject* GetDataReference() const {
		return m_attributeReference;
	}

	virtual bool IsDataReference(const meta_identifier_t& id) const {
		return id == m_attributeReference->GetMetaID();
	}

	//ctor
	IMetaObjectRecordDataRef();
	virtual ~IMetaObjectRecordDataRef();

	//get data selector 
	virtual eSelectorDataType GetSelectorDataType() const {
		return eSelectorDataType::eSelectorDataType_reference;
	}

	//meta events
	virtual bool OnCreateMetaObject(IMetadata* metaData);
	virtual bool OnLoadMetaObject(IMetadata* metaData);
	virtual bool OnSaveMetaObject();
	virtual bool OnDeleteMetaObject();

	//module manager is started or exit 
	//after and before for designer 
	virtual bool OnBeforeRunMetaObject(int flags);
	virtual bool OnAfterRunMetaObject(int flags);

	virtual bool OnBeforeCloseMetaObject();
	virtual bool OnAfterCloseMetaObject();

	//process choice 
	virtual bool ProcessChoice(IValueFrame* ownerValue, const meta_identifier_t& id = wxNOT_FOUND);
	virtual bool ProcessListChoice(IValueFrame* ownerValue, const meta_identifier_t& id = wxNOT_FOUND);

	//find attributes, tables etc 
	virtual CMetaEnumerationObject* FindEnumByName(const wxString& docPath) const
	{
		for (auto obj : GetObjectEnums()) {
			if (docPath == obj->GetDocPath())
				return obj;
		}
		return NULL;
	}

	//override base objects 
	virtual std::vector<IMetaAttributeObject*> GetGenericAttributes() const;
	virtual std::vector<CMetaFormObject*> GetGenericForms() const;

	//get attribute code 
	virtual IMetaAttributeObject* GetAttributeForCode() const {
		return NULL;
	}

	//override base objects 
	virtual std::vector<IMetaAttributeObject*> GetDefaultAttributes() const = 0;

	//other objects
	virtual std::vector<CMetaEnumerationObject*> GetObjectEnums() const;

	//searched attributes 
	virtual std::vector<IMetaAttributeObject*> GetSearchedAttributes() const = 0;

	//create associate value 
	virtual IRecordDataObject* CreateObjectValue() override;

	//create empty group
	virtual IRecordDataObjectRef* CreateGroupObjectRefValue() { return NULL; }
	virtual IRecordDataObjectRef* CreateGroupObjectRefValue(const Guid& guid) { return NULL; }

	//create empty object
	virtual IRecordDataObjectRef* CreateObjectRefValue() = 0;  //create object 
	virtual IRecordDataObjectRef* CreateObjectRefValue(const Guid& guid) = 0; //create object and read by guid 

	virtual CReferenceDataObject* FindObjectValue(const Guid& guid); //find by guid and ret reference 

	//get default form 
	virtual CValueForm* GetDefaultCommandForm() {
		return GetListForm();
	};

	//support form 
	virtual CValueForm* GetListForm(const wxString& formName = wxEmptyString, IValueFrame* ownerControl = NULL, const CUniqueKey& formGuid = Guid()) = 0;
	virtual CValueForm* GetSelectForm(const wxString& formName = wxEmptyString, IValueFrame* ownerControl = NULL, const CUniqueKey& formGuid = Guid()) = 0;

	//descriptions...
	virtual wxString GetDescription(const IObjectValueInfo* objValue) const = 0;

	//special functions for DB 
	virtual wxString GetTableNameDB() const;

	//create and update table 
	virtual bool CreateAndUpdateTableDB(IConfigMetadata* srcMetaData, IMetaObject* srcMetaObject, int flags);

	//load & save metadata from DB 
	virtual bool LoadData(CMemoryReader& reader);
	virtual bool SaveData(CMemoryWriter& writer = CMemoryWriter());
	virtual bool DeleteData() { return true; }

	//process default query
	int ProcessAttribute(const wxString& tableName, IMetaAttributeObject* srcAttr, IMetaAttributeObject* dstAttr);
	int ProcessEnumeration(const wxString& tableName, const meta_identifier_t& id, CMetaEnumerationObject* srcEnum, CMetaEnumerationObject* dstEnum);
	int ProcessTable(const wxString& tabularName, CMetaTableObject* srcTable, CMetaTableObject* dstTable);

protected:

	//support form 
	virtual CValueForm* GetListForm(const meta_identifier_t& id, IValueFrame* ownerControl, const CUniqueKey& formGuid = Guid());
	virtual CValueForm* GetSelectForm(const meta_identifier_t& id, IValueFrame* ownerControl, const CUniqueKey& formGuid = Guid());

protected:

	CMetaDefaultAttributeObject* m_attributeReference;
};

//metaObject with reference and deletion mark 
class IMetaObjectRecordDataMutableRef : public IMetaObjectRecordDataRef {
	wxDECLARE_ABSTRACT_CLASS(IMetaObjectRecordDataMutableRef);
private:
	CMetaDefaultAttributeObject* m_attributeDeletionMark;
public:

	CMetaDefaultAttributeObject* GetDataDeletionMark() const {
		return m_attributeDeletionMark;
	}

	bool IsDataDeletionMark(const meta_identifier_t& id) const {
		return id == m_attributeDeletionMark->GetMetaID();
	}

	IMetaObjectRecordDataMutableRef();
	virtual ~IMetaObjectRecordDataMutableRef();

	//events: 
	virtual bool OnCreateMetaObject(IMetadata* metaData);
	virtual bool OnLoadMetaObject(IMetadata* metaData);
	virtual bool OnSaveMetaObject();
	virtual bool OnDeleteMetaObject();

	//module manager is started or exit 
	virtual bool OnBeforeRunMetaObject(int flags);
	virtual bool OnAfterRunMetaObject(int flags);

	virtual bool OnBeforeCloseMetaObject();
	virtual bool OnAfterCloseMetaObject();

protected:

	//load & save metadata from DB 
	virtual bool LoadData(CMemoryReader& reader);
	virtual bool SaveData(CMemoryWriter& writer = CMemoryWriter());
};

//metaObject with key   
class IMetaObjectRegisterData :
	public IMetaObjectWrapperData {
	wxDECLARE_ABSTRACT_CLASS(IMetaObjectRegisterData);
protected:

	std::vector<IMetaObject*> m_aMetaObjects;

	//default attributes 
	CMetaDefaultAttributeObject* m_attributePeriod;
	CMetaDefaultAttributeObject* m_attributeRecorder;
	CMetaDefaultAttributeObject* m_attributeLineNumber;
	CMetaDefaultAttributeObject* m_attributeLineActive;

public:

	CMetaDefaultAttributeObject* GetRegisterActive() const {
		return m_attributeLineActive;
	}

	bool IsRegisterActive(const meta_identifier_t& id) const {
		return id == m_attributeLineActive->GetMetaID();
	}

	CMetaDefaultAttributeObject* GetRegisterPeriod() const {
		return m_attributePeriod;
	}

	bool IsRegisterPeriod(const meta_identifier_t& id) const {
		return id == m_attributePeriod->GetMetaID();
	}

	CMetaDefaultAttributeObject* GetRegisterRecorder() const {
		return m_attributeRecorder;
	}

	bool IsRegisterRecorder(const meta_identifier_t& id) const {
		return id == m_attributeRecorder->GetMetaID();
	}

	CMetaDefaultAttributeObject* GetRegisterLineNumber() const {
		return m_attributeLineNumber;
	}

	bool IsRegisterLineNumber(const meta_identifier_t& id) const {
		return id == m_attributeLineNumber->GetMetaID();
	}

	///////////////////////////////////////////////////////////////////

	//base objects 
	virtual std::vector<IMetaObject*> GetObjects(const CLASS_ID& clsid) const {
		return IMetaObject::GetObjects(clsid);
	}

	virtual std::vector<IMetaObject*> GetObjects() const { return m_aMetaObjects; }

	//override base objects 
	virtual std::vector<IMetaAttributeObject*> GetGenericAttributes() const;
	virtual std::vector<CMetaFormObject*> GetGenericForms() const;

	//get default attributes
	virtual std::vector<IMetaAttributeObject*> GetDefaultAttributes() const = 0;

	//get attributes, form etc.. 
	virtual std::vector<IMetaAttributeObject*> GetObjectDimensions() const;
	virtual std::vector<IMetaAttributeObject*> GetObjectResources() const;
	virtual std::vector<IMetaAttributeObject*> GetObjectAttributes() const;

	//get dimension keys 
	virtual std::vector<IMetaAttributeObject*> GetGenericDimensions() const = 0;

	virtual std::vector<CMetaFormObject*> GetObjectForms() const;
	virtual std::vector<CMetaGridObject*> GetObjectTemplates() const;

	//find attributes, tables etc 
	virtual IMetaAttributeObject* FindDefAttributeByName(const wxString& docPath) const
	{
		for (auto obj : GetDefaultAttributes()) {
			if (docPath == obj->GetDocPath())
				return obj;
		}
		return NULL;
	}

	virtual IMetaAttributeObject* FindDimensionByName(const wxString& docPath) const
	{
		for (auto obj : GetObjectDimensions()) {
			if (docPath == obj->GetDocPath())
				return obj;
		}
		return NULL;
	}

	virtual IMetaAttributeObject* FindResourceByName(const wxString& docPath) const
	{
		for (auto obj : GetObjectResources()) {
			if (docPath == obj->GetDocPath())
				return obj;
		}

		return NULL;
	}

	virtual IMetaAttributeObject* FindAttributeByName(const wxString& docPath) const
	{
		for (auto obj : GetObjectAttributes()) {
			if (docPath == obj->GetDocPath())
				return obj;
		}
		return NULL;
	}

	//find in current metaObject
	virtual IMetaAttributeObject* FindAttribute(const meta_identifier_t& id) const;

	//has record manager 
	virtual bool HasRecordManager() const {
		return false;
	}

	//has recorder 
	virtual bool HasRecorder() const {
		return true;
	}

	virtual IRecordSetObject* CreateRecordSet() = 0;
	virtual IRecordSetObject* CreateRecordSet(const CUniquePairKey& uniqueKey) = 0;
	virtual IRecordManagerObject* CreateRecordManager() { return NULL; }
	virtual IRecordManagerObject* CreateRecordManager(const CUniquePairKey& uniqueKey) { return NULL; }

	//get module object in compose object 
	virtual CMetaModuleObject* GetModuleObject() const { return NULL; }
	virtual CMetaCommonModuleObject* GetModuleManager() const { return NULL; }

	//Get metaObject by def id
	virtual CMetaFormObject* GetDefaultFormByID(const form_identifier_t& id) = 0;

	//create associate value 
	virtual IRecordSetObject* CreateRecordSetValue() = 0; //create object 
	virtual IRecordManagerObject* CreateRecordManagerValue() { //create object
		return NULL;
	}

	//create in this metaObject 
	virtual void AppendChild(IMetaObject* child) { m_aMetaObjects.push_back(child); }
	virtual void RemoveChild(IMetaObject* child) {
		auto itFounded = std::find(m_aMetaObjects.begin(), m_aMetaObjects.end(), child);
		if (itFounded != m_aMetaObjects.end()) m_aMetaObjects.erase(itFounded);
	}

	IMetaObjectRegisterData();
	virtual ~IMetaObjectRegisterData();

	virtual wxString GetClassName() const {
		return wxT("register");
	}

	//meta events
	virtual bool OnCreateMetaObject(IMetadata* metaData);
	virtual bool OnLoadMetaObject(IMetadata* metaData);
	virtual bool OnSaveMetaObject();
	virtual bool OnDeleteMetaObject();

	//module manager is started or exit 
	virtual bool OnBeforeRunMetaObject(int flags);
	virtual bool OnAfterCloseMetaObject();

	//get default form 
	virtual CValueForm* GetDefaultCommandForm() {
		return GetListForm();
	};

	//support form 
	virtual CValueForm* GetListForm(const wxString& formName = wxEmptyString, IValueFrame* ownerControl = NULL, const CUniqueKey& formGuid = Guid()) = 0;

	//special functions for DB 
	virtual wxString GetTableNameDB() const;

	//create and update table 
	virtual bool CreateAndUpdateTableDB(IConfigMetadata* srcMetaData, IMetaObject* srcMetaObject, int flags);

	//process default query
	int ProcessDimension(const wxString& tableName, IMetaAttributeObject* srcAttr, IMetaAttributeObject* dstAttr);
	int ProcessResource(const wxString& tableName, IMetaAttributeObject* srcAttr, IMetaAttributeObject* dstAttr);
	int ProcessAttribute(const wxString& tableName, IMetaAttributeObject* srcAttr, IMetaAttributeObject* dstAttr);

protected:

	//load & save metadata from DB 
	virtual bool LoadData(CMemoryReader& reader);
	virtual bool SaveData(CMemoryWriter& writer = CMemoryWriter());
	virtual bool DeleteData() { return true; }

protected:

	//support form 
	virtual CValueForm* GetListForm(const meta_identifier_t& id, IValueFrame* ownerControl, const CUniqueKey& formGuid = Guid());
};

enum
{
	OBJECT_NORMAL = 1,
	OBJECT_GROUP
};

//********************************************************************************************
//*                                      Base data                                           *
//********************************************************************************************

#include "common/srcExplorer.h"

class ISourceDataObject
{
public:

	ISourceDataObject() {}
	virtual ~ISourceDataObject() {}

	//override default type object 
	virtual bool SaveModify() { return true; }

	//get metadata from object 
	virtual IMetaObjectWrapperData* GetMetaObject() const = 0;

	//get unique identifier 
	virtual CUniqueKey GetGuid() const = 0;

	//standart override 
	virtual CMethods* GetPMethods() const = 0;
	virtual void PrepareNames() const = 0;
	virtual CValue Method(methodArg_t& aParams) = 0;

	virtual void SetAttribute(attributeArg_t& aParams, CValue& cVal) = 0;
	virtual CValue GetAttribute(attributeArg_t& aParams) = 0;

	virtual inline bool IsEmpty() const = 0;

	//support source data 
	virtual CSourceExplorer GetSourceExplorer() const = 0;
	virtual bool GetTable(IValueTable*& tableValue, const meta_identifier_t& id) = 0;

	//support source set/get data 
	virtual void SetValueByMetaID(const meta_identifier_t& id, const CValue& cVal) {};
	virtual CValue GetValueByMetaID(const meta_identifier_t& id) const { return CValue(); }

	//counter
	virtual void IncrRef() = 0;
	virtual void DecrRef() = 0;

	//Get ref class 
	virtual CLASS_ID GetClassType() const = 0;

	virtual wxString GetTypeString() const = 0;
	virtual wxString GetString() const = 0;

	//operator 
	virtual operator CValue() const = 0;
};

//********************************************************************************************
//*                                      Object                                              *
//********************************************************************************************

#pragma region objects 
class IRecordDataObject : public CValue, public IActionSource,
	public ISourceDataObject, public IObjectValueInfo, public IModuleInfo {
	wxDECLARE_ABSTRACT_CLASS(IRecordDataObject);
public:

	//override copy constructor
	IRecordDataObject(const Guid& guid, bool newObject = true);
	IRecordDataObject(const IRecordDataObject& source);
	virtual ~IRecordDataObject();

	//support actions 
	virtual actionData_t GetActions(const form_identifier_t& formType) { return actionData_t(); }
	virtual void ExecuteAction(const action_identifier_t& action, CValueForm* srcForm) {}

	virtual IRecordDataObject* CopyObjectValue() = 0;

	//standart override 
	virtual CMethods* GetPMethods() const {
		return CValue::GetPMethods();
	}

	virtual void PrepareNames() const {
		CValue::PrepareNames();
	}

	virtual CValue Method(methodArg_t& aParams) {
		return CValue::Method(aParams);
	}

	virtual void SetAttribute(attributeArg_t& aParams, CValue& cVal) {
		CValue::SetAttribute(aParams, cVal);
	}

	virtual CValue GetAttribute(attributeArg_t& aParams) {
		return CValue::GetAttribute(aParams);
	}

	//check is empty
	virtual inline bool IsEmpty() const override {
		return CValue::IsEmpty();
	}

	//support source data 
	virtual CSourceExplorer GetSourceExplorer() const;
	virtual bool GetTable(IValueTable*& tableValue, const meta_identifier_t& id);

	//support source set/get data 
	virtual void SetValueByMetaID(const meta_identifier_t& id, const CValue& cVal);
	virtual CValue GetValueByMetaID(const meta_identifier_t& id) const;

	//counter
	virtual void IncrRef() { CValue::IncrRef(); }
	virtual void DecrRef() { CValue::DecrRef(); }

	//get metadata from object 
	virtual IMetaObjectRecordData* GetMetaObject() const = 0;

	//get unique identifier 
	virtual CUniqueKey GetGuid() const = 0;

	//get frame
	virtual CValueForm* GetForm() const;

	//support show 
	virtual void ShowFormValue(const wxString& formName = wxEmptyString, IValueFrame* owner = NULL) = 0;
	virtual CValueForm* GetFormValue(const wxString& formName = wxEmptyString, IValueFrame* owner = NULL) = 0;

	//default showing
	virtual void ShowValue() override {
		ShowFormValue();
	}

	//save modify 
	virtual bool SaveModify() override { return true; }

	//Get ref class 
	virtual CLASS_ID GetClassType() const;

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

	//operator 
	virtual operator CValue() const { return this; }

protected:

	virtual void PrepareEmptyObject();

protected:

	friend class IMetaObjectRecordData;

protected:

	CMethods* m_methods;
};

//Object with reference type 
class IRecordDataObjectRef : public IRecordDataObject {
	wxDECLARE_ABSTRACT_CLASS(IRecordDataObjectRef);
protected:

	// code generator
	class CCodeGenerator {
		IMetaObjectRecordDataRef* m_metaObject;
		IMetaAttributeObject* m_metaAttribute;
	public:
		CValue GenerateCode() const;
		meta_identifier_t GetMetaID() const {
			return m_metaAttribute->GetMetaID();
		}
		CCodeGenerator(IMetaObjectRecordDataRef* metaObject, IMetaAttributeObject* attribute) : m_metaObject(metaObject), m_metaAttribute(attribute) {}
	};

	CCodeGenerator* m_codeGenerator;

public:

	virtual bool InitializeObject(const IRecordDataObject* source = NULL);

	IRecordDataObjectRef();
	IRecordDataObjectRef(IMetaObjectRecordDataRef* metaObject);
	IRecordDataObjectRef(IMetaObjectRecordDataRef* metaObject, const Guid& guid);
	IRecordDataObjectRef(const IRecordDataObjectRef& source);
	virtual ~IRecordDataObjectRef();

	//Get ref class 
	virtual CLASS_ID GetClassType() const;

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

	//check is empty
	virtual inline bool IsEmpty() const override {
		return !m_objGuid.isValid();
	}

	//support source data 
	virtual CSourceExplorer GetSourceExplorer() const;
	virtual bool GetTable(IValueTable*& tableValue, const meta_identifier_t& id);

	//get metadata from object 
	virtual IMetaObjectRecordData* GetMetaObject() const {
		return m_metaObject;
	};

	//set modify 
	virtual void Modify(bool mod);

	//support source set/get data 
	virtual void SetValueByMetaID(const meta_identifier_t& id, const CValue& cVal);
	virtual CValue GetValueByMetaID(const meta_identifier_t& id) const;

	//get unique identifier 
	virtual CUniqueKey GetGuid() const {
		return m_objGuid;
	}

	//copy new object
	virtual IRecordDataObject* CopyObjectValue() = 0;

	//is new object?
	virtual bool IsNewObject() const {
		return m_newObject;
	}

	//get reference
	virtual CReferenceDataObject* GetReference() const;

protected:

	virtual bool ReadData();
	virtual bool SaveData();
	virtual bool DeleteData();

protected:

	virtual void PrepareEmptyObject();
	virtual void PrepareEmptyObject(const IRecordDataObject* source);

protected:

	friend class CTabularSectionDataObjectRef;

	bool m_objModified;
	IMetaObjectRecordDataRef* m_metaObject;
	reference_t* m_reference_impl;
};
#pragma endregion

//Object with register type 
#pragma region registers 
class CRecordKeyObject : public CValue {
	wxDECLARE_ABSTRACT_CLASS(CRecordKeyObject);
public:
	CRecordKeyObject(IMetaObjectRegisterData* metaObject);
	virtual ~CRecordKeyObject();

	//standart override 
	virtual CMethods* GetPMethods() const;

	virtual void PrepareNames() const;
	virtual CValue Method(methodArg_t& aParams);

	virtual void SetAttribute(attributeArg_t& aParams, CValue& cVal);
	virtual CValue GetAttribute(attributeArg_t& aParams);

	//check is empty
	virtual inline bool IsEmpty() const override;

	//Get unique key 
	CUniquePairKey GetUniqueKey() {
		return CUniquePairKey(m_metaObject, m_aKeyValues);
	}

	//get metadata from object 
	virtual IMetaObjectRegisterData* GetMetaObject() const {
		return m_metaObject;
	};

	//Get ref class 
	virtual CLASS_ID GetClassType() const;

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

protected:
	IMetaObjectRegisterData* m_metaObject;
	CMethods* m_methods;

	std::map<meta_identifier_t, CValue> m_aKeyValues;
};

class IRecordSetObject : public IValueTable, public IModuleInfo {
	wxDECLARE_ABSTRACT_CLASS(IRecordSetObject);
public:

	virtual IValueTableColumnCollection* GetColumns() const {
		return m_dataColumnCollection;
	}

	virtual IValueTableReturnLine* GetRowAt(unsigned int line)
	{
		if (line > m_aObjectValues.size())
			return NULL;
		return new CRecordSetRegisterReturnLine(this, line);
	}

	class CRecordSetRegisterColumnCollection : public IValueTable::IValueTableColumnCollection {
		wxDECLARE_DYNAMIC_CLASS(CRecordSetRegisterColumnCollection);
	public:

		class CValueRecordSetRegisterColumnInfo : public IValueTable::IValueTableColumnCollection::IValueTableColumnInfo {
			wxDECLARE_DYNAMIC_CLASS(CValueRecordSetRegisterColumnInfo);
			IMetaAttributeObject* m_metaAttribute;
		public:

			virtual unsigned int GetColumnID() const { return m_metaAttribute->GetMetaID(); }
			virtual wxString GetColumnName() const { return m_metaAttribute->GetName(); }
			virtual wxString GetColumnCaption() const { return m_metaAttribute->GetSynonym(); }
			virtual CValueTypeDescription* GetColumnTypes() const { return m_metaAttribute->GetValueTypeDescription(); }
			virtual int GetColumnWidth() const { return 0; }

			CValueRecordSetRegisterColumnInfo();
			CValueRecordSetRegisterColumnInfo(IMetaAttributeObject* metaAttribute);
			virtual ~CValueRecordSetRegisterColumnInfo();

			virtual wxString GetTypeString() const {
				return wxT("recordSetRegisterColumnInfo");
			}

			virtual wxString GetString() const {
				return wxT("recordSetRegisterColumnInfo");
			}

			friend CRecordSetRegisterColumnCollection;
		};

	public:

		CRecordSetRegisterColumnCollection();
		CRecordSetRegisterColumnCollection(IRecordSetObject* ownerTable);
		virtual ~CRecordSetRegisterColumnCollection();

		virtual CValueTypeDescription* GetColumnTypes(unsigned int col) const
		{
			CValueRecordSetRegisterColumnInfo* columnInfo = m_aColumnInfo.at(col);
			wxASSERT(columnInfo);
			return columnInfo->GetColumnTypes();
		}

		virtual IValueTableColumnInfo* GetColumnInfo(unsigned int idx) const
		{
			if (m_aColumnInfo.size() < idx)
				return NULL;

			auto foundedIt = m_aColumnInfo.begin();
			std::advance(foundedIt, idx);
			return foundedIt->second;
		}

		virtual unsigned int GetColumnCount() const {
			IMetaObjectWrapperData* metaTable = m_ownerTable->GetMetaObject();
			wxASSERT(metaTable);
			auto attributes =
				metaTable->GetGenericAttributes();
			return attributes.size();
		}

		virtual wxString GetTypeString() const {
			return wxT("recordSetRegisterColumn");
		}

		virtual wxString GetString() const {
			return wxT("recordSetRegisterColumn");
		}

		//array support 
		virtual CValue GetAt(const CValue& cKey);
		virtual void SetAt(const CValue& cKey, CValue& cVal);

		//������ � ����������� 
		virtual bool HasIterator() const { return true; }
		virtual CValue GetItAt(unsigned int idx)
		{
			auto itFounded = m_aColumnInfo.begin();
			std::advance(itFounded, idx);
			return itFounded->second;
		}

		virtual unsigned int GetItSize() const { return GetColumnCount(); }

		friend class IRecordSetObject;

	protected:

		IRecordSetObject* m_ownerTable;
		CMethods* m_methods;

		std::map<meta_identifier_t, CValueRecordSetRegisterColumnInfo*> m_aColumnInfo;
	};

	class CRecordSetRegisterReturnLine : public IValueTableReturnLine {
		wxDECLARE_DYNAMIC_CLASS(CRecordSetRegisterReturnLine);
	public:
		IRecordSetObject* m_ownerTable; int m_lineTable;
	public:

		virtual unsigned int GetLineTable() const {
			return m_lineTable;
		}

		virtual IValueTable* GetOwnerTable() const {
			return m_ownerTable;
		}

		CRecordSetRegisterReturnLine();
		CRecordSetRegisterReturnLine(IRecordSetObject* ownerTable, int line);
		virtual ~CRecordSetRegisterReturnLine();

		//set meta/get meta
		virtual void SetValueByMetaID(const meta_identifier_t& id, const CValue& cVal);
		virtual CValue GetValueByMetaID(const meta_identifier_t& id) const;

		virtual CMethods* GetPMethods() const { PrepareNames(); return m_methods; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
		virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������

		virtual void SetAttribute(attributeArg_t& aParams, CValue& cVal); //��������� ��������
		virtual CValue GetAttribute(attributeArg_t& aParams); //�������� ��������

		virtual wxString GetTypeString() const {
			return wxT("recordSetRegisterRow");
		}

		virtual wxString GetString() const {
			return wxT("recordSetRegisterRow");
		}

		friend class IRecordSetObject;
	private:
		CMethods* m_methods;
	};

	class CRecordSetRegisterKeyValue : public CValue {
		wxDECLARE_DYNAMIC_CLASS(CRecordSetRegisterReturnLine);
	public:
		class CRecordSetRegisterKeyDescriptionValue : public CValue {
			wxDECLARE_DYNAMIC_CLASS(CRecordSetRegisterKeyDesriptionValue);
		public:

			CRecordSetRegisterKeyDescriptionValue(IRecordSetObject* recordSet = NULL, const meta_identifier_t& id = wxNOT_FOUND);
			virtual ~CRecordSetRegisterKeyDescriptionValue();

			virtual inline bool IsEmpty() const {
				return false;
			}

			//****************************************************************************
			//*                              Support methods                             *
			//****************************************************************************

			virtual CMethods* GetPMethods() const;                          // �������� ������ �� ����� �������� ������� ���� ��������� � �������
			virtual void PrepareNames() const;                             // ���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
			virtual CValue Method(methodArg_t& aParams);       // ����� ������

			virtual void SetAttribute(attributeArg_t& aParams, CValue& cVal);//��������� ��������
			virtual CValue GetAttribute(attributeArg_t& aParams);//�������� ��������

			virtual wxString GetTypeString() const {
				return wxT("recordSetRegisterKeyDescription");
			}

			virtual wxString GetString() const {
				return wxT("recordSetRegisterKeyDescription");
			}

		protected:
			IRecordSetObject* m_recordSet;
			CMethods* m_methods;

			meta_identifier_t m_metaId;
		};
	public:

		CRecordSetRegisterKeyValue(IRecordSetObject* recordSet = NULL);
		virtual ~CRecordSetRegisterKeyValue();

		virtual inline bool IsEmpty() const {
			return false;
		}

		//****************************************************************************
		//*                              Support methods                             *
		//****************************************************************************

		virtual CMethods* GetPMethods() const;                          // �������� ������ �� ����� �������� ������� ���� ��������� � �������
		virtual void PrepareNames() const;                             // ���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
		virtual CValue Method(methodArg_t& aParams);       // ����� ������

		virtual void SetAttribute(attributeArg_t& aParams, CValue& cVal);//��������� ��������
		virtual CValue GetAttribute(attributeArg_t& aParams);//�������� ��������

		virtual wxString GetTypeString() const {
			return wxT("recordSetRegisterKey");
		}

		virtual wxString GetString() const {
			return wxT("recordSetRegisterKey");
		}

	protected:
		IRecordSetObject* m_recordSet;
		CMethods* m_methods;
	};

	CRecordSetRegisterColumnCollection* m_dataColumnCollection;
	CRecordSetRegisterKeyValue* m_recordSetKeyValue;

public:

	virtual void CreateEmptyKey();
	virtual bool InitializeObject(const IRecordSetObject* source = NULL, bool newRecord = false);

	bool FindKeyValue(const meta_identifier_t& id) const;
	void SetKeyValue(const meta_identifier_t& id, const CValue& cValue);
	CValue GetKeyValue(const meta_identifier_t& id) const;
	void EraseKeyValue(const meta_identifier_t& id);

	IRecordSetObject(IMetaObjectRegisterData* metaObject);
	IRecordSetObject(IMetaObjectRegisterData* metaObject, const CUniquePairKey& uniqueKey);
	IRecordSetObject(const IRecordSetObject& source);
	virtual ~IRecordSetObject();

	//copy new object
	virtual IRecordSetObject* CopyRegisterValue() = 0;

	bool Selected() const {
		return m_selected;
	}

	void Read() {
		ReadData();
	}

	//counter
	virtual void IncrRef() {
		CValue::IncrRef();
	}

	virtual void DecrRef() {
		CValue::DecrRef();
	}

	virtual inline bool IsEmpty() const;

	//set modify 
	virtual void Modify(bool mod);

	//is modified 
	virtual bool IsModified() const;

	//get metadata from object 
	virtual IMetaObjectRegisterData* GetMetaObject() const {
		return m_metaObject;
	};

	virtual bool AutoCreateColumns() const { return false; }
	virtual bool EditableLine(const wxDataViewItem& item, unsigned int col) const {
		return false;
	}

	virtual void ActivateItem(CValueForm* formOwner,
		const wxDataViewItem& item, unsigned int col) {
		IValueTable::RowValueStartEdit(item, col);
	}

	//support def. table (in runtime)
	void Prepend(const wxString& text);
	void DeleteItem(const wxDataViewItem& item);
	void DeleteItems(const wxDataViewItemArray& items);

	// implementation of base class virtuals to define model
	virtual unsigned int GetColumnCount() const override;
	virtual wxString GetColumnType(unsigned int col) const override;

	virtual void GetValueByRow(wxVariant& variant,
		unsigned int row, unsigned int col) const override;

	virtual bool GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr& attr) const override;

	virtual bool SetValueByRow(const wxVariant& variant,
		unsigned int row, unsigned int col) override;

	//append new row
	virtual unsigned int AppenRow(unsigned int before = 0);

	virtual bool LoadDataFromTable(IValueTable* srcTable);
	virtual IValueTable* SaveDataToTable() const;

	//default methods
	virtual bool WriteRecordSet(bool replace = true, bool clearTable = true) = 0;
	virtual bool DeleteRecordSet() = 0;

	//array
	virtual CValue GetAt(const CValue& cKey);

	//Get ref class 
	virtual CLASS_ID GetClassType() const;

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;
	//operator 
	virtual operator CValue() const {
		return this;
	}

	//������ � ����������� 
	virtual bool HasIterator() const override { return true; }

	virtual CValue GetItEmpty() override {
		return new CRecordSetRegisterReturnLine(this, wxNOT_FOUND);
	}

	virtual CValue GetItAt(unsigned int idx) override {
		if (idx > m_aObjectValues.size())
			return CValue();

		return new CRecordSetRegisterReturnLine(this, idx);
	}

	virtual unsigned int GetItSize() const override {
		return m_aObjectValues.size();
	}

protected:

	virtual bool ExistData();
	virtual bool ExistData(number_t& lastNum); //for records
	virtual bool ReadData();
	virtual bool SaveData(bool replace = true, bool clearTable = true);
	virtual bool DeleteData();

	////////////////////////////////////////

	virtual bool SaveVirtualTable() {
		return true;
	}

	virtual bool DeleteVirtualTable() {
		return true;
	}

protected:

	//set meta/get meta
	virtual void SetValueByMetaID(long line, const meta_identifier_t& id, const CValue& cVal);
	virtual CValue GetValueByMetaID(long line, const meta_identifier_t& id) const;

protected:

	friend class IRecordManagerObject;

	bool m_objModified;
	bool m_selected;

	std::map<meta_identifier_t, CValue> m_aKeyValues;
	std::vector<std::map<meta_identifier_t, CValue>> m_aObjectValues;

	IMetaObjectRegisterData* m_metaObject;
	CMethods* m_methods;
};

class IRecordManagerObject : public CValue,
	public ISourceDataObject, public IActionSource {
	wxDECLARE_ABSTRACT_CLASS(IRecordManagerObject);
public:

	virtual void CreateEmptyKey();
	virtual bool InitializeObject(const IRecordManagerObject* source = NULL, bool newRecord = false);

	IRecordManagerObject(IMetaObjectRegisterData* metaObject);
	IRecordManagerObject(IMetaObjectRegisterData* metaObject, const CUniquePairKey& uniqueKey);
	IRecordManagerObject(const IRecordManagerObject& source);
	virtual ~IRecordManagerObject();

	//copy new object
	virtual IRecordManagerObject* CopyRegisterValue() = 0;

	//counter
	virtual void IncrRef() {
		CValue::IncrRef();
	}

	virtual void DecrRef() {
		CValue::DecrRef();
	}

	//get unique identifier 
	virtual CUniqueKey GetGuid() const {
		return m_objGuid;
	};

	//save modify 
	virtual bool SaveModify() override {
		return WriteRegister();
	}

	//default methods
	virtual CValue CopyRegister() = 0;
	virtual bool WriteRegister(bool replace = true) = 0;
	virtual bool DeleteRegister() = 0;

	//check is empty
	virtual inline bool IsEmpty() const override;

	//set modify 
	virtual void Modify(bool mod);

	//is modified 
	virtual bool IsModified() const;

	//support source set/get data 
	virtual void SetValueByMetaID(const meta_identifier_t& id, const CValue& cVal);
	virtual CValue GetValueByMetaID(const meta_identifier_t& id) const;

	//support source data 
	virtual CSourceExplorer GetSourceExplorer() const;
	virtual bool GetTable(IValueTable*& tableValue, const meta_identifier_t& id);

	//get metadata from object 
	virtual IMetaObjectRegisterData* GetMetaObject() const {
		return m_metaObject;
	};

	//get frame
	virtual CValueForm* GetForm() const;

	//support show 
	virtual void ShowFormValue(const wxString& formName = wxEmptyString, IValueFrame* owner = NULL) = 0;
	virtual CValueForm* GetFormValue(const wxString& formName = wxEmptyString, IValueFrame* owner = NULL) = 0;

	//default showing
	virtual void ShowValue() override {
		ShowFormValue();
	}

	//Get ref class 
	virtual CLASS_ID GetClassType() const;

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

	//operator 
	virtual operator CValue() const {
		return this;
	}

protected:

	virtual void PrepareEmptyObject(const IRecordManagerObject* source);

protected:

	virtual bool ExistData();
	virtual bool ReadData();
	virtual bool SaveData(bool replace = true);
	virtual bool DeleteData();

protected:

	CUniquePairKey m_objGuid;

	IMetaObjectRegisterData* m_metaObject;
	IRecordSetObject* m_recordSet;

	IValueTable::IValueTableReturnLine* m_recordLine;

	CMethods* m_methods;
};
#pragma endregion

#endif 