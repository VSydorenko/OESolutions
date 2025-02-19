#ifndef __PROC_CONTEXT__H__
#define __PROC_CONTEXT__H__

#include "byteCode.h"
#include "compileContext.h"

struct CRunContextSmall {

	long m_lStart, m_lParamCount;

	CValue m_cLocVars[MAX_STATIC_VAR] = {};
	CValue* m_pLocVars = nullptr;

	CValue* m_cRefLocVars[MAX_STATIC_VAR] = {};
	CValue** m_pRefLocVars = nullptr;

	long m_lVarCount;

public:

	CRunContextSmall(int varCount = wxNOT_FOUND) :
		m_lVarCount(0), m_lParamCount(0) {
		if (varCount >= 0) {
			SetLocalCount(varCount);
		}
	};

	~CRunContextSmall() {
		if (m_lVarCount > MAX_STATIC_VAR) {
			delete[]m_pLocVars;
			delete[]m_pRefLocVars;
		}
	}

	void SetLocalCount(const long varCount) {
		m_lVarCount = varCount;
		if (m_lVarCount > MAX_STATIC_VAR) {
			m_pLocVars = new CValue[m_lVarCount];
			m_pRefLocVars = new CValue * [m_lVarCount];
		}
		else {
			m_pLocVars = m_cLocVars;
			m_pRefLocVars = m_cRefLocVars;
		}
		for (long i = 0; i < m_lVarCount; i++) {
			m_pRefLocVars[i] = &m_pLocVars[i];
		}
	};

	long GetLocalCount() const { return m_lVarCount; }
};

class CProcUnit;

struct CRunContext {

	CProcUnit* m_procUnit = nullptr;
	CCompileContext* m_compileContext = nullptr;

	long m_lStart, m_lCurLine; //current executing bytecode line
	long m_lVarCount, m_lParamCount;

	CValue m_cLocVars[MAX_STATIC_VAR] = {};
	CValue* m_pLocVars = nullptr;

	CValue* m_cRefLocVars[MAX_STATIC_VAR] = {};
	CValue** m_pRefLocVars = nullptr;

	std::map<wxString, CProcUnit*> m_listEval;

public:

	CRunContext(int varCount = wxNOT_FOUND) :
		m_lVarCount(0), m_lParamCount(0), m_lStart(0), m_lCurLine(0) {
		if (varCount >= 0) {
			SetLocalCount(varCount);
		}
	};

	~CRunContext();
	CByteCode* CRunContext::GetByteCode() const;

	void SetLocalCount(const long varCount) {

		m_lVarCount = varCount;

		if (m_lVarCount > MAX_STATIC_VAR) {
			m_pLocVars = new CValue[m_lVarCount];
			m_pRefLocVars = new CValue * [m_lVarCount];
		}
		else {
			m_pLocVars = m_cLocVars;
			m_pRefLocVars = m_cRefLocVars;
		}

		for (long i = 0; i < m_lVarCount; i++) m_pRefLocVars[i] = &m_pLocVars[i];
	}

	long GetLocalCount() const { return m_lVarCount; }

	void SetProcUnit(CProcUnit* procUnit) { m_procUnit = procUnit; }
	CProcUnit* GetProcUnit() const { return m_procUnit; }
};

#endif // ! _PROC_CONTEXT__H__
