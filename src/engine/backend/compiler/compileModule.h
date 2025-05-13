#ifndef _COMPILE_MODULE_H__
#define _COMPILE_MODULE_H__

#include "backend/backend.h"
#include "backend/compiler/compileCode.h"

//////////////////////////////////////////////////////////////////////
class BACKEND_API IMetaObjectModule;
//////////////////////////////////////////////////////////////////////

class BACKEND_API CCompileModule : public CCompileCode {
public:
	virtual bool Recompile(); // recompile current module from meta-object
	virtual bool Compile(); // compile current module from meta-object
public:

	CCompileModule(IMetaObjectModule* moduleObject, bool onlyFunction = false);
	virtual ~CCompileModule() {}

	virtual CCompileModule* GetParent() const { return dynamic_cast<CCompileModule*>(m_parent); }
	virtual IMetaObjectModule* GetModuleObject() const { return m_moduleObject; }

protected:
	IMetaObjectModule* m_moduleObject;
};

class BACKEND_API CCompileCommonModule : public CCompileModule {
public:
	CCompileCommonModule(IMetaObjectModule* moduleObject) :
		CCompileModule(moduleObject, true) {
	}
};

class BACKEND_API CCompileGlobalModule : public CCompileCommonModule {
public:
	CCompileGlobalModule(IMetaObjectModule* moduleObject) :
		CCompileCommonModule(moduleObject) {
	}
};

#endif