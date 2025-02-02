#ifndef _COMPILE_CONTEXT__H_
#define _COMPILE_CONTEXT__H_

#include "backend/compiler/byteCode.h"

struct CCompileContext;

struct CVariable {

	bool m_bExport;
	bool m_bContext;
	bool m_bTempVar;
	unsigned int m_nNumber;
	wxString m_strName;       //��� ����������
	wxString m_strType;       //��� �������
	wxString m_strContextVar; //��� ����������� ����������
	wxString m_strRealName;   //��� ���������� ��������

public:

	CVariable() : m_bExport(false), m_bContext(false), m_bTempVar(false), m_nNumber(0) {};
	CVariable(const wxString& sVariableName) : m_strName(sVariableName), m_bExport(false), m_bContext(false), m_bTempVar(false), m_nNumber(0) {};
};

struct CParamVariable
{
	bool m_bByRef;
	wxString m_strName;   //��� ����������
	wxString m_strType;   //��� ��������
	CParamUnit m_vData; //�������� �� ���������
public:

	CParamVariable() : m_bByRef(false) {
		m_vData.m_nArray = -1;
		m_vData.m_nIndex = -1;
	};
};

//����������� �������
struct CFunction {

	bool m_bExport, m_bContext;

	wxString m_strRealName; //��� �������
	wxString m_strName; //��� ������� � ������� ��������
	wxString m_strType;	 //��� (� ����. �������), ���� ��� �������������� �������

	CCompileContext* m_pContext;//������� ����������

	unsigned int m_lVarCount;//����� ��������� ����������
	unsigned int m_nStart;//��������� ������� � ������� ����-�����
	unsigned int m_nFinish;//�������� ������� � ������� ����-�����

	CParamUnit m_realRetValue;//��� �������� ���������� ��� �������� ������

	//��� IntelliSense
	unsigned int m_nNumberLine;	//����� ������ ��������� ������ (��� ����� ��������)

	wxString m_strShortDescription;//�������� � ���� ��� ������ ����� ��������� ����� �������(���������)
	wxString m_strLongDescription;//�������� � ���� ���� ������� (�.�.� ��� ������ �����) ���� ����������� �� ����������� ������� (���������)
	wxString m_strContextVar; //��� ���������� ����������

	std::vector<CParamVariable> m_aParamList;

public:

	CFunction(const wxString& funcName, CCompileContext* compileContext = nullptr);
	~CFunction();
};

struct CLabel {
	wxString m_strName;
	int		m_nLine;
	int		m_nError;
};

struct CCompileContext {

	CFunction* m_functionContext;
	CCompileContext* m_parentContext; //������������ ��������
	CCompileCode* m_compileModule;

	//����������
	std::map <wxString, CVariable> m_cVariables;

	int m_nTempVar;//����� ������� ��������� ����������
	int m_nFindLocalInParent;//������� ������ ���������� � �������� (�� ���� �������), � ��������� ������� � ��������� ������ ������ ���������� ����������)

	//������� � ���������
	std::map<wxString, CFunction*>	m_cFunctions;//������ ������������� ����������� �������

	short m_nReturn;//����� ��������� ��������� RETURN : RETURN_NONE,RETURN_PROCEDURE,RETURN_FUNCTION
	wxString m_strCurFuncName;//��� ������� ������������� ������� (��� ��������� �������� ������ ����������� �������)

	//�����
	//��������� ��������
	unsigned short m_nDoNumber;//����� ���������� �����

	std::map<unsigned short, std::vector<int>*> aContinueList;//������ ���������� Continue
	std::map<unsigned short, std::vector<int>*> aBreakList;//������ ���������� Break

	//�����
	std::map<wxString, unsigned int> m_cLabelsDef;	//����������
	std::vector <CLabel> m_cLabels;	//������ ������������� ��������� �� �����

public:

	//��������� ������� �������� ��� ������ Continue � Break
	void StartDoList() {
		//������� ������ ��� ������ Continue � Break (� ��� ����� �������� ������ ���� �����, ��� ����������� ��������������� �������)
		m_nDoNumber++;
		aContinueList[m_nDoNumber] = new std::vector<int>();
		aBreakList[m_nDoNumber] = new std::vector<int>();
	}

	//��������� ������� �������� ��� ������ Continue � Break
	void FinishDoList(CByteCode& cByteCode, int gotoContinue, int gotoBreak) {
		std::vector<int>* pListC = (std::vector<int>*)aContinueList[m_nDoNumber];
		std::vector<int>* pListB = (std::vector<int>*)aBreakList[m_nDoNumber];
		if (pListC == 0 || pListB == 0) {
#ifdef DEBUG 
			wxLogDebug("Error (FinishDoList) gotoContinue=%d, gotoBreak=%d\n", gotoContinue, gotoBreak);
			wxLogDebug("m_nDoNumber=%d\n", m_nDoNumber);
#endif 
			m_nDoNumber--;
			return;
		}
		for (unsigned int i = 0; i < pListC->size(); i++) {
			cByteCode.m_aCodeList[*pListC[i].data()].m_param1.m_nIndex = gotoContinue;
		}
		for (unsigned int i = 0; i < pListB->size(); i++) {
			cByteCode.m_aCodeList[*pListB[i].data()].m_param1.m_nIndex = gotoBreak;
		}
		aContinueList.erase(m_nDoNumber);
		aContinueList.erase(m_nDoNumber);
		delete pListC;
		delete pListB;
		m_nDoNumber--;
	}

	void DoLabels();

	void SetModule(CCompileCode* module) {
		m_compileModule = module;
	}

	void SetFunction(CFunction* function) {
		m_functionContext = function;
	}

	CParamUnit AddVariable(const wxString& strVarName, const wxString& strType = wxEmptyString, bool bExport = false, bool bContext = false, bool bTempVar = false);
	CParamUnit GetVariable(const wxString& strVarName, bool bFindInParent = true, bool bCheckError = false, bool bContext = false, bool bTempVar = false);

	bool FindVariable(const wxString& strVarName, CVariable*& foundedVar, bool context = false);
	bool FindFunction(const wxString& funcName, CFunction*& foundedFunc, bool context = false);

	CCompileContext(CCompileContext* hSetParent = nullptr) : m_parentContext(hSetParent),
		m_nDoNumber(0), m_nReturn(0), m_nTempVar(0), m_nFindLocalInParent(1),
		m_compileModule(nullptr), m_functionContext(nullptr) {
	};

	~CCompileContext();
};

#endif