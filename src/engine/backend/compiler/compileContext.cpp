#include "compileContext.h"
#include "compileCode.h"



CFunction::CFunction(const wxString& funcName, CCompileContext* compileContext) : m_strName(funcName), m_pContext(compileContext),
m_bExport(false), m_bContext(false), m_lVarCount(0), m_nStart(0), m_nFinish(0), m_nNumberLine(0) {
	m_realRetValue.m_nArray = 0;
	m_realRetValue.m_nIndex = 0;
};

CFunction::~CFunction() {
	if (m_pContext != nullptr) {
		wxDELETE(m_pContext);
	};
};

////////////////////////////////////////////////////////////////////////
// CCompileContext CCompileContext CCompileContext CCompileContext//
////////////////////////////////////////////////////////////////////////

/**
 * ��������� ����� ���������� � ������
 * ���������� ����������� ���������� � ���� CParamUnit
 */
CParamUnit CCompileContext::AddVariable(const wxString& strVarName,
	const wxString& typeVar, bool exportVar, bool contextVar, bool tempVar)
{
	CVariable* foundedVariable = nullptr;
	if (FindVariable(strVarName, foundedVariable)) { //���� ���������� + ��������� ���������� = ������
		m_compileModule->SetError(ERROR_IDENTIFIER_DUPLICATE, strVarName);
	}

	unsigned int nCountVar = m_cVariables.size();

	CVariable cCurrentVariable;
	cCurrentVariable.m_strName = stringUtils::MakeUpper(strVarName);
	cCurrentVariable.m_strRealName = strVarName;
	cCurrentVariable.m_bExport = exportVar;
	cCurrentVariable.m_bContext = contextVar;
	cCurrentVariable.m_bTempVar = tempVar;
	cCurrentVariable.m_strType = typeVar;
	cCurrentVariable.m_nNumber = nCountVar;

	m_cVariables.insert_or_assign(
		stringUtils::MakeUpper(strVarName), cCurrentVariable
	);

	CParamUnit cRet;
	cRet.m_nArray = 0;
	cRet.m_strType = typeVar;
	cRet.m_nIndex = nCountVar;
	return cRet;
}

/**
 * ������� ���������� ����� ���������� �� ���������� �����
 * ����� ����������� ����������, ������� � �������� ��������� �� ���� ������������
 * ���� ��������� ���������� ���, �� ��������� ����� ����������� ����������
 */
CParamUnit CCompileContext::GetVariable(const wxString& strVarName, bool bFindInParent, bool bCheckError, bool contextVar, bool tempVar)
{
	int nCanUseLocalInParent = m_nFindLocalInParent;
	CVariable* currentVariable = nullptr;
	if (!FindVariable(strVarName, currentVariable)) {
		//���� � ������������ ����������(�������)
		if (bFindInParent) {
			int nParentNumber = 0;
			CCompileContext* pCurContext = m_parentContext;
			while (pCurContext) {
				nParentNumber++;
				if (nParentNumber > MAX_OBJECTS_LEVEL) {
					//CSystemFunction::Message(pCurContext->m_compileModule->GetModuleName());
					if (nParentNumber > 2 * MAX_OBJECTS_LEVEL) {
						CBackendException::Error("Recursive call of modules!");
					}
				}
				if (pCurContext->FindVariable(strVarName, currentVariable)) { //�����
					//������� ��� ���������� ���������� ��� ��� (���� m_nFindLocalInParent=true, �� ����� ����� ��������� ���������� ��������)	
					if (nCanUseLocalInParent > 0 ||
						currentVariable->m_bExport) {
						CParamUnit variable;
						//���������� ����� ����������
						variable.m_nArray = nParentNumber;
						variable.m_nIndex = currentVariable->m_nNumber;
						variable.m_strType = currentVariable->m_strType;
						return variable;
					}
				}
				nCanUseLocalInParent--;
				pCurContext = pCurContext->m_parentContext;
			}
		}

		if (bCheckError) {
			m_compileModule->SetError(ERROR_VAR_NOT_FOUND, strVarName); //������� ��������� �� ������
		}

		//�� ���� ��� ���������� ���������� - ���������
		return AddVariable(strVarName, wxEmptyString, contextVar, contextVar, tempVar);
	}

	wxASSERT(currentVariable);

	CParamUnit variable;
	//���������� ����� � ��� ����������
	variable.m_nArray = 0;
	variable.m_nIndex = currentVariable->m_nNumber;
	variable.m_strType = currentVariable->m_strType;
	return variable;
}

/**
 * ����� ���������� � ��� �������
 * ���������� 1 - ���� ���������� �������
 */
bool CCompileContext::FindVariable(const wxString& strVarName, CVariable*& foundedVar, bool contextVar)
{
	auto it = std::find_if(m_cVariables.begin(), m_cVariables.end(),
		[strVarName](std::pair < const wxString, CVariable>& pair) {
			return stringUtils::CompareString(strVarName, pair.first); }
	);

	if (contextVar) {
		if (it != m_cVariables.end()) {
			foundedVar = &it->second;
			return it->second.m_bContext;
		}
		if (m_parentContext && m_parentContext->FindVariable(strVarName, foundedVar, contextVar))
			return true;
		foundedVar = nullptr;
		return false;
	}
	else if (it != m_cVariables.end()) {
		foundedVar = &it->second;
		return true;
	}
	foundedVar = nullptr;
	return false;
}

/**
 * ����� ���������� � ��� �������
 * ���������� 1 - ���� ���������� �������
 */
bool CCompileContext::FindFunction(const wxString& funcName, CFunction*& foundedFunc, bool contextVar) {
	auto it = std::find_if(m_cFunctions.begin(), m_cFunctions.end(),
		[funcName](std::pair < const wxString, CFunction*>& pair) {
			return stringUtils::CompareString(funcName, pair.first); }
	);
	if (contextVar) {
		if (it != m_cFunctions.end() && it->second) {
			foundedFunc = it->second;
			return it->second->m_bContext;
		}
		if (m_parentContext && m_parentContext->FindFunction(funcName, foundedFunc, contextVar))
			return true;
		foundedFunc = nullptr;
		return false;
	}
	else if (it != m_cFunctions.end() && it->second) {
		foundedFunc = it->second;
		return true;
	}
	foundedFunc = nullptr;
	return false;
}

/**
 * ����������� ���������� GOTO � �������
 */
void CCompileContext::DoLabels()
{
	wxASSERT(m_compileModule != nullptr);
	for (unsigned int i = 0; i < m_cLabels.size(); i++) {
		const wxString& strName = m_cLabels[i].m_strName;
		int oldLine = m_cLabels[i].m_nLine;
		//���� ����� ����� � ������ ����������� �����
		unsigned int currLine = m_cLabelsDef[strName];
		if (!currLine) {
			m_compileModule->m_nCurrentCompile = m_cLabels[i].m_nError;
			m_compileModule->SetError(ERROR_LABEL_DEFINE, strName); //��������� ������������� ����������� �����
		}
		//���������� ����� ��������:
		m_compileModule->m_cByteCode.m_aCodeList[oldLine].m_param1.m_nIndex = currLine + 1;
	}
};

CCompileContext::~CCompileContext() {
	for (auto it = m_cFunctions.begin(); it != m_cFunctions.end(); it++) {
		wxDELETE(it->second);
	}
	m_cFunctions.clear();
}
