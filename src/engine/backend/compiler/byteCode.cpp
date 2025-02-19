#include "byteCode.h"

////////////////////////////////////////////////////////////////////////
// CByteCode CByteCode CByteCode CByteCode						      //
////////////////////////////////////////////////////////////////////////

long CByteCode::FindMethod(const wxString& strMethodName) const
{
	auto foundedMethod = std::find_if(m_listFunc.begin(), m_listFunc.end(),
		[strMethodName](const std::pair<const wxString, CByteMethod>& pair) {
			return stringUtils::CompareString(strMethodName, pair.first);
		}
	);
	if (foundedMethod != m_listFunc.end())
		return (long)foundedMethod->second;
	return wxNOT_FOUND;
}

long CByteCode::FindExportMethod(const wxString& strMethodName) const
{
	auto foundedMethod = std::find_if(m_listExportFunc.begin(), m_listExportFunc.end(),
		[strMethodName](const std::pair<const wxString, CByteMethod>& pair) {
			return stringUtils::CompareString(strMethodName, pair.first);
		}
	);
	if (foundedMethod != m_listExportFunc.end())
		return (long)foundedMethod->second;
	return wxNOT_FOUND;
}

long CByteCode::FindFunction(const wxString& funcName) const
{
	auto foundedFunction = std::find_if(m_listFunc.begin(), m_listFunc.end(),
		[funcName](const std::pair<const wxString, CByteMethod>& pair) {
			return stringUtils::CompareString(funcName, pair.first) && pair.second.m_bCodeRet;
		}
	);
	if (foundedFunction != m_listFunc.end())
		return (long)foundedFunction->second;
	return wxNOT_FOUND;
}

long CByteCode::FindExportFunction(const wxString& funcName) const
{
	auto foundedFunction = std::find_if(m_listExportFunc.begin(), m_listExportFunc.end(),
		[funcName](const std::pair<const wxString, CByteMethod>& pair) {
			return stringUtils::CompareString(funcName, pair.first) && pair.second.m_bCodeRet;
		}
	);
	if (foundedFunction != m_listExportFunc.end())
		return (long)foundedFunction->second;
	return wxNOT_FOUND;
}

long CByteCode::FindProcedure(const wxString& procName) const
{
	auto foundedProcedure = std::find_if(m_listFunc.begin(), m_listFunc.end(),
		[procName](const std::pair<const wxString, CByteMethod>& pair) {
			return stringUtils::CompareString(procName, pair.first) && !pair.second.m_bCodeRet;
		}
	);
	if (foundedProcedure != m_listFunc.end())
		return (long)foundedProcedure->second;
	return wxNOT_FOUND;
}

long CByteCode::FindExportProcedure(const wxString& procName) const
{
	auto foundedProcedure = std::find_if(m_listExportFunc.begin(), m_listExportFunc.end(),
		[procName](const std::pair<const wxString, CByteMethod>& pair) {
			return stringUtils::CompareString(procName, pair.first) && !pair.second.m_bCodeRet;
		}
	);
	if (foundedProcedure != m_listExportFunc.end())
		return (long)foundedProcedure->second;
	return wxNOT_FOUND;
}

long CByteCode::FindVariable(const wxString& strVarName) const
{
	auto foundedFunction = std::find_if(m_listVar.begin(), m_listVar.end(),
		[strVarName](const std::pair<const wxString, long>& pair) {
			return stringUtils::CompareString(strVarName, pair.first);
		}
	);
	if (foundedFunction != m_listVar.end())
		return (long)foundedFunction->second;
	return wxNOT_FOUND;
}

long CByteCode::FindExportVariable(const wxString& strVarName) const
{
	auto foundedFunction = std::find_if(m_listExportVar.begin(), m_listExportVar.end(),
		[strVarName](const std::pair<const wxString, long>& pair) {
			return stringUtils::CompareString(strVarName, pair.first);
		}
	);
	if (foundedFunction != m_listExportVar.end())
		return (long)foundedFunction->second;
	return wxNOT_FOUND;
}