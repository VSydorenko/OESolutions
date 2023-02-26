#ifndef _COMPILEMODULE_H__
#define _COMPILEMODULE_H__

#include "translateModule.h"

struct param_t {
	
	wxLongLong_t m_nArray;
	wxLongLong_t m_nIndex;
	wxString m_sName;//��� ���������� 
	wxString m_sType;//��� ���������� � ����. ������� (� ������ ����� ���������)

public:
	param_t() : m_nArray(0), m_nIndex(0) {}
};

struct variable_t {
	
	bool m_bExport;
	bool m_bContext;
	bool m_bTempVar;
	unsigned int m_nNumber;
	wxString m_sName;       //��� ����������
	wxString m_sType;       //��� �������
	wxString m_sContextVar; //��� ����������� ����������
	wxString m_sRealName;   //��� ���������� ��������

public:

	variable_t() : m_bExport(false), m_bContext(false), m_bTempVar(false), m_nNumber(0) {};
	variable_t(const wxString& sVariableName) : m_sName(sVariableName), m_bExport(false), m_bContext(false), m_bTempVar(false), m_nNumber(0) {};
};

struct function_t;

//�������� ������ ���� ���������
struct byteRaw_t {

	short m_nOper;                //��� ����������
	unsigned int m_nNumberString;	//����� ��������� ������ (��� ������ ������)
	unsigned int m_nNumberLine;	//����� ������ ��������� ������ (��� ����� ��������)

	//��������� ��� ����������:
	param_t m_param1;
	param_t m_param2;
	param_t m_param3;
	param_t m_param4; // - ������������ ��� �����������

	wxString m_sModuleName; //��� ������ (�.�. �������� include ����������� �� ������ �������)
	wxString m_sDocPath; // ���������� ���� � ��������� 
	wxString m_sFileName; // ���� � ����� (���� ������� ���������) 

public:

	byteRaw_t() : m_param1(), m_param2(), m_param3(), m_param4(),
		m_nOper(0), m_nNumberString(0), m_nNumberLine(0) {}
};

class CCompileModule;

//����� ����-����
struct byteCode_t {

	struct byteMethod_t {
		long m_lCodeParamCount;
		long m_lCodeLine;
		bool m_bCodeRet;
	public:
		operator long() const {
			return m_lCodeLine;
		}
	};

	//��������:
	byteCode_t* m_parent;	//������������ ����-��� (��� ��������)
	bool m_bCompile;		//������� �������� ����������
	CCompileModule* m_compileModule;
	long m_lVarCount;		// ���������� ��������� ���������� � ������
	long m_lStartModule;	// ������ ������� ������ ������
	wxString m_sModuleName;//��� ������������ ������, �������� ����������� ����-���

	//������ ������� � ����������� ����������
	std::vector<CValue*> m_aExternValues;
	std::vector <byteRaw_t>	m_aCodeList;//����������� ��� ������
	std::vector <CValue> m_aConstList;//������ �������� ������
	std::map<wxString, long> m_aVarList; //������ ���������� ������	
	std::map<wxString, byteMethod_t> m_aFuncList; //������ ������� � �������� ������
	std::map<wxString, long> m_aExportVarList; //������ ���������� ���������� ������
	std::map<wxString, byteMethod_t> m_aExportFuncList; //������ ���������� ������� � �������� ������

public:

	byteCode_t() {
		Reset();
	};

	void SetModule(CCompileModule* compileModule) {
		m_compileModule = compileModule;
	}

	long FindMethod(const wxString& methodName) const;
	long FindExportMethod(const wxString& methodName) const;
	long FindVariable(const wxString& varName) const;
	long FindExportVariable(const wxString& varName) const;

	long GetNParams(const long lCodeLine) const {
		// if is not initializer then set no return value
		auto foundedFunction = std::find_if(m_aFuncList.begin(), m_aFuncList.end(),
			[lCodeLine](const std::pair<const wxString, byteMethod_t>& pair) {
				return lCodeLine == ((long)pair.second - 1);
			}
		);
		if (foundedFunction != m_aFuncList.end())
			return foundedFunction->second.m_lCodeParamCount;
		return 0;
	}

