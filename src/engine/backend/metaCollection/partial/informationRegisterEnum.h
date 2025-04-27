#ifndef _INFORMATION_REGISTER_ENUM_H__
#define _INFORMATION_REGISTER_ENUM_H__

enum eWriteRegisterMode {
	eIndependent,
	eSubordinateRecorder
};

enum ePeriodicity {
	eNonPeriodic,
	eWithinSecond,
	eWithinDay,
};

#pragma region enumeration
#include "backend/compiler/enumUnit.h"
class CValueEnumPeriodicity : public IEnumeration<ePeriodicity> {
	wxDECLARE_DYNAMIC_CLASS(CValueEnumPeriodicity);
public:
	CValueEnumPeriodicity() : IEnumeration() {}
	//CValueEnumPeriodicity(ePeriodicity periodicity) : IEnumeration(periodicity) {}

	virtual void CreateEnumeration() {
		AddEnumeration(ePeriodicity::eNonPeriodic, wxT("nonPeriodic"));
		AddEnumeration(ePeriodicity::eWithinSecond, wxT("withinSecond"));
		AddEnumeration(ePeriodicity::eWithinDay, wxT("withinDay"));
	}
};
class CValueEnumWriteRegisterMode : public IEnumeration<eWriteRegisterMode> {
	wxDECLARE_DYNAMIC_CLASS(CValueEnumWriteRegisterMode);
public:
	CValueEnumWriteRegisterMode() : IEnumeration() {}
	//CValueEnumWriteRegisterMode(eWriteRegisterMode mode) : IEnumeration(mode) {}

	virtual void CreateEnumeration() {
		AddEnumeration(eWriteRegisterMode::eIndependent, wxT("independent"));
		AddEnumeration(eWriteRegisterMode::eSubordinateRecorder, wxT("subordinateRecorder"));
	}
};
#pragma endregion 

#endif