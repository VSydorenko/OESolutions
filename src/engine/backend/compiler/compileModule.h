#ifndef _COMPILE_MODULE_H__
#define _COMPILE_MODULE_H__

#include "backend/backend.h"
#include "backend/compiler/compileCode.h"

//////////////////////////////////////////////////////////////////////
class BACKEND_API CMetaObjectModule;
//////////////////////////////////////////////////////////////////////

class BACKEND_API CCompileModule : public CCompileCode {
	bool Recompile(); //������������ �������� ����� �� ����-�������
public: bool Compile(); //������������ �������� ����� �� ����-�������
public:

	CCompileModule(CMetaObjectModule* moduleObject, bool onlyFunction = false);
	virtual ~CCompileModule() {}

	virtual CMetaObjectModule* GetModuleObject() const;
	virtual CCompileModule* GetParent() const;

protected:
	CMetaObjectModule* m_moduleObject;
};

class BACKEND_API CCompileCommonModule : public CCompileModule {
public:
	CCompileCommonModule(CMetaObjectModule* moduleObject) :
		CCompileModule(moduleObject, false) {
	}
};

class BACKEND_API CCompileGlobalModule : public CCompileCommonModule {
public:
	CCompileGlobalModule(CMetaObjectModule* moduleObject) :
		CCompileCommonModule(moduleObject) {
	}
};

#endif