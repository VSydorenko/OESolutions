#ifndef _COMPILE_CODE_H__
#define _COMPILE_CODE_H__

#include "translateCode.h"
#include "compileContext.h"

//*******************************************************************
//*                         �����: ����������                       *
//*******************************************************************

class BACKEND_API CCompileCode : public CTranslateCode {

	friend class CProcUnit;

	struct CCallFunction {

		wxString m_strName;//��� ���������� �������
		wxString m_strRealName;//��� ���������� �������

		CParamUnit m_sRetValue;//���������� ���� ������ ������������ ��������� ���������� �������
		CParamUnit m_sContextVal;//��������� �� ���������� ��������

		unsigned int m_nAddLine;//��������� � ������� ����-�����, ��� ���������� ����� (��� ������, ����� ���� �����, �� ������� ��� �� ���������)
		unsigned int m_nError;//��� ������ ��������� ��� �������

		unsigned int m_nNumberString;	//����� ��������� ������ (��� ������ ������)
		unsigned int m_nNumberLine;	//����� ������ ��������� ������ (��� ����� ��������)

		wxString m_strModuleName;//��� ������ (�.�. �������� include ����������� �� ������ �������)

		std::vector<CParamUnit> m_aParamList;//������ ������������ ���������� (������ ���������, ���� �������� �� ������, �� �.�. (-1,-1))
	};

	friend struct CCompileContext;

public:

	CCompileCode();
	CCompileCode(const wxString& strModuleName, const wxString& strDocPath, bool onlyFunction = false);
	CCompileCode(const wxString& strFileName);

	virtual ~CCompileCode();

	//�������� ������:
	void Reset();//����� ������ ��� ���������� ������������� �������

	void PrepareModuleData();

	void AddVariable(const wxString& strName, const CValue& value);//��������� ������� ����������
	void AddVariable(const wxString& strName, CValue* pValue);//��������� ������� ����������

	void AddContextVariable(const wxString& strName, const CValue& value);
	void AddContextVariable(const wxString& strName, CValue* pValue);

	void RemoveVariable(const wxString& strName);

	void SetParent(CCompileCode* parent); //��������� ������������� ������ � ������������ ����. �����������
	virtual CCompileCode* GetParent() const {
		return m_parent;
	}

	virtual bool Recompile(); // ������������ ������ �� ����-�������

	virtual bool Compile(); // ���������� ������ �� ����-�������
	virtual bool Compile(const wxString& strCode); //���������� ������

	//��������:
	bool m_onlyFunction; // true - only functions and export functions 

	CCompileCode* m_parent;//������������ ������ (�.�. �� ��������� � �������� ��������� ���� ����������� ������)

	//������� �������� ����������, ������� � �����
	CCompileContext m_cContext, * m_pContext;
	CByteCode		m_cByteCode;        //�������� ������ ����-����� ��� ���������� ����������� �������

	int				m_nCurrentCompile;		//������� ��������� � ������� ������
	bool			m_bExpressionOnly;		//������ ���������� ��������� (��� ����� ������� �������)
	bool			m_changeCode;

	//������������ ������� ����������
	std::map<wxString, CValue*> m_aExternValues;

	//������������ ����������� ����������
	std::map<wxString, CValue*> m_aContextValues;

protected:

	std::map<wxString, unsigned int> m_aHashConstList;
	std::vector <CCallFunction*> m_apCallFunctions;	//������ ������������� ������� �������� � �������

public:

	static void InitializeCompileModule();

	CCompileContext* GetContext() {
		m_cContext.SetModule(this);
		return &m_cContext;
	};

private:
	//������ ������ ������ ��� ����������:
	void SetError(int nErr, const wxString& strError = wxEmptyString);
	void SetError(int nErr, const wxUniChar &c);
public:

	virtual void ProcessError(const wxString& strFileName,
		const wxString& strModuleName, const wxString& strDocPath,
		unsigned int currPos, unsigned int currLine,
		const wxString& strCodeLineError, int codeError, const wxString& strErrorDesc
	) const;

	CParamUnit GetExpression(int priority = 0);

protected:

	lexem_t PreviewGetLexem();
	lexem_t GetLexem();
	lexem_t GETLexem();
	void GETDelimeter(const wxUniChar &c);

	bool IsDelimeter(const wxUniChar &c);
	bool IsKeyWord(int nKey);

	bool IsNextDelimeter(const wxUniChar &c);
	bool IsNextKeyWord(int nKey);

	void GETKeyWord(int nKey);

	wxString GETIdentifier(bool strRealName = false);
	CValue GETConstant();

	void AddLineInfo(CByteUnit& code);

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

	CParamUnit GetCallFunction(const wxString& strName);
	CParamUnit GetCurrentIdentifier(int& nIsSet);

	CParamUnit FindConst(const CValue& vData);

	bool AddCallFunction(CCallFunction* pRealCall);
	CFunction* GetFunction(const wxString& strName, int* pNumber = nullptr);

	bool IsTypeVar(const wxString& sVariable = wxEmptyString);
	wxString GetTypeVar(const wxString& sVariable = wxEmptyString);
	void AddTypeSet(const CParamUnit& sVariable);

	int GetConstString(const wxString& sMethod);

protected:

	virtual CParamUnit AddVariable(const wxString& strName,
		const wxString& strType = wxEmptyString, bool bExport = false, bool bContext = false, bool bTempVar = false);
	virtual CParamUnit GetVariable();
	virtual CParamUnit GetVariable(const wxString& strName, bool bCheckError = false, bool bLoadFromContext = false);
};

#endif 