	bool HasRetVal(const long lCodeLine) const {
		// if is not initializer then set no return value
		auto foundedFunction = std::find_if(m_aFuncList.begin(), m_aFuncList.end(),
			[lCodeLine](const std::pair<const wxString, byteMethod_t>& pair) {
				return pair.second.m_bCodeRet && lCodeLine == ((long)pair.second - 1);
			}
		);
		return foundedFunction != m_aFuncList.end();
	}

	void Reset() {
		m_lStartModule = 0;
		m_lVarCount = 0;
		m_sModuleName = wxEmptyString;
		m_bCompile = false;
		m_parent = NULL;
		m_compileModule = NULL;
		m_aCodeList.clear();
		m_aConstList.clear();
		m_aVarList.clear();
		m_aFuncList.clear();
		m_aExportVarList.clear();
		m_aExportFuncList.clear();
		m_aExternValues.clear();
	}
};

struct label_t {
	wxString m_sName;
	int		m_nLine;
	int		m_nError;
};

class CCompileModule;

//*******************************************************************
//*                �����: �������� ����������                       *
//*******************************************************************

typedef std::vector<CValue> CDefValueList;
typedef std::vector<bool> CDefBoolList;
typedef std::vector<int> CDefIntList;

struct compileContext_t {

	function_t* m_functionContext;
	compileContext_t* m_parentContext;//������������ ��������

	CCompileModule* m_compileModule;

	//����������
	std::map <wxString, variable_t> m_cVariables;

	int m_nTempVar;//����� ������� ��������� ����������
	int m_nFindLocalInParent;//������� ������ ���������� � �������� (�� ���� �������), � ��������� ������� � ��������� ������ ������ ���������� ����������)

	//������� � ���������
	std::map<wxString, function_t*>	m_cFunctions;//������ ������������� ����������� �������

	short m_nReturn;//����� ��������� ��������� RETURN : RETURN_NONE,RETURN_PROCEDURE,RETURN_FUNCTION
	wxString m_sCurFuncName;//��� ������� ������������� ������� (��� ��������� �������� ������ ����������� �������)

	//�����
	//��������� ��������
	unsigned short m_nDoNumber;//����� ���������� �����

	std::map<unsigned short, CDefIntList*> aContinueList;//������ ���������� Continue
	std::map<unsigned short, CDefIntList*> aBreakList;//������ ���������� Break

	//�����
	std::map<wxString, unsigned int> m_cLabelsDef;	//����������
	std::vector <label_t> m_cLabels;	//������ ������������� ��������� �� �����

public:

	//��������� ������� �������� ��� ������ Continue � Break
	void StartDoList() {
		//������� ������ ��� ������ Continue � Break (� ��� ����� �������� ������ ���� �����, ��� ����������� ��������������� �������)
		m_nDoNumber++;
		aContinueList[m_nDoNumber] = new CDefIntList();
		aBreakList[m_nDoNumber] = new CDefIntList();
	}

	//��������� ������� �������� ��� ������ Continue � Break
	void FinishDoList(byteCode_t& cByteCode, int nGotoContinue, int nGotoBreak) {
		CDefIntList* pListC = (CDefIntList*)aContinueList[m_nDoNumber];
		CDefIntList* pListB = (CDefIntList*)aBreakList[m_nDoNumber];
		if (pListC == 0 || pListB == 0) {
#ifdef _DEBUG 
			wxLogDebug("Error (FinishDoList) nGotoContinue=%d, nGotoBreak=%d\n", nGotoContinue, nGotoBreak);
			wxLogDebug("m_nDoNumber=%d\n", m_nDoNumber);
#endif 
			m_nDoNumber--;
			return;
		}
		for (unsigned int i = 0; i < pListC->size(); i++)
			cByteCode.m_aCodeList[*pListC[i].data()].m_param1.m_nIndex = nGotoContinue;

		for (unsigned int i = 0; i < pListB->size(); i++)
			cByteCode.m_aCodeList[*pListB[i].data()].m_param1.m_nIndex = nGotoBreak;
		aContinueList.erase(m_nDoNumber);
		aContinueList.erase(m_nDoNumber);
		delete pListC;
		delete pListB;
		m_nDoNumber--;
	}

	void DoLabels();

