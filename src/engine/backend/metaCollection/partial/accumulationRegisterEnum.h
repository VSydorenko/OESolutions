#ifndef _ACCUMULATION_REGISTER_ENUM_H__
#define _ACCUMULATION_REGISTER_ENUM_H__

enum eRegisterType {
	eBalances,
	eTurnovers
};

enum eRecordType {
	eExpense,
	eReceipt
};

#pragma region enumeration
#include "backend/compiler/enumUnit.h"
class CValueEnumAccumulationRegisterType : public IEnumeration<eRegisterType> {
	wxDECLARE_DYNAMIC_CLASS(CValueEnumAccumulationRegisterType);
public:
	CValueEnumAccumulationRegisterType() : IEnumeration() {}
	//CValueEnumAccumulationRegisterType(eRegisterType mode) : IEnumeration(mode) {}

	virtual void CreateEnumeration() {
		AddEnumeration(eRegisterType::eBalances, wxT("balances"));
		AddEnumeration(eRegisterType::eTurnovers, wxT("turnovers"));
	}
};
class CValueEnumAccumulationRegisterRecordType : public IEnumeration<eRecordType> {
	wxDECLARE_DYNAMIC_CLASS(CValueEnumAccumulationRegisterRecordType);
public:
	static CValue CreateDefEnumValue() {
		return CValue::CreateEnumObject<CValueEnumAccumulationRegisterRecordType>(eRecordType::eExpense);
	}
	
	CValueEnumAccumulationRegisterRecordType() : IEnumeration() {}
	//CValueEnumAccumulationRegisterRecordType(eRecordType recordType) : IEnumeration(recordType) {}

	virtual void CreateEnumeration() {
		AddEnumeration(eExpense, wxT("expense"));
		AddEnumeration(eReceipt, wxT("receipt"));
	}
};
const class_identifier_t g_enumRecordTypeCLSID = string_to_clsid("EN_RETP");
#pragma endregion 

#endif