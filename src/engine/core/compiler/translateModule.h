#ifndef _TRANSLATEMODULE_H__
#define _TRANSLATEMODULE_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <map>
#include <vector>

#include "value.h"
#include "translateError.h"

//������ �������� ����
struct aKeyWordsDef {
	//char *Rus;
	char* Eng;
	char* sShortDescription;
};

extern struct aKeyWordsDef s_aKeyWords[];

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
	short	    m_nData;		//����� ���������� �����(KEYWORD) ��� ������ �����������(DELIMITER)
	CValue	    m_vData;		//��������, ���� ��� ��������� ��� �������� ��� ��������������
	wxString	m_sData;		//��� ��� �������������� (����������, ������� � ��.)

	//�������������� ����������: 
	wxString m_sModuleName;//��� ������ (�.�. �������� include ����������� �� ������ �������)
	wxString m_sDocPath; // ���������� ���� � ��������� 
	wxString m_sFileName; // ���� � ����� (���� ������� ���������) 

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

//����� ��� �������� ����������� ������������
class CDefList {
	CDefList* m_pParent;
	std::map<wxString, CLexemList*> m_aDefList;//�������� ������� ������
public:

	CDefList() : m_pParent(NULL) {};
	~CDefList();

	void SetParent(CDefList* p) {
		m_pParent = p;
	}

	void RemoveDef(const wxString& sName);
	bool HasDef(const wxString& sName) const;
	CLexemList* GetDef(const wxString& sName);
	void SetDef(const wxString& sName, CLexemList*);
	void SetDef(const wxString& sName, const wxString &sValue);
};

/************************************************
CTranslateModule-���� �������� ��������� ����
����� ����� - ��������� Load() � TranslateModule().
������ ��������� ��������� ������������� ���������� � ��������
������ ������������ ����, ������ ��������� ��������� ����������
(������� ����). � �������� ���������� � ��������� ������ �����������
������ "������" ����-���� � ���������� cByteCode.
*************************************************/

class CTranslateModule
{
	static CDefList glDefList;

public:

	CTranslateModule();
	CTranslateModule(const wxString& moduleName, const wxString& docPath);
	CTranslateModule(const wxString& fileName);

	virtual ~CTranslateModule();

	bool HasDef(const wxString& sName) const {
		if (m_aDefList != NULL)
			return m_aDefList->HasDef(sName);
		return false;
	};

	//������:
	void Load(const wxString& code);

	void AppendModule(CTranslateModule* module);
	void RemoveModule(CTranslateModule* module);

	void OnSetParent(CTranslateModule* setParent);

	void Clear();
	bool PrepareLexem();

	void SetError(int codeError, int currPos, const wxString& errorDesc = wxEmptyString) const;
	void SetError(int codeError,
		const wxString& fileName, const wxString& moduleName, const wxString& docPath,
		int currPos, int currLine,
		const wxString& errorDesc = wxEmptyString) const;

	const std::vector<lexem_t>& GetLexems() const {
		return m_aLexemList;
	}

public:

	inline void SkipSpaces() const;

	bool IsByte(char c) const;
	char GetByte() const;

	bool IsWord() const;
	wxString GetWord(bool bOrigin = false, bool bGetPoint = false, wxString* psOrig = NULL);

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
	void PrepareFromCurrent(int nMode, const wxString& sName = wxEmptyString);
	
	wxString GetModuleName() const {
		return m_sModuleName;
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

	std::vector<CTranslateModule*> m_aTranslateModules;

	//������ � ���������� ��� �������� ������
	//��������� "��������":
	CDefList* m_aDefList;
	bool m_bAutoDeleteDefList;
	int m_nModePreparing;

	//��������:
	wxString m_sModuleName;//��� �������������� ������ (��� ������ ���������� ��� �������)
	wxString m_sDocPath; // ���������� ���� � ��������� 
	wxString m_sFileName; // ���� � ����� (���� ������� ���������) 

	unsigned int m_nSizeText;//������ ��������� ������

	//�������� �����:
	wxString m_sBuffer;
	wxString m_sBUFFER;//����� - �������� ��� � ������� ��������

	mutable unsigned int m_nCurPos;//������� ������� ��������������� ������
	mutable unsigned int m_nCurLine;

	//������������� ������ � ���������:
	std::vector<lexem_t> m_aLexemList;
};

#endif