#ifndef __UNIQUE_KEY_H__
#define __UNIQUE_KEY_H__

#include "backend/compiler/value.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

class BACKEND_API IMetaObjectRegisterData;

////////////////////////////////////////////////////////////////////////////////////////////////////

//class keys 
class BACKEND_API CUniqueKey {
protected:
	enum class enUniqueData {
		enUniqueKey = 10,
		enUniqueGuid
	} m_uniqueData;

public:

	bool isValid() const;
	void reset();

	Guid GetGuid() const {
		return m_objGuid;
	}

	CUniqueKey();
	CUniqueKey(const Guid& guid);

	virtual bool IsOk() const {
		return m_metaObject != nullptr;
	}

	bool operator > (const CUniqueKey& other) const;
	bool operator >= (const CUniqueKey& other) const;
	bool operator < (const CUniqueKey& other) const;
	bool operator <= (const CUniqueKey& other) const;

	// overload equality and inequality operator
	virtual bool operator==(const CUniqueKey& other) const;
	virtual bool operator!=(const CUniqueKey& other) const;

	virtual bool operator==(const Guid& other) const;
	virtual bool operator!=(const Guid& other) const;

	operator wxString() const {
		return GetGuid().str();
	}

	operator Guid() const {
		return GetGuid();
	}

	operator guid_t() const {
		return GetGuid();
	}

protected:

	CUniqueKey(enUniqueData uniqueData) : m_uniqueData(uniqueData) {}

protected:

	Guid m_objGuid;
	IMetaObjectRegisterData* m_metaObject;
	valueArray_t m_keyValues;
};

class BACKEND_API CUniquePairKey : public CUniqueKey {
public:

	CUniquePairKey(IMetaObjectRegisterData* metaObject = nullptr);
	CUniquePairKey(IMetaObjectRegisterData* metaObject, const valueArray_t& keyValues);

	bool IsOk() const {
		return m_metaObject != nullptr && m_keyValues.size() > 0;
	}

	void SetKeyPair(IMetaObjectRegisterData* metaObject,
		valueArray_t& keys) {
		m_metaObject = metaObject; m_keyValues = keys;
	}

	bool FindKey(const meta_identifier_t& id) const {
		auto& it = m_keyValues.find(id);
		return it != m_keyValues.end();
	}

	CValue GetKey(const meta_identifier_t& id) const {
		auto& it = m_keyValues.find(id);
		if (it != m_keyValues.end())
			return it->second;
		return CValue();
	}

	operator valueArray_t() const {
		return m_keyValues;
	}
};

#define wxNullUniqueKey CUniqueKey()
#define wxNullUniquePairKey CUniquePairKey()

#endif // !_UNIQUE_KEY_H__