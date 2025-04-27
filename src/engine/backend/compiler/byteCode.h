#ifndef __BYTE_CODE_H__
#define __BYTE_CODE_H__

#include "backend/compiler/value.h"

struct CParamUnit {

	wxLongLong_t m_numArray;
	wxLongLong_t m_numIndex;
	wxString	 m_strName;			//variable name 
	wxString	 m_strType;			//variable type in English notation (in case of explicit typing)

public:
	CParamUnit() : m_numArray(0), m_numIndex(0) {}
};

//storing one program step
struct CByteUnit {

	short		 m_numOper;			//instruction code
	unsigned int m_numString;		//source text number (for error output)
	unsigned int m_numLine;			//source line number (for breakpoints)

	//parameters for instructions:
	CParamUnit	 m_param1;
	CParamUnit	 m_param2;
	CParamUnit	 m_param3;
	CParamUnit	 m_param4;			 // - used for optimization

	wxString	 m_strModuleName;	 // module name (since it is possible to include connections from different modules)
	wxString	 m_strDocPath; 	 	 // unique path to the document
	wxString	 m_strFileName; 	 // path to the file (if external processing)

public:

	CByteUnit() : m_param1(), m_param2(), m_param3(), m_param4(),
		m_numOper(0), m_numString(0), m_numLine(0) {
	}
};

class BACKEND_API CCompileCode;

struct CByteCode {

	struct CByteMethod {
		long m_lCodeParamCount;
		long m_lCodeLine;
		bool m_bCodeRet;
	public:
		operator long() const {
			return m_lCodeLine;
		}
	};

	//Attributes:
	CByteCode* m_parent; //Parent bytecode (for checking)
	bool m_bCompile; //indication of successful compilation
	CCompileCode* m_compileModule;
	long m_lVarCount; // number of local variables in the module
	long m_lStartModule; // beginning of the module start position
	wxString m_strModuleName;//name of the executable module to which the bytecode belongs

	//list of external and context variables
	std::vector<CValue*> m_listExternValue;
	std::vector <CByteUnit> m_listCode;//executable code of the module
	std::vector <CValue> m_listConst;//list of module constants

	std::map<wxString, long> m_listVar; //list of module variables
	std::map<wxString, CByteMethod> m_listFunc; //list of module functions and procedures
	std::map<wxString, long> m_listExportVar; //list of module export variables
	std::map<wxString, CByteMethod> m_listExportFunc; //list of module export functions and procedures

public:

	CByteCode() {
		Reset();
	};

	void SetModule(CCompileCode* compileCode) {
		m_compileModule = compileCode;
	}

	long FindMethod(const wxString& strMethodName) const;
	long FindExportMethod(const wxString& strMethodName) const;

	long FindFunction(const wxString& funcName) const;
	long FindExportFunction(const wxString& funcName) const;

	long FindProcedure(const wxString& procName) const;
	long FindExportProcedure(const wxString& procName) const;

	long FindVariable(const wxString& strVarName) const;
	long FindExportVariable(const wxString& strVarName) const;

	long GetNParams(const long lCodeLine) const {
		// if is not initializer then set no return value
		auto foundedFunction = std::find_if(m_listFunc.begin(), m_listFunc.end(),
			[lCodeLine](const std::pair<const wxString, CByteMethod>& pair) {
				return lCodeLine == ((long)pair.second);
			}
		);
		if (foundedFunction != m_listFunc.end())
			return foundedFunction->second.m_lCodeParamCount;
		return 0;
	}

	bool HasRetVal(const long lCodeLine) const {
		// if is not initializer then set no return value
		auto foundedFunction = std::find_if(m_listFunc.begin(), m_listFunc.end(),
			[lCodeLine](const std::pair<const wxString, CByteMethod>& pair) {
				return pair.second.m_bCodeRet && lCodeLine == ((long)pair.second);
			}
		);
		return foundedFunction != m_listFunc.end();
	}

	void Reset() {
		m_lStartModule = 0;
		m_lVarCount = 0;
		m_strModuleName = wxEmptyString;
		m_bCompile = false;
		m_parent = nullptr;
		m_compileModule = nullptr;
		m_listCode.clear();
		m_listConst.clear();
		m_listVar.clear();
		m_listFunc.clear();
		m_listExportVar.clear();
		m_listExportFunc.clear();
		m_listExternValue.clear();
	}
};

#endif // !_BYTE_CODE_H__
