#include "compileModule.h"
#include "backend/metaCollection/metaModuleObject.h"

#pragma warning(push)
#pragma warning(disable : 4018)

CCompileModule::CCompileModule(CMetaObjectModule* moduleObject, bool onlyFunction) :
	CCompileCode(moduleObject->GetFullName(), moduleObject->GetDocPath(), onlyFunction),
	m_moduleObject(moduleObject)
{
	InitializeCompileModule();

	m_cByteCode.m_strModuleName = m_moduleObject->GetFullName();

	m_strModuleName = m_moduleObject->GetFullName();
	m_strDocPath = m_moduleObject->GetDocPath();
	m_strFileName = m_moduleObject->GetFileName();

	Load(m_moduleObject->GetModuleText());

	//у родительских контекстов локальные переменные не ищем!
	m_cContext.m_nFindLocalInParent = 0;
}

/**
 * Compile
 * Назначение:
 * Трасляция и компиляция исходного кода в байт-код (объектный код)
 * Возвращаемое значение:
 * true,false
 */
bool CCompileModule::Compile()
{
	//clear functions & variables 
	Reset();

	if (m_parent != nullptr) {

		if (m_moduleObject &&
			m_moduleObject->IsGlobalModule()) {

			m_strModuleName = m_moduleObject->GetFullName();
			m_strDocPath = m_moduleObject->GetDocPath();
			m_strFileName = m_moduleObject->GetFileName();

			m_changeCode = false;

			Load(m_moduleObject->GetModuleText());

			return m_parent != nullptr ?
				m_parent->Compile() : true;
		}
	}

	//контекст самого модуля
	m_pContext = GetContext();

	//рекурсивно компилируем модули на случай каких-либо изменений 
	if (m_parent != nullptr) {

		std::stack<CCompileModule*> compileModule; bool callRecompile = false;

		CCompileModule* parentModule = GetParent();

		while (parentModule != nullptr) {
			if (parentModule->m_changeCode) callRecompile = true;
			if (callRecompile) compileModule.push(parentModule);
			parentModule = parentModule->GetParent();
		}

		while (!compileModule.empty()) {
			CCompileModule* compileCode = compileModule.top();
			if (!compileCode->Recompile()) return false;
			compileModule.pop();
		}
	}

	if (m_moduleObject != nullptr) {

		m_cByteCode.m_strModuleName = m_moduleObject->GetFullName();

		if (m_parent != nullptr) {
			m_cByteCode.m_parent = &m_parent->m_cByteCode;
			m_cContext.m_parentContext = &m_parent->m_cContext;
		}

		m_strModuleName = m_moduleObject->GetFullName();
		m_strDocPath = m_moduleObject->GetDocPath();
		m_strFileName = m_moduleObject->GetFileName();

		Load(m_moduleObject->GetModuleText());
	}

	//prepare lexem 
	if (!PrepareLexem()) {
		return false;
	}

	//подготовить контекстные переменные 
	PrepareModuleData();

	//Компиляция 
	if (CompileModule()) {
		m_changeCode = false;
		return true;
	}

	return false;
}

/**
 * Recompile
 * Назначение:
 * Трасляция и перекомпиляция текущего исходного кода в байт-код (объектный код)
 * Возвращаемое значение:
 * true,false
 */
bool CCompileModule::Recompile()
{
	//clear functions & variables 
	Reset();

	if (m_parent != nullptr) {
		if (m_moduleObject &&
			m_moduleObject->IsGlobalModule()) {

			m_strModuleName = m_moduleObject->GetFullName();
			m_strDocPath = m_moduleObject->GetDocPath();
			m_strFileName = m_moduleObject->GetFileName();

			m_changeCode = false;

			Load(m_moduleObject->GetModuleText());

			return m_parent != nullptr ?
				((CCompileModule*)m_parent)->Compile() : true;
		}
	}

	//контекст самого модуля
	m_pContext = GetContext();

	if (m_moduleObject) {
		m_cByteCode.m_strModuleName = m_moduleObject->GetFullName();

		if (m_parent) {
			m_cByteCode.m_parent = &m_parent->m_cByteCode;
			m_cContext.m_parentContext = &m_parent->m_cContext;
		}

		m_strModuleName = m_moduleObject->GetFullName();
		m_strDocPath = m_moduleObject->GetDocPath();
		m_strFileName = m_moduleObject->GetFileName();

		Load(m_moduleObject->GetModuleText());
	}

	//prepare lexem 
	if (!PrepareLexem()) {
		return false;
	}

	//подготовить контекстные переменные 
	PrepareModuleData();

	//Компиляция 
	if (CompileModule()) {
		m_changeCode = false;
		return true;
	}

	m_changeCode = true;
	return false;
}

#pragma warning(pop)