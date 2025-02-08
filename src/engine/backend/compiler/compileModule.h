#ifndef _COMPILE_MODULE_H__
#define _COMPILE_MODULE_H__

#include "backend/backend.h"
#include "backend/compiler/compileCode.h"

//////////////////////////////////////////////////////////////////////
class BACKEND_API CMetaObjectModule;
//////////////////////////////////////////////////////////////////////

class BACKEND_API CCompileModule : public CCompileCode {
public:
	virtual bool Recompile(); //ѕерекомпил¤ци¤ текущего модул¤ из мета-объекта
	virtual bool Compile(); //ѕерекомпил¤ци¤ текущего модул¤ из мета-объекта
public:

	CCompileModule(CMetaObjectModule* moduleObject, bool onlyFunction = false);
	virtual ~CCompileModule() {}

	virtual CCompileModule* GetParent() const { return dynamic_cast<CCompileModule*>(m_parent); }
	virtual CMetaObjectModule* GetModuleObject() const { return m_moduleObject; }

protected:
	CMetaObjectModule* m_moduleObject;
};

class BACKEND_API CCompileCommonModule : public CCompileModule {
public:
	CCompileCommonModule(CMetaObjectModule* moduleObject) :
		CCompileModule(moduleObject, true) {
	}
};

class BACKEND_API CCompileGlobalModule : public CCompileCommonModule {
public:
	CCompileGlobalModule(CMetaObjectModule* moduleObject) :
		CCompileCommonModule(moduleObject) {
	}
};

#endif