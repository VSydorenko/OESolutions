#ifndef _AUTOCOMPLETE_COMPILE_H__
#define _AUTOCOMPLETE_COMPILE_H__

#include "backend/moduleInfo.h"
#include "backend/compiler/compileCode.h"

struct CParamValue {
	wxString m_paramName;//variable name 
	wxString m_paramType;//variable type in English notation (in case of explicit typing)
	CValue m_paramObject;
};

class CPrecompileModule;
struct CPrecompileFunction;

struct CPrecompileVariable
{
	bool bExport;
	bool bContext;
	bool bTempVar;

	int nNumber;

	wxString strName;//��� ����������
	wxString strType;//��� ��������
	wxString strRealName;

	CValue m_valContext;
	CValue m_valObject;

	CPrecompileVariable() : bExport(false), bContext(false), bTempVar(false), nNumber(0) {};
	CPrecompileVariable(wxString csVarName) : strName(csVarName), bExport(false), bContext(false), bTempVar(false), nNumber(0) {};
};

struct CPrecompileContext
{
	CPrecompileModule* pModule;
	void SetModule(CPrecompileModule* pSetModule) { pModule = pSetModule; }

	CPrecompileContext* pParent;//������������ ��������
	CPrecompileContext* pStopParent;//������ ����������� ������� �����������
	CPrecompileContext* pContinueParent;//������ ����������� ������� �����������
	bool bStaticVariable;		//��� ���������� ���������

	//����������
	std::map <wxString, CPrecompileVariable> cVariables;

	CParamValue GetVariable(const wxString& strVarName, bool bFindInParent = true, bool bCheckError = false, const CValue& valVar = CValue());
	CParamValue AddVariable(const wxString& strVarName, const wxString& varType = wxEmptyString, bool bExport = false, bool bTempVar = false, const CValue& valVar = CValue());
	void SetVariable(const wxString& strVarName, const CValue& valVar);

	bool FindVariable(const wxString& strName, CValue& valContext = CValue(), bool bContext = false);
	bool FindFunction(const wxString& strName, CValue& valContext = CValue(), bool bContext = false);

	void RemoveVariable(const wxString& strName);

	int nTempVar;//����� ������� ��������� ����������
	int nFindLocalInParent;//������� ������ ���������� � �������� (�� ���� �������), � ��������� ������� � ��������� ������ ������ ���������� ����������)

	//������� � ���������
	std::map<wxString, CPrecompileFunction*> cFunctions;//������ ������������� ����������� �������
	int nReturn;//����� ��������� ��������� RETURN : RETURN_NONE,RETURN_PROCEDURE,RETURN_FUNCTION
	wxString sCurFuncName;//��� ������� ������������� ������� (��� ��������� �������� ������ ����������� �������)

	CPrecompileContext(CPrecompileContext* hSetParent = nullptr) {
		pParent = hSetParent;

		nReturn = 0;
		nFindLocalInParent = 1;
		pModule = nullptr;

		pStopParent = nullptr;
		pContinueParent = nullptr;

		if (hSetParent) {
			pStopParent = hSetParent->pStopParent;
			pContinueParent = hSetParent->pContinueParent;
		}

		nTempVar = 0;
		bStaticVariable = false;

	};

	~CPrecompileContext();
};

//����������� �������
struct CPrecompileFunction
{
	wxString strRealName;//��� �������
	wxString strName;//��� ������� � ������� ��������
	std::vector<CParamValue> aParamList;
	bool bExport;
	bool bContext;
	CPrecompileContext* m_pContext;//������� ����������
	int nVarCount;// number of local variables
	int nStart;// starting position � ������� ����-�����
	int nFinish;//�������� ������� � ������� ����-�����

	CValue m_valContext;

	CParamValue RealRetValue;//��� �������� ���������� ��� �������� ������
	bool bSysFunction;
	wxString strType;		//��� (� ����. �������), ���� ��� �������������� �������

	//��� IntelliSense
	int	nNumberLine;	//source line number (for breakpoints)
	wxString strShortDescription;//�������� � ���� ��� ������ ����� ��������� ����� �������(���������)
	wxString sLongDescription;//�������� � ���� ���� ������� (�.�.� ��� ������ �����) ���� ����������� �� ����������� ������� (���������)

	CPrecompileFunction(const wxString& strFuncName, CPrecompileContext* pSetContext = nullptr)
	{
		strName = strFuncName;
		m_pContext = pSetContext;
		bExport = false;
		bContext = false;
		nVarCount = 0;
		nStart = 0;
		nFinish = 0;
		bSysFunction = false;
		nNumberLine = -1;
	};

	~CPrecompileFunction()
	{
		if (m_pContext)//������� ����������� �������� (� ������ ������� ���� ������ ����� � ��������� ����������)
			delete m_pContext;
	};
};

//*******************************************************************
//*                         �����: ���-����������                   *
//*******************************************************************
class CPrecompileModule : public CTranslateCode
{
	int m_numCurrentCompile;		//������� ��������� � ������� ������

	CMetaObjectModule* m_moduleObject;

	std::map<wxString, unsigned int> m_aHashConstList;

	CPrecompileContext	cContext;
	CPrecompileContext* m_pContext;
	CPrecompileContext* m_pCurrentContext;

	CValue m_valObject;

	unsigned int nLastPosition;

	wxString sLastExpression;
	wxString sLastKeyword;
	wxString sLastParentKeyword;

	bool m_bCalcValue;

	unsigned int m_nCurrentPos;

	friend class CCodeEditor;

public:

	//�������� ������:
	virtual void Clear();//����� ������ ��� ���������� ������������� �������
	void PrepareModuleData();

	CPrecompileModule(CMetaObjectModule* moduleObject);
	virtual ~CPrecompileModule();

	CValue GetComputeValue() const {
		return m_valObject;
	}

	CPrecompileContext* GetContext() {
		cContext.SetModule(this);
		return &cContext;
	};

	CPrecompileContext* GetCurrentContext() const {
		return m_pCurrentContext;
	}

	bool Compile();

	bool PrepareLexem();
	void PatchLexem(unsigned int line, int offsetLine, unsigned int offsetString, unsigned int modFlags);

protected:

	bool CompileFunction();
	bool CompileDeclaration();

	bool CompileBlock();

	bool CompileNewObject();
	bool CompileGoto();
	bool CompileIf();
	bool CompileWhile();
	bool CompileFor();
	bool CompileForeach();

protected:

	bool CompileModule();

	CLexem PreviewGetLexem();
	CLexem GetLexem();
	CLexem GETLexem();
	void GETDelimeter(const wxUniChar &c);

	bool IsNextDelimeter(const wxUniChar &c);
	bool IsNextKeyWord(int nKey);
	void GETKeyWord(int nKey);
	wxString GETIdentifier(bool strRealName = false);
	CValue GETConstant();
	int GetConstString(const wxString& sMethod);

	int IsTypeVar(const wxString& strType = wxEmptyString);
	wxString GetTypeVar(const wxString& strType = wxEmptyString);

	CParamValue GetExpression(int nPriority = 0);

	CParamValue GetCurrentIdentifier(int& nIsSet);
	CParamValue GetCallFunction(const wxString& strName);

	void AddVariable(const wxString& strVarName, const CValue& varVal);

	CParamValue GetVariable(const wxString& strVarName, bool bCheckError = false);
	CParamValue GetVariable();

	void SetVariable(const wxString& strVarName, const CValue& varVal);

	CParamValue FindConst(CValue& vData);
};

#endif 