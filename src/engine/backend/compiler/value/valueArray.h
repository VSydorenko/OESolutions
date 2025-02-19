#ifndef _VALUEARRAY_H__
#define _VALUEARRAY_H__

#include "value.h"

//Array support
class BACKEND_API CValueArray : public CValue {
	wxDECLARE_DYNAMIC_CLASS(CValueArray);
private:
	std::vector <CValue> m_arrValues;
private:

	enum Func {
		enAdd = 0,
		enInsert,
		enCount,
		enFind,
		enClear,
		enGet,
		enSet,
		enRemove
	};

	inline void CheckIndex(unsigned int index) const;

public:

	CValueArray() :
		CValue(eValueTypes::TYPE_VALUE) {
	}

	CValueArray(const std::vector <CValue>& arr) :
		CValue(eValueTypes::TYPE_VALUE, true), m_arrValues(arr) {
	}

	virtual ~CValueArray() {
		Clear();
	}

	virtual bool Init();
	virtual bool Init(CValue** paParams, const long lSizeArray);

	//check is empty
	virtual inline bool IsEmpty() const {
		return m_arrValues.empty();
	}

public:

	//Attribute -> String key
	//working with array as an aggregate object
	static CMethodHelper m_methodHelper;

	virtual CMethodHelper* GetPMethods() const { // get a reference to the class helper for parsing attribute and method names
		return &m_methodHelper;
	}
	virtual void PrepareNames() const;                         // this method is automatically called to initialize attribute and method names.
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);       //method call

	//Расширенные методы
	void Add(const CValue& varValue) {
		m_arrValues.push_back(varValue);
	}

	void Insert(unsigned int index, const CValue& varValue) {
		CheckIndex(index);
		m_arrValues.insert(m_arrValues.begin() + index, varValue);
	}

	unsigned int Count() const {
		return m_arrValues.size();
	}

	CValue Find(const CValue& varValue) {
		auto& it = std::find(m_arrValues.begin(), m_arrValues.end(), varValue);
		if (it != m_arrValues.end())
			return std::distance(m_arrValues.begin(), it);
		return CValue();
	}

	void Remove(unsigned int index) {
		CheckIndex(index);
		auto& it = std::find(m_arrValues.begin(), m_arrValues.end(), index);
		if (it != m_arrValues.end())
			m_arrValues.erase(it);
	}

	void Clear() {
		m_arrValues.clear();
	}

	//array support 
	virtual bool SetAt(const CValue& varKeyValue, const CValue& varValue);
	virtual bool GetAt(const CValue& varKeyValue, CValue& pvarValue);

	//Working with iterators
	virtual bool HasIterator() const {
		return true;
	}
	virtual CValue GetIteratorAt(unsigned int idx) {
		return m_arrValues[idx];
	}
	virtual unsigned int GetIteratorCount() const {
		return Count();
	}
};

#endif