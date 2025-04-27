#ifndef __METAOBJECT_METADATA_ENUM_H__
#define __METAOBJECT_METADATA_ENUM_H__

#include "backend/compiler/enumUnit.h"
class CValueEnumVersion : public IEnumeration<eProgramVersion> {
	wxDECLARE_DYNAMIC_CLASS(CValueEnumVersion);
public:
	
	CValueEnumVersion() : IEnumeration() {}
	//CValueEnumVersion(eProgramVersion v) : IEnumeration(v) {}

	virtual void CreateEnumeration() {
		AddEnumeration(version_oes_1_0_0, wxT("oes_1_0_0"));
		AddEnumeration(version_oes_last, wxT("oes_last"));
	};
};

#endif