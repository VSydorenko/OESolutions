#ifndef _TRANSLATEMODULE_H__
#define _TRANSLATEMODULE_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <map>
#include <vector>

#include "codeDef.h"
#include "backend/backend_exception.h"

#include "value/value.h"

//������ �������� ����
struct aKeyWordsDef {
	//char *Rus;
	char* Eng;
	char* strShortDescription;
};

extern BACKEND_API struct aKeyWordsDef s_aKeyWords[];

enum {
	LEXEM_ADD = 0,
	LEXEM_ADDDEF,
	LEXEM_IGNORE,
};

//�������� �������:
enum {
	RETURN_NONE = 0,//��� �������� (��� ������)
	RETURN_PROCEDURE,//������� �� ���������
	RETURN_FUNCTION,//������� �� �������
};

//�������� ���������� (�������� � �����. ��������� � ������� nArray ����-����)
enum {
	DEF_VAR_SKIP = -1,//����������� ��������
	DEF_VAR_DEFAULT = -2,//�������� �� ���������
	DEF_VAR_TEMP = -3,//������� ��������� ��������� ����������
	DEF_VAR_NORET = -7,//������� (���������) �� ���������� ��������
	DEF_VAR_CONST = 1000,//�������� ��������

};

//�����������

//�������� ������ ��������� �� ��������� ����
struct lexem_t {

	//��� �������:
	short       m_nType;

	//���������� �������:
	short		m_nData;		//����� ���������� �����(KEYWORD) ��� ������ �����������(DELIMITER)
	CValue	    m_vData;		//��������, ���� ��� ��������� ��� �������� ��� ��������������
	wxString	m_strData;		//��� ��� �������������� (����������, ������� � ��.)

	//�������������� ����������: 
	wxString m_strModuleName;//��� ������ (�.�. �������� include ����������� �� ������ �������)
	wxString m_strDocPath; // ���������� ���� � ��������� 
	wxString m_strFileName; // ���� � ����� (���� ������� ���������) 

	unsigned int m_nNumberLine;	  //����� ������ ��������� ������ (��� ����� ��������)
	unsigned int m_nNumberString;	  //����� ��������� ������ (��� ������ ������)

public:

	//�����������: 
	lexem_t() :
		m_nType(0),
		m_nData(0),
		m_nNumberString(0),
		m_nNumberLine(0) {
	}
};

typedef std::vector<lexem_t> CLexemList;

/************************************************
CTranslateCode-���� �������� ��������� ����
����� ����� - ��������� Load() � TranslateModule().
������ ��������� ��������� ������������� ���������� � ��������
������ ������������ ����, ������ ��������� ��������� ����������
(������� ����). � �������� ���������� � ��������� ������ �����������
������ "������" ����-���� � ���������� cByteCode.
*************************************************/

class BACKEND_API CTranslateCode {

	//����� ��� �������� ����������� ������������
	class CDefineList {
		CDefineList* m_parentDefine;
		std::map<wxString, CLexemList*> m_defineList;//�������� ������� ������
	public:

		CDefineList() : m_parentDefine(nullptr) {};
		~CDefineList() { Clear(); }

		void Clear() { m_defineList.clear(); }

		void SetParent(CDefineList* parent) {
			m_parentDefine = parent;
		}

		void RemoveDef(const wxString& strName);
		bool HasDefine(const wxString& strName) const;
		CLexemList* GetDefine(const wxString& strName);
		void SetDefine(const wxString& strName, CLexemList*);
		void SetDefine(const wxString& strName, const wxString& strValue);
	};

	static CDefineList s_glDefineList;

public:

	CTranslateCode();
	CTranslateCode(const wxString& strModuleName, const wxString& strDocPath);
	CTranslateCode(const wxString& strFileName);

	virtual ~CTranslateCode();

	bool HasDefine(const wxString& strName) const {
		if (m_defineList != nullptr)
			return m_defineList->HasDefine(strName);
		return false;
	};

	//������:
	void Load(const wxString& strCode);

	void AppendModule(CTranslateCode* module);
	void RemoveModule(CTranslateCode* module);

	virtual void OnSetParent(CTranslateCode* setParent);

	virtual void Clear();
	bool PrepareLexem();

protected:
	void SetError(int codeError, int currPos, const wxString& errorDesc = wxEmptyString) const;
	void SetError(int codeError,
		const wxString& strFileName, const wxString& strModuleName, const wxString& strDocPath,
		int currPos, int currLine,
		const wxString& errorDesc = wxEmptyString) const;
public:
	virtual void ProcessError(const wxString& strFileName,
		const wxString& strModuleName, const wxString& strDocPath,
		unsigned int currPos, unsigned int currLine,
		const wxString& strCodeLineError, int codeError, const wxString& strErrorDesc 
	) const;
public:

	inline void SkipSpaces() const;

	bool IsByte(const wxUniChar&c) const;
	wxUniChar GetByte() const;

	bool IsWord() const;
	wxString GetWord(bool bOrigin = false, bool bGetPoint = false, wxString* psOrig = nullptr);

	bool IsNumber() const;
	wxString GetNumber() const;

	bool IsString() const;
	wxString GetString() const;

	bool IsDate() const;
	wxString GetDate() const;

	bool IsEnd() const;

#if defined(_LP64) || defined(__LP64__) || defined(__arch64__) || defined(_WIN64)
	static long long IsKeyWord(const wxString& sKeyWord);
#else
	static int IsKeyWord(const wxString& sKeyWord);
#endif

	wxString GetStrToEndLine() const;
	void PrepareFromCurrent(int nMode, const wxString& strName = wxEmptyString);

	wxString GetModuleName() const {
		return m_strModuleName;
	}

	unsigned int GetBufferSize() const {
		return m_strBuffer.size();
	}

	unsigned int GetCurrentPos() const {
		return m_nCurPos;
	}

	unsigned int GetCurrentLine() const {
		return m_nCurLine;
	}

public:

	static std::map<wxString, void*> m_aHashKeyWords;
	static void LoadKeyWords();

protected:

	//������ � ���������� ��� �������� ������
	std::vector<CTranslateCode*> m_listTranslateCode;


	//��������� "��������":
	CDefineList* m_defineList;

	bool m_bAutoDeleteDefList;
	int m_nModePreparing;

	//��������:
	wxString m_strModuleName;//��� �������������� ������ (��� ������ ���������� ��� �������)
	wxString m_strDocPath; // ���������� ���� � ��������� 
	wxString m_strFileName; // ���� � ����� (���� ������� ���������) 

	unsigned int m_bufferSize;//������ ��������� ������

	//�������� �����:
	wxString m_strBuffer;
		
	mutable unsigned int m_nCurPos;//������� ������� ��������������� ������
	mutable unsigned int m_nCurLine;

	//������������� ������ � ���������:
	std::vector<lexem_t> m_listLexem;
};

#endif