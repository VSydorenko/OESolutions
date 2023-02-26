#include "codeEditorInterpreter.h"
#include "core/compiler/definition.h"
#include "core/compiler/systemObjects.h"
#include "utils/stringUtils.h"

//////////////////////////////////////////////////////////////////////
// CPrecompileContext CPrecompileContext CPrecompileContext CPrecompileContext  //
//////////////////////////////////////////////////////////////////////

/**
 * ����� ���������� � ��� �������
 * ���������� 1 - ���� ���������� �������
 */
bool CPrecompileContext::FindVariable(const wxString& sName, CValue& vContext, bool bContext)
{
	if (bContext)
	{
		auto itFounded = cVariables.find(StringUtils::MakeUpper(sName));
		if (itFounded != cVariables.end())
		{
			vContext = itFounded->second.m_valContext;
			return itFounded->second.bContext;
		}
		return false;
	}
	else
	{
		return cVariables.find(StringUtils::MakeUpper(sName)) != cVariables.end();
	}
}

/**
 * ����� ���������� � ��� �������
 * ���������� 1 - ���� ���������� �������
 */
bool CPrecompileContext::FindFunction(const wxString& sName, CValue& vContext, bool bContext)
{
	if (bContext)
	{
		auto itFounded = cFunctions.find(StringUtils::MakeUpper(sName));
		if (itFounded != cFunctions.end() && itFounded->second)
		{
			vContext = itFounded->second->m_valContext;
			return itFounded->second->bContext;
		}
		return false;
	}
	else
	{
		return cFunctions.find(StringUtils::MakeUpper(sName)) != cFunctions.end();
	}
}

void CPrecompileContext::RemoveVariable(const wxString& sName)
{
	auto itFounded = cVariables.find(StringUtils::MakeUpper(sName));
	if (itFounded != cVariables.end()) {
		cVariables.erase(itFounded);
	}
}

/**
 * ��������� ����� ���������� � ������
 * ���������� ����������� ���������� � ���� SParamValue
 */
SParamValue CPrecompileContext::AddVariable(const wxString& paramName, const wxString& paramType, bool bExport, bool bTempVar, const CValue& valVar)
{
	if (FindVariable(paramName)) //���� ���������� + ��������� ���������� = ������
		return SParamValue();

	CPrecompileVariable currentVar;
	currentVar.bContext = false;
	currentVar.sName = StringUtils::MakeUpper(paramName);
	currentVar.sRealName = paramName;
	currentVar.bExport = bExport;
	currentVar.bTempVar = bTempVar;
	currentVar.sType = paramType;
	currentVar.m_valObject = valVar;
	currentVar.nNumber = cVariables.size();

	cVariables[StringUtils::MakeUpper(paramName)] = currentVar;

	SParamValue paramValue;
	paramValue.m_paramType = paramType;
	paramValue.m_paramObject = valVar;
	return paramValue;
}

void CPrecompileContext::SetVariable(const wxString& varName, const CValue& valVar)
{
	if (FindVariable(varName)) {
		cVariables[StringUtils::MakeUpper(varName)].m_valObject = valVar; 
	}
}

/**
 * ������� ���������� ����� ���������� �� ���������� �����
 * ����� ����������� ����������, ������� � �������� ��������� �� ���� ������������
 * ���� ��������� ���������� ���, �� ��������� ����� ����������� ����������
 */
SParamValue CPrecompileContext::GetVariable(const wxString& sName, bool bFindInParent, bool bCheckError, const CValue& valVar)
{
	int nCanUseLocalInParent = nFindLocalInParent;
	SParamValue Variable;
	Variable.m_paramName = StringUtils::MakeUpper(sName);
	if (!FindVariable(sName)) {
		if (bFindInParent) {//���� � ������������ ����������(�������)
			int nParentNumber = 0;
			CPrecompileContext* pCurContext = pParent;
			CPrecompileContext* pNotParent = pStopParent;
			while (pCurContext) {
				nParentNumber++;
				if (nParentNumber > MAX_OBJECTS_LEVEL) {
					CSystemObjects::Message(pCurContext->pModule->GetModuleName());
					if (nParentNumber > 2 * MAX_OBJECTS_LEVEL)
						break;
				}

				if (pCurContext == pNotParent) {//������� ������ != ����������� �����������
					//��������� ��������� ��������
					pNotParent = pCurContext->pParent;
					if (pNotParent == pContinueParent)//������� �� ���������� - ��� ����������� ���������
						pNotParent = NULL;
				}
				else {
					if (pCurContext->FindVariable(sName)) {//�����
						CPrecompileVariable currentVar = pCurContext->cVariables[StringUtils::MakeUpper(sName)];
						//������� ��� ���������� ���������� ��� ��� (���� nFindLocalInParent=true, �� ����� ����� ��������� ���������� ��������)
						if (nCanUseLocalInParent > 0 || currentVar.bExport) {
							//���������� ����� ����������
							Variable.m_paramType = currentVar.sType;
							Variable.m_paramObject = currentVar.m_valObject;
							return Variable;
						}
					}
				}
				nCanUseLocalInParent--;
				pCurContext = pCurContext->pParent;
			}
		}

		if (bCheckError)
			return Variable;

		bool bTempVar = sName.Left(1) == "@";
		//�� ���� ��� ���������� ���������� - ���������
		AddVariable(sName, wxEmptyString, false, bTempVar, valVar);
	}

	//���������� ����� � ��� ����������
	CPrecompileVariable currentVar = cVariables[StringUtils::MakeUpper(sName)];
	Variable.m_paramType = currentVar.sType;
	Variable.m_paramObject = currentVar.m_valObject;
	return Variable;
}

CPrecompileContext::~CPrecompileContext()
{
	for (auto it = cFunctions.begin(); it != cFunctions.end(); it++) {
		CPrecompileFunction* pFunction = (CPrecompileFunction*)it->second;
		if (pFunction)
			delete pFunction;
	}
	cFunctions.clear();
}