	void SetModule(CCompileModule* module) {
		m_compileModule = module;
	}

	void SetFunction(function_t* function) {
		m_functionContext = function;
	}

	param_t AddVariable(const wxString& varName, const wxString& sType = wxEmptyString, bool bExport = false, bool bContext = false, bool bTempVar = false);
	param_t GetVariable(const wxString& varName, bool bFindInParent = true, bool bCheckError = false, bool bContext = false, bool bTempVar = false);

	bool FindVariable(const wxString& varName, variable_t*& foundedVar, bool context = false);
	bool FindFunction(const wxString& funcName, function_t*& foundedFunc, bool context = false);

	compileContext_t(compileContext_t* hSetParent = NULL) : m_parentContext(hSetParent),
		m_nDoNumber(0), m_nReturn(0), m_nTempVar(0), m_nFindLocalInParent(1),
		m_compileModule(NULL), m_functionContext(NULL) {
	};

	~compileContext_t();
};

struct paramVariable_t
{
	bool m_bByRef;

	wxString m_sName;//��� ����������
	wxString m_sType;//��� ��������

	param_t  m_vData;//�������� �� ���������

public:

	paramVariable_t() : m_bByRef(false) {
		m_vData.m_nArray = -1;
		m_vData.m_nIndex = -1;
	};
};

//����������� �������
struct function_t {

	bool m_bExport, m_bContext;

	wxString m_sRealName; //��� �������
	wxString m_sName; //��� ������� � ������� ��������
	wxString m_sType;	 //��� (� ����. �������), ���� ��� �������������� �������

	compileContext_t* m_pContext;//������� ����������

	unsigned int m_lVarCount;//����� ��������� ����������
	unsigned int m_nStart;//��������� ������� � ������� ����-�����
	unsigned int m_nFinish;//�������� ������� � ������� ����-�����

	param_t m_realRetValue;//��� �������� ���������� ��� �������� ������

	//��� IntelliSense
	unsigned int m_nNumberLine;	//����� ������ ��������� ������ (��� ����� ��������)

	wxString m_sShortDescription;//�������� � ���� ��� ������ ����� ��������� ����� �������(���������)
	wxString m_sLongDescription;//�������� � ���� ���� ������� (�.�.� ��� ������ �����) ���� ����������� �� ����������� ������� (���������)
	wxString m_sContextVar; //��� ���������� ����������

	std::vector<paramVariable_t> m_aParamList;

public:
	
	function_t(const wxString& funcName, compileContext_t* compileContext = NULL) : m_sName(funcName), m_pContext(compileContext),
		m_bExport(false), m_bContext(false), m_lVarCount(0), m_nStart(0), m_nFinish(0), m_nNumberLine(0) {
		m_realRetValue.m_nArray = 0;
		m_realRetValue.m_nIndex = 0;
	};

	~function_t() {
		if (m_pContext != NULL)
			delete m_pContext; //������� ����������� �������� (� ������ ������� ���� ������ ����� � ��������� ����������)
	};
};

struct �allFunction_t {

	wxString m_sName;//��� ���������� �������
	wxString m_sRealName;//��� ���������� �������

	param_t m_sRetValue;//���������� ���� ������ ������������ ��������� ���������� �������
	param_t m_sContextVal;//��������� �� ���������� ��������

	unsigned int m_nAddLine;//��������� � ������� ����-�����, ��� ���������� ����� (��� ������, ����� ���� �����, �� ������� ��� �� ���������)
	unsigned int m_nError;//��� ������ ��������� ��� �������

	unsigned int m_nNumberString;	//����� ��������� ������ (��� ������ ������)
	unsigned int m_nNumberLine;	//����� ������ ��������� ������ (��� ����� ��������)

	wxString m_sModuleName;//��� ������ (�.�. �������� include ����������� �� ������ �������)

	std::vector<param_t> m_aParamList;//������ ������������ ���������� (������ ���������, ���� �������� �� ������, �� �.�. (-1,-1))
};

class CMetaModuleObject;

//*******************************************************************
//*                         �����: ����������                       *
//*******************************************************************

class CCompileModule : public CTranslateModule {
	friend class CProcUnit;
private:
	bool Recompile(); //�������������� �������� ������ �� ����-�������
public:

