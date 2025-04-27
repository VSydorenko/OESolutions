////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko 
//	Description : module information for CValue 
////////////////////////////////////////////////////////////////////////////

#include "moduleInfo.h"

IModuleDataObject::IModuleDataObject() :
	m_compileModule(nullptr),
	m_procUnit(nullptr)
{
}

IModuleDataObject::IModuleDataObject(CCompileModule* compileCode) :
	m_compileModule(compileCode), m_procUnit(nullptr)
{
}

IModuleDataObject::~IModuleDataObject()
{
	wxDELETE(m_compileModule);
	wxDELETE(m_procUnit);
}

bool IModuleDataObject::ExecuteProc(const wxString& strMethodName, 
	CValue** paParams, const long lSizeArray) {
	if (m_procUnit != nullptr && m_procUnit->FindMethod(strMethodName) != wxNOT_FOUND) {
		return m_procUnit->CallAsProc(strMethodName, paParams, lSizeArray);
	}
	return false; 
}

bool IModuleDataObject::ExecuteFunc(const wxString& strMethodName,
	CValue& pvarRetValue, CValue** paParams, const long lSizeArray) {
	if (m_procUnit != nullptr && m_procUnit->FindMethod(strMethodName) != wxNOT_FOUND) {
		return m_procUnit->CallAsFunc(strMethodName, pvarRetValue, paParams, lSizeArray);;
	}
	return false;
}