#ifndef _BYTE_CODE_H__
#define _BYTE_CODE_H__

#include "backend/compiler/value/value.h"

struct CParamUnit {

	wxLongLong_t m_nArray;
	wxLongLong_t m_nIndex;
	wxString	 m_strName;//��� ���������� 
	wxString	 m_strType;//��� ���������� � ����. ������� (� ������ ����� ���������)

public:
	CParamUnit() : m_nArray(0), m_nIndex(0) {}
};

//�������� ������ ���� ���������
struct CByteUnit {

	short		 m_nOper;                //��� ����������
	unsigned int m_nNumberString;	//����� ��������� ������ (��� ������ ������)
	unsigned int m_nNumberLine;	//����� ������ ��������� ������ (��� ����� ��������)

	//��������� ��� ����������:
	CParamUnit	 m_param1;
	CParamUnit	 m_param2;
	CParamUnit	 m_param3;
	CParamUnit	 m_param4; // - ������������ ��� �����������

	wxString	 m_strModuleName; //��� ������ (�.�. �������� include ����������� �� ������ �������)
	wxString	 m_strDocPath; // ���������� ���� � ��������� 
	wxString	 m_strFileName; // ���� � ����� (���� ������� ���������) 

public:

	CByteUnit() : m_param1(), m_param2(), m_param3(), m_param4(),
		m_nOper(0), m_nNumberString(0), m_nNumberLine(0) {
	}
};

class CCompileCode;

//����� ����-����
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

	//��������:
	CByteCode* m_parent;	//������������ ����-��� (��� ��������)
	bool m_bCompile;		//������� �������� ����������
	CCompileCode* m_compileModule;
	long m_lVarCount;		// ���������� ��������� ���������� � ������
	long m_lStartModule;	// ������ ������� ������ ������
	wxString m_strModuleName;//��� ������������ ������, �������� ����������� ����-���

	//������ ������� � ����������� ����������
	std::vector<CValue*> m_aExternValues;
	std::vector <CByteUnit> m_aCodeList;//����������� ��� ������
	std::vector <CValue> m_aConstList;//������ �������� ������

	std::map<wxString, long> m_aVarList; //������ ���������� ������	
	std::map<wxString, CByteMethod> m_aFuncList; //������ ������� � �������� ������
	std::map<wxString, long> m_aExportVarList; //������ ���������� ���������� ������
	std::map<wxString, CByteMethod> m_aExportFuncList; //������ ���������� ������� � �������� ������

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
		auto foundedFunction = std::find_if(m_aFuncList.begin(), m_aFuncList.end(),
			[lCodeLine](const std::pair<const wxString, CByteMethod>& pair) {
				return lCodeLine == ((long)pair.second);
			}
		);
		if (foundedFunction != m_aFuncList.end())
			return foundedFunction->second.m_lCodeParamCount;
		return 0;
	}

	bool HasRetVal(const long lCodeLine) const {
		// if is not initializer then set no return value
		auto foundedFunction = std::find_if(m_aFuncList.begin(), m_aFuncList.end(),
			[lCodeLine](const std::pair<const wxString, CByteMethod>& pair) {
				return pair.second.m_bCodeRet && lCodeLine == ((long)pair.second);
			}
		);
		return foundedFunction != m_aFuncList.end();
	}

	void Reset() {
		m_lStartModule = 0;
		m_lVarCount = 0;
		m_strModuleName = wxEmptyString;
		m_bCompile = false;
		m_parent = nullptr;
		m_compileModule = nullptr;
		m_aCodeList.clear();
		m_aConstList.clear();
		m_aVarList.clear();
		m_aFuncList.clear();
		m_aExportVarList.clear();
		m_aExportFuncList.clear();
		m_aExternValues.clear();
	}
};

#endif // !_BYTE_CODE_H__