	CCompileModule();
	CCompileModule(CMetaModuleObject* moduleObject, bool commonModule = false);
	virtual ~CCompileModule();

	//�������� ������:
	void Reset();//����� ������ ��� ���������� ������������� �������

	void PrepareModuleData();

	void AddVariable(const wxString& sName, const CValue& value);//��������� ������� ����������
	void AddVariable(const wxString& sName, CValue* pValue);//��������� ������� ����������

	void AddContextVariable(const wxString& sName, const CValue& value);
	void AddContextVariable(const wxString& sName, CValue* pValue);

	void RemoveVariable(const wxString& sName);

	void SetParent(CCompileModule* pSetParent); //��������� ������������� ������ � ������������ ����. �����������

	CCompileModule* GetParent() const {
		return m_parent;
	}

	bool Compile(); //���������� ������ �� ����-�������

	//��������:
	bool m_bCommonModule; // true - only functions and export functions 

	CCompileModule* m_parent;//������������ ������ (�.�. �� ��������� � �������� ��������� ���� ����������� ������)

	//������� �������� ����������, ������� � �����
	compileContext_t m_cContext, * m_pContext;
	byteCode_t m_cByteCode;        //�������� ������ ����-����� ��� ���������� ����������� �������

	int m_nCurrentCompile;		//������� ��������� � ������� ������

	bool m_bExpressionOnly;		//������ ���������� ��������� (��� ����� ������� �������)
	bool m_bNeedRecompile;

	//������������ ������� ����������
	std::map<wxString, CValue*> m_aExternValues;
	//������������ ����������� ����������
	std::map<wxString, CValue*> m_aContextValues;

protected:

	CMetaModuleObject* m_moduleObject;

	std::map<wxString, unsigned int> m_aHashConstList;
	std::vector <�allFunction_t*> m_apCallFunctions;	//������ ������������� ������� �������� � �������

	int m_nLastNumberLine;

public:

	static void InitializeCompileModule();

	compileContext_t* GetContext() {
		m_cContext.SetModule(this);
		return &m_cContext;
	};

	CMetaModuleObject* GetModuleObject() const {
		return m_moduleObject;
	}

	//������ ������ ������ ��� ����������:
	void SetError(int nErr, const wxString& sError = wxEmptyString);
	void SetError(int nErr, char c);

	param_t GetExpression(int nPriority = 0);

protected:

	lexem_t PreviewGetLexem();
	lexem_t GetLexem();
	lexem_t GETLexem();
	void GETDelimeter(char c);

	bool IsDelimeter(char c);
	bool IsKeyWord(int nKey);

	bool IsNextDelimeter(char c);
	bool IsNextKeyWord(int nKey);

	void GETKeyWord(int nKey);

	wxString GETIdentifier(bool realName = false);
	CValue GETConstant();

	void AddLineInfo(byteRaw_t& code);

	bool CompileModule();

	bool CompileFunction();
	bool CompileDeclaration();

	bool CompileBlock();

	bool CompileNewObject();
	bool CompileGoto();
	bool CompileIf();
	bool CompileWhile();
	bool CompileFor();
	bool CompileForeach();

	param_t GetCallFunction(const wxString& sName);
	param_t GetCurrentIdentifier(int& nIsSet);

	param_t FindConst(CValue& vData);

	bool AddCallFunction(�allFunction_t* pRealCall);
	function_t* GetFunction(const wxString& sName, int* pNumber = NULL);

	bool IsTypeVar(const wxString& sVariable = wxEmptyString);
	wxString GetTypeVar(const wxString& sVariable = wxEmptyString);
	void AddTypeSet(const param_t& sVariable);

	int GetConstString(const wxString& sMethod);

protected:

	virtual param_t AddVariable(const wxString& sName,
		const wxString& sType = wxEmptyString, bool bExport = false, bool bContext = false, bool bTempVar = false);
	virtual param_t GetVariable();
	virtual param_t GetVariable(const wxString& sName, bool bCheckError = false, bool bLoadFromContext = false);
};

class CGlobalCompileModule : public CCompileModule {
public:
	CGlobalCompileModule(CMetaModuleObject* moduleObject) :
		CCompileModule(moduleObject, false) {
	}
};

#endif 