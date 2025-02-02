#ifndef _SELECTOR_H__
#define _SELECTOR_H__

#include "backend/metaCollection/partial/object.h"

class BACKEND_API ISelectorObject : public CValue {
public:

	ISelectorObject();
	virtual ~ISelectorObject();

	virtual bool Next() = 0;

	//is empty
	virtual inline bool IsEmpty() const {
		return false;
	}

	//get metaData from object 
	virtual IMetaObjectGenericData* GetMetaObject() const = 0;

	//Get ref class 
	virtual class_identifier_t GetClassType() const;

	//types 
	virtual wxString GetClassName() const;
	virtual wxString GetString() const;

protected:

	virtual void Reset() = 0;
	virtual bool Read() = 0;

protected:
	CMethodHelper* m_methodHelper;
};

class BACKEND_API CSelectorDataObject : public ISelectorObject,
	public IObjectValueInfo {
public:

	CSelectorDataObject(IMetaObjectRecordDataMutableRef* metaObject);

	virtual bool Next();
	virtual IRecordDataObjectRef* GetObject(const Guid& guid) const;

	//get metaData from object 
	virtual IMetaObjectRecordData* GetMetaObject() const {
		return m_metaObject;
	}

	virtual CMethodHelper* GetPMethods() const { //�������� ������ �� ����� �������� ������� ���� ��������� � �������
		//PrepareNames(); 
		return m_methodHelper;
	}
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//����� ������

	//attribute
	virtual bool SetPropVal(const long lPropNum, const CValue& varPropVal);        //��������� ��������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);                   //�������� ��������

protected:

	virtual void Reset();
	virtual bool Read();

protected:

	IMetaObjectRecordDataMutableRef* m_metaObject;
	std::vector<Guid> m_currentValues;
};

/////////////////////////////////////////////////////////////////////////////

class BACKEND_API CSelectorRegisterObject :
	public ISelectorObject {
public:
	CSelectorRegisterObject(IMetaObjectRegisterData* metaObject);

	virtual bool Next();
	virtual IRecordManagerObject* GetRecordManager(const valueArray_t& keyValues) const;

	//get metaData from object 
	virtual IMetaObjectRegisterData* GetMetaObject() const {
		return m_metaObject;
	}

	virtual CMethodHelper* GetPMethods() const { //�������� ������ �� ����� �������� ������� ���� ��������� � �������
		//PrepareNames(); 
		return m_methodHelper;
	}
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//����� ������

	//attribute
	virtual bool SetPropVal(const long lPropNum, const CValue& varPropVal);        //��������� ��������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);                   //�������� ��������

protected:

	virtual void Reset();
	virtual bool Read();

protected:

	IMetaObjectRegisterData* m_metaObject;

	valueArray_t m_keyValues;
	
	std::vector <valueArray_t> m_currentValues;
	std::map<
		valueArray_t,
		valueArray_t
	> m_objectValues;
};

#endif