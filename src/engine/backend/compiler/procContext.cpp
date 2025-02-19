#include "procContext.h"
#include "compileCode.h"
#include "procUnit.h"

//*************************************************************************************************
//*                                        RunContext                                             *
//*************************************************************************************************

CRunContext::~CRunContext() {
	if (m_lVarCount > MAX_STATIC_VAR) {
		wxDELETEA(m_pLocVars);
		wxDELETEA(m_pRefLocVars);
	}
	// delete m_listEval
	for (auto& it : m_listEval) {
		CProcUnit*& procUnit(it.second);
		if (procUnit != nullptr) {
			wxDELETE(procUnit->m_pByteCode->m_compileModule);
			wxDELETE(procUnit);
		}
	}
}

CByteCode* CRunContext::GetByteCode() const {
	return m_procUnit ?
		m_procUnit->GetByteCode() : nullptr;
}