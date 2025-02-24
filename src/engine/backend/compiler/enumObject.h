#ifndef _ENUM_OBJECT_H__
#define _ENUM_OBJECT_H__

#include "enum.h"

//default base class for all enumerations
template <typename valT>
class IEnumeration : public IEnumerationValue<valT> {
	std::map<valT, wxString> m_listEnumValue;
protected:

	template <typename valType>
	class CEnumerationVariant : public IEnumerationVariant<valType> {
		wxString m_name;
		wxString m_description;
		class_identifier_t m_clsid;
		valType m_value;
	public:

		CEnumerationVariant(valType value, const class_identifier_t& clsid) : IEnumerationVariant(),
			m_value(value), m_clsid(clsid) {
		}

		void CreateEnumeration(
			const wxString& name, const wxString& descr,
			valType value) {
			m_name = name;
			m_description = descr;
			m_value = value;
		}

		virtual valT GetEnumValue() const override {
			return m_value;
		}

		virtual void SetEnumValue(valT val) override {
			m_value = val;
		}

		virtual bool FindValue(const wxString& findData, std::vector<CValue>& listValue) const {
			value_ptr<IEnumeration<valType>> enumOwner = CValue::CreateAndConvertObjectRef<IEnumeration<valType>>(m_clsid);
			for (auto& e : enumOwner->m_listEnumValue) {
				if (e.second.Contains(findData)) {
					CEnumerationVariant<valType>* enumValue = new CEnumerationVariant<valType>(e.first, m_clsid);
					if (enumValue != nullptr) {
						enumValue->CreateEnumeration(
							enumOwner->GetEnumName(e.first),
							enumOwner->GetEnumDescription(e.first),
							e.first
						);
						listValue.push_back(enumValue);
					}
				}
			}
			std::sort(listValue.begin(), listValue.end(), [](const CValue& a, const CValue& b) { return a.GetString() < b.GetString(); });
			return listValue.size() > 0;
		}

		//operator '=='
		virtual inline bool CompareValueEQ(const CValue& cParam) const override {
			CEnumerationVariant<valType>* compareEnumeration = dynamic_cast<CEnumerationVariant<valType> *>(cParam.GetRef());
			if (compareEnumeration) return m_value == compareEnumeration->m_value;
			IEnumeration<valType>* compareEnumerationOwner = dynamic_cast<IEnumeration<valType> *>(cParam.GetRef());
			if (compareEnumerationOwner) return m_value == compareEnumerationOwner->GetEnumValue();
			return false;
		}

		//operator '!='
		virtual inline bool CompareValueNE(const CValue& cParam) const override {
			CEnumerationVariant<valType>* compareEnumeration = dynamic_cast<CEnumerationVariant<valType> *>(cParam.GetRef());
			if (compareEnumeration) return m_value != compareEnumeration->m_value;
			IEnumeration<valType>* compareEnumerationOwner = dynamic_cast<IEnumeration<valType> *>(cParam.GetRef());
			if (compareEnumerationOwner) return m_value != compareEnumerationOwner->GetEnumValue();
			return true;
		}

		//get type id
		virtual class_identifier_t GetClassType() const override {
			return m_clsid;
		}

		//check is empty
		virtual inline bool IsEmpty() const {
			return false;
		}

		//type info
		virtual wxString GetClassName() const {
			return CValue::GetNameObjectFromID(m_clsid);
		}

		//type conversion
		virtual wxString GetString() const override {
			return m_name;
		}

		virtual number_t GetNumber() const override {
			return m_value;
		}
	};

	virtual wxString GetEnumName(valT val) const {
		return m_listEnumValue.at(val);
	}

	virtual wxString GetEnumDescription(valT val) const {
		return wxEmptyString;
	}

	friend class PropertyOption;

private:

	inline void CreateEnumeration(valT val) {
		wxASSERT(m_listEnumValue.find(val) != m_listEnumValue.end());
		if (m_value != nullptr) {
			m_value->CreateEnumeration(
				GetEnumName(val),
				GetEnumDescription(val),
				val
			);
		}
	}

public:

	inline void AddEnumeration(valT val, const wxString& name, const wxString& descr = wxEmptyString) {
		wxASSERT(m_listEnumValue.find(val) == m_listEnumValue.end());
		m_listEnumValue.insert_or_assign(val, name);
		m_listEnumString.push_back(name);
	}

	IEnumeration() : IEnumerationValue(true), m_value(nullptr) {}
	IEnumeration(valT defValue) : IEnumerationValue(), m_value(nullptr) {}

	virtual ~IEnumeration() {
		if (m_value != nullptr)
			m_value->DecrRef();
	}

	virtual bool Init() {
		return true;
	}

	virtual bool Init(CValue** paParams, const long lSizeArray) {
		const valT& defValue = static_cast<valT>(paParams[0]->GetInteger());
		IEnumeration::InitializeEnumeration(defValue);
		return true;
	}

	virtual valT GetDefaultEnumValue() const {
		auto itEnums = m_listEnumValue.begin();
		std::advance(itEnums, 1);
		if (itEnums != m_listEnumValue.end())
			return itEnums->first;
		return valT();
	};

	virtual valT GetEnumValue() const override {
		if (m_value != nullptr) {
			return m_value->GetEnumValue();
		}
		return valT();
	}

	virtual void SetEnumValue(valT val) override {
		if (m_value != nullptr) {
			m_value->CreateEnumeration(
				GetEnumName(val),
				GetEnumDescription(val),
				val
			);
		}
	}

	//initialize enumeration 
	virtual void InitializeEnumeration() {
		PrepareNames();
	}

	virtual void InitializeEnumeration(valT defValue) {
		if (m_value != nullptr)
			m_value->DecrRef();
		m_value = new CEnumerationVariant(defValue, CValue::GetClassType());
		m_value->IncrRef();
		CreateEnumeration(defValue);
	}

	virtual CValue* GetEnumVariantValue() const {
		return m_value;
	}

	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal) override { //attribute value
		auto itEnums = m_listEnumValue.begin();
		std::advance(itEnums, lPropNum);
		if (itEnums != m_listEnumValue.end()) {
			CEnumerationVariant<valT>* enumValue =
				new CEnumerationVariant<valT>(itEnums->first, CValue::GetClassType());
			if (enumValue != nullptr) {
				enumValue->CreateEnumeration(
					GetEnumName(itEnums->first),
					GetEnumDescription(itEnums->first),
					itEnums->first
				);
				pvarPropVal = enumValue;
				return true;
			}
			return false;
		}
		return false;
	}

	virtual bool FindValue(const wxString& findData, std::vector<CValue>& listValue) const {
		for (auto& e : m_listEnumValue) {
			if (e.second.Contains(findData)) {
				CEnumerationVariant<valT>* enumValue = new CEnumerationVariant<valT>(e.first, CValue::GetClassType());
				if (enumValue != nullptr) {
					enumValue->CreateEnumeration(
						GetEnumName(e.first),
						GetEnumDescription(e.first),
						e.first
					);
					listValue.push_back(enumValue);
				}
			}
		}
		std::sort(listValue.begin(), listValue.end(), [](const CValue& a, const CValue& b) { return a.GetString() < b.GetString(); });
		return listValue.size() > 0;
	}

	//operator '=='
	virtual inline bool CompareValueEQ(const CValue& cParam) const override {
		if (m_value != nullptr) {
			return m_value->CompareValueEQ(cParam);
		}
		return CValue::CompareValueEQ(cParam);
	}

	//operator '!='
	virtual inline bool CompareValueNE(const CValue& cParam) const override {
		if (m_value != nullptr) {
			return m_value->CompareValueNE(cParam);
		}
		return CValue::CompareValueNE(cParam);
	}

	//check is empty
	virtual inline bool IsEmpty() const {
		return false;
	}

	//type info
	virtual wxString GetClassName() const final {
		return m_value ? m_value->GetClassName() :
			CValue::GetClassName();
	};

	//type conversion
	virtual wxString GetString() const final {
		return m_value ? m_value->GetString() :
			GetClassName();
	}

	virtual number_t GetNumber() const final {
		return m_value ? m_value->GetNumber() :
			wxNOT_FOUND;
	}

protected:

	CEnumerationVariant<valT>* m_value; //current enum value
};

#endif