////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko, 2�-team
//	Description : translate module 
////////////////////////////////////////////////////////////////////////////

#include "translateCode.h"

//////////////////////////////////////////////////////////////////////
//                           Constants
//////////////////////////////////////////////////////////////////////

static std::map<wxString, void*> s_aHelpDescription; //�������� �������� ���� � ��������� �������
static std::map<wxString, void*> s_aHashKeywordList;

CTranslateCode::CDefineList CTranslateCode::s_glDefineList; //���������� ������ �����������
std::map<wxString, void*>	CTranslateCode::m_aHashKeyWords;//������ �������� ����

//////////////////////////////////////////////////////////////////////
// Global array
//////////////////////////////////////////////////////////////////////
struct aKeyWordsDef s_aKeyWords[] =
{
	{"if"},
	{"then"},
	{"else"},
	{"elseif"},
	{"endif"},
	{"for"},
	{"foreach"},
	{"to"},
	{"in"},
	{"do"},
	{"enddo"},
	{"while"},
	{"goto"},
	{"not"},
	{"and"},
	{"or"},
	{"procedure"},
	{"endprocedure"},
	{"function"},
	{"endfunction"},
	{"export"},
	{"val"},
	{"return"},
	{"try"},
	{"except"},
	{"endtry"},
	{"continue"},
	{"break"},
	{"raise"},
	{"var"},

	//create object
	{"new"},

	//undefined type
	{"undefined"},

	//null type
	{"null"},

	//boolean type
	{"true"},
	{"false"},

	//preprocessor:
	{"#define"},
	{"#undef"},
	{"#ifdef"},
	{"#ifndef"},
	{"#else"},
	{"#endif"},
	{"#region"},
	{"#endregion"},
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTranslateCode::CDefineList::~CDefineList()
{
	m_defineList.clear();
}

void CTranslateCode::CDefineList::RemoveDef(const wxString& strName)
{
	m_defineList.erase(stringUtils::MakeUpper(strName));
}

bool CTranslateCode::CDefineList::HasDefine(const wxString& strName) const
{
	auto itDefList = m_defineList.find(stringUtils::MakeUpper(strName));
	if (itDefList != m_defineList.end())
		return true;

	static int nLevel = 0;
	nLevel++;
	if (nLevel > MAX_OBJECTS_LEVEL) {
		CBackendException::Error(_("Recursive module call (#3)"));
	}

	//���� � ���������
	bool bRes = false;
	if (m_parentDefine)
		bRes = m_parentDefine->HasDefine(strName);
	nLevel--;
	return bRes;
}

CLexemList* CTranslateCode::CDefineList::GetDefine(const wxString& strName)
{
	auto itDefList = m_defineList.find(stringUtils::MakeUpper(strName));
	if (itDefList != m_defineList.end())
		return itDefList->second;

	//���� � ���������
	if (m_parentDefine && m_parentDefine->HasDefine(strName))
		return m_parentDefine->GetDefine(strName);

	CLexemList* lexList = new CLexemList();
	m_defineList[stringUtils::MakeUpper(strName)] = lexList;
	return lexList;
}

void CTranslateCode::CDefineList::SetDefine(const wxString& strName, CLexemList* pDef)
{
	CLexemList* pList = GetDefine(strName);
	pList->clear();
	if (pDef != nullptr) {
		for (unsigned int i = 0; i < pDef->size(); i++) {
			pList->push_back(*pDef[i].data());
		}
	}
}

void CTranslateCode::CDefineList::SetDefine(const wxString& strName, const wxString& strValue)
{
	CLexemList List;
	if (strValue.length() > 0) {
		lexem_t Lex;
		Lex.m_nType = CONSTANT;
		if (strValue[0] == '-' || strValue[0] == '+' || (strValue[0] >= '0' && strValue[0] <= '9'))//�����
			Lex.m_vData.SetNumber(strValue);
		else
			Lex.m_vData.SetString(strValue);
		List.push_back(Lex);
		SetDefine(stringUtils::MakeUpper(strName), &List);
	}
	else {
		SetDefine(stringUtils::MakeUpper(strName), nullptr);
	}
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTranslateCode::CTranslateCode() : m_defineList(nullptr),
m_bAutoDeleteDefList(false),
m_nModePreparing(LEXEM_ADD)
{
	//���������� ������ �������� ����
	if (m_aHashKeyWords.size() == 0) {
		LoadKeyWords(); //������ ���� ���
	}

	Clear();
}

CTranslateCode::CTranslateCode(const wxString& strModuleName, const wxString& strDocPath) : m_defineList(nullptr),
m_strModuleName(strModuleName), m_strDocPath(strDocPath),
m_bAutoDeleteDefList(false),
m_nModePreparing(LEXEM_ADD)
{
	//���������� ������ �������� ����
	if (m_aHashKeyWords.size() == 0) {
		LoadKeyWords(); //������ ���� ���
	}

	Clear();
}

CTranslateCode::CTranslateCode(const wxString& strFileName) : m_defineList(nullptr),
m_strFileName(strFileName),
m_bAutoDeleteDefList(false),
m_nModePreparing(LEXEM_ADD)
{
	//���������� ������ �������� ����
	if (m_aHashKeyWords.size() == 0) {
		LoadKeyWords(); //������ ���� ���
	}

	Clear();
}

CTranslateCode::~CTranslateCode()
{
	if (m_bAutoDeleteDefList && m_defineList) {
		delete m_defineList;
	}
}

/**
 *���������� ������ �������� ����
 */
void CTranslateCode::LoadKeyWords()
{
	m_aHashKeyWords.clear();

#if defined(_LP64) || defined(__LP64__) || defined(__arch64__) || defined(_WIN64)
	for (unsigned long long i = 0; i < sizeof(s_aKeyWords) / sizeof(s_aKeyWords[0]); i++)
#else 
	for (unsigned int i = 0; i < sizeof(s_aKeyWords) / sizeof(s_aKeyWords[0]); i++)
#endif 
	{
		const wxString& strEng = stringUtils::MakeUpper(s_aKeyWords[i].Eng);
		m_aHashKeyWords[strEng] = (void*)(i + 1);

		//��������� � ������ ��� ��������������� �����������
		s_aHashKeywordList[strEng] = (void*)1;
		s_aHelpDescription[strEng] = &s_aKeyWords[i].strShortDescription;
	}
}

//////////////////////////////////////////////////////////////////////
// Translating
//////////////////////////////////////////////////////////////////////

/**
* Clear
* ����������:
* ����������� ���������� ��� ������ ����������
* ������������ ��������:
* ���
*/
void CTranslateCode::Clear()
{
	m_strBuffer.clear();

	//m_listLexem.clear();
	//m_listTranslateCode.clear();

	m_bufferSize = m_nCurPos = m_nCurLine = 0;
}

/**
 * Load
 * ����������:
 * ��������� ����� �������� ������� + ����������� ���������� ��� ����������
 * ������������ ��������:
 * ���
 */
void CTranslateCode::Load(const wxString& strCode)
{
	Clear();

	m_bufferSize = strCode.length();
	m_strBuffer = strCode;
}

/**
 * SetError
 * ����������:
 * ��������� ������ ���������� � ������� ����������
 * ������������ ��������:
 * ����� �� ���������� ����������!strCurWord
 */

void CTranslateCode::SetError(int codeError, int currPos, const wxString& errorDesc) const
{
	size_t start_pos = 0;
	//���� ������ ������ � ������� �������� ��������� �� ������ ����������
	for (int i = currPos; i > 0; i--) {
		if (m_strBuffer[i] == '\n') {
			start_pos = i + 1; break;
		};
	}
	const int currLine = 1 + m_strBuffer.Left(start_pos).Replace('\n', '\n');

	CTranslateCode::SetError(codeError,
		m_strFileName, m_strModuleName, m_strDocPath,
		currPos, currLine,
		errorDesc);
}

/**
 * SetError
 * ����������:
 * ��������� ������ ���������� � ������� ����������
 * ������������ ��������:
 * ����� �� ���������� ����������!strCurWord
 */

void CTranslateCode::SetError(int codeError,
	const wxString& strFileName, const wxString& strModuleName, const wxString& strDocPath,
	int currPos, int currLine,
	const wxString& strErrorDesc) const
{
	CTranslateCode* translateCode = nullptr;
	if (m_strDocPath != strDocPath) {
		for (auto tModule : m_listTranslateCode) {
			if (strDocPath == tModule->m_strDocPath) {
				translateCode = tModule; break;
			}
		}
	}

	const wxString& strCodeLineError =
		CBackendException::FindErrorCodeLine(m_strBuffer, currPos);

	ProcessError(
		strFileName,
		strModuleName, strDocPath,
		currPos, currLine,
		strCodeLineError, codeError, strErrorDesc
	);
}

/**
 * ProcessError
 * ����������:
 * ��������� ������ ���������� � ������� ����������
 * ������������ ��������:
 * ����� �� ���������� ����������!strCurWord
 */

void CTranslateCode::ProcessError(const wxString& strFileName,
	const wxString& strModuleName, const wxString& strDocPath,
	unsigned int currPos, unsigned int currLine,
	const wxString& strCodeLineError, int codeError, const wxString& strErrorDesc) const
{
}

/**
 * SkipSpaces
 * ����������:
 * ���������� ��� ���������� ������� �� ������ �����
 * ���� ����������� �� ������ ����� �������������� � ����� ������
 * ������������ ��������:
 * ���
 */
void CTranslateCode::SkipSpaces() const
{
	unsigned int i = m_nCurPos;
	for (; i < m_bufferSize; i++) {
		const wxUniChar& c = m_strBuffer[i];
		if (c != ' ' && c != '\t' && c != '\n' && c != '\r') {
			if (c == '/') { //����� ��� �����������
				if (i + 1 < m_bufferSize) {
					if (m_strBuffer[i + 1] == '/') { //���������� �����������
						for (unsigned int j = i; j < m_bufferSize; j++) {
							m_nCurPos = j;
							if (m_strBuffer[j] == '\n' || m_strBuffer[j] == 13) {
								//������������ ��������� ������
								SkipSpaces();
								return;
							}
						}
						i = m_nCurPos + 1;
					}
				}
			}
			m_nCurPos = i;
			break;
		}
		else if (c == '\n') {
			m_nCurLine++;
		}
	}

	if (i == m_bufferSize) {
		m_nCurPos = m_bufferSize;
	}
}

/**
 * IsByte
 * ����������:
 * ��������� �������� �� ��������� ���� (��� ����� ��������) ������
 * ��������� �����
 * ������������ ��������:
 * true,false
 */
bool CTranslateCode::IsByte(const wxUniChar& c) const
{
	SkipSpaces();
	if (m_nCurPos >= m_bufferSize)
		return false;
	if (m_strBuffer[m_nCurPos] == c)
		return true;
	return false;
}

/**
 * GetByte
 * ����������:
 * �������� �� ������� ���� (��� ����� ��������)
 * ���� ������ ����� ���, �� ��������� ����������
 * ������������ ��������:
 * ���� �� ������
 */
wxUniChar CTranslateCode::GetByte() const
{
	SkipSpaces();
	if (m_nCurPos < m_bufferSize)
		return m_strBuffer[m_nCurPos++];
	SetError(ERROR_TRANSLATE_BYTE, m_nCurPos);
	return '\0';
}

/**
 * IsWord
 * ����������:
 * ��������� (�� ������� ������� �������� ������)
 * �������� �� ��������� ����� ���� ������ (��������� ������� � ��.)
 * ������������ ��������:
 * true,false
 */
bool CTranslateCode::IsWord() const
{
	SkipSpaces();
	if (m_nCurPos < m_bufferSize) {
		const wxUniChar& c = m_strBuffer[m_nCurPos];
		if (((c == '_') ||
			(c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
			(c >= '�' && c <= '�') || (c >= '�' && c <= '�') ||
			(c == '#')) && (c != '[' && c != ']'))
			return true;
	}
	return false;
}

/**
 * GetWord
 * ����������:
 * ������� �� ������ ��������� �����
 * ���� ����� ��� (�.�. ��������� ����� ���� �� �������� ������), �� ��������� ����������
 * ��������: getPoint
 * true - ��������� ����� ��� ��������� ����� ����� (��� ��������� ����� ���������)
 * ������������ ��������:
 * ����� �� ������
 */
wxString CTranslateCode::GetWord(bool originName, bool getPoint, wxString* psOrig)
{
	SkipSpaces();
	if (m_nCurPos >= m_bufferSize) {
		SetError(ERROR_TRANSLATE_WORD, m_nCurPos);
	}
	int nNext = m_nCurPos;
	wxString strWord;
	for (unsigned int i = m_nCurPos; i < m_bufferSize; i++) {
		const wxUniChar& c = m_strBuffer[i];
		// if array then break
		if (c == '[' || c == ']') break;
		if ((c == '_') ||
			(c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
			(c >= '�' && c <= '�') || (c >= '�' && c <= '�') ||

			(c >= '0' && c <= '9') ||
			(c == '#' && i == m_nCurPos) || //���� ������ ������ # - ��� ��������� �����
			(c == '.' && getPoint)) {
			if (c == L'.' && getPoint)
				getPoint = false; //����� ������ ����������� ������ ���� ���
			nNext = i + 1;
		}
		else break;
		strWord += c;
	}
	int nFirst = m_nCurPos;
	m_nCurPos = nNext;
	if (nFirst == nNext) {
		SetError(ERROR_TRANSLATE_WORD, nFirst);
	}
	if (originName) {
		return strWord;
	}
	else {
		if (psOrig) {
			*psOrig = strWord;
		}
		return strWord.Upper();
	}
}

/**
 * GetStrToEndLine
 * ����������:
 * �������� ��� ������ �� ����� (������� 13 ��� ����� ���� ���������)
 * ������
 */
wxString CTranslateCode::GetStrToEndLine() const
{
	unsigned int nStart = m_nCurPos;
	unsigned int i = m_nCurPos;
	for (; i < m_bufferSize; i++) {
		if (m_strBuffer[i] == '\r' || m_strBuffer[i] == '\n') {
			i++; break;
		}
	}
	m_nCurPos = i;
	return m_strBuffer.Mid(nStart, m_nCurPos - nStart);
}

/**
 * IsNumber
 * ����������:
 * ��������� (�� ������� ������� �������� ������)
 * �������� �� ��������� ����� ���� ������-���������� (��������� ������� � ��.)
 * ������������ ��������:
 * true,false
*/
bool CTranslateCode::IsNumber() const
{
	SkipSpaces();
	if (m_nCurPos < m_bufferSize) {
		return m_strBuffer[m_nCurPos] >= '0' && m_strBuffer[m_nCurPos] <= '9';
	}
	return false;
}

/**
 * GetNumber
 * ����������:
 * �������� �� ������� �����
 * ���� �������� ����� ���, �� ��������� ����������
 * ������������ ��������:
 * ����� �� ������
 */
wxString CTranslateCode::GetNumber() const
{
	if (!IsNumber()) {
		SetError(ERROR_TRANSLATE_NUMBER, m_nCurPos);
	}
	SkipSpaces();
	if (m_nCurPos >= m_bufferSize) {
		SetError(ERROR_TRANSLATE_NUMBER, m_nCurPos);
	}
	int nNext = m_nCurPos; short get_point = 0;
	wxString strNumber;
	for (unsigned int i = m_nCurPos; i < m_bufferSize; i++) {
		const wxUniChar& c = m_strBuffer[i];
		if ((c >= '0' && c <= '9') || (c == '.')) {
			if (c == '.')
				get_point++; //����� ������ ����������� ������ ���� ���
			nNext = i + 1;
		}
		else break;
		strNumber += c;
	}
	int nFirst = m_nCurPos;
	m_nCurPos = nNext;
	if (nFirst == nNext) {
		SetError(ERROR_TRANSLATE_NUMBER, nFirst);
	}
	else if (IsWord() && m_strBuffer[nNext] != ' ') {
		SetError(ERROR_TRANSLATE_NUMBER, nFirst);
	}
	else if (get_point > 1) {
		SetError(ERROR_TRANSLATE_NUMBER, nFirst);
	}
	return strNumber;
}

/**
 * IsString
 * ����������:
 * ��������� �������� �� ��������� ����� �������� (��� ����� ��������) �������-����������, ����������� � �������
 * ������������ ��������:
 * true,false
 */
bool CTranslateCode::IsString() const
{
	return IsByte('\"') || IsByte('|');
}

/**
 * GetString
 * ����������:
 * �������� �� ������� ������, ����������� � �������
 * ���� ����� ������ ���, �� ��������� ����������
 * ������������ ��������:
 * ������ �� ������
 */
wxString CTranslateCode::GetString() const
{
	if (!IsString()) {
		SetError(ERROR_TRANSLATE_STRING, m_nCurPos);
	}
	unsigned int nCount = 0; bool skip_space = false;
	SkipSpaces();
	if (m_nCurPos >= m_bufferSize) {
		SetError(ERROR_TRANSLATE_WORD, m_nCurPos);
	}
	int nNext = m_nCurPos;
	wxString strString;
	for (unsigned int i = m_nCurPos; i < m_bufferSize; i++) {
		const wxUniChar& c = m_strBuffer[i];
		if (c == '\n') {
			strString += '\n';
			nNext = m_nCurPos + 1;
			m_nCurLine++;
			skip_space = true;
		}
		if (skip_space && c == '|') {
			skip_space = false;
			continue;
		}
		else if (skip_space && (c == ' ' || c == '\t' || c == '\n' || c == '\r')) {
			continue;
		}
		else if (skip_space && (c != ' ' && c != '\t' && c != '\n' && c != '\r')) {
			break;
		}
		if (nCount < 2) {
			nNext = i + 1;
			if (c == '\"') {
				if (i != m_nCurPos && i + 1 < m_bufferSize) {
					if (m_strBuffer[i + 1] == '\"') {
						strString += '\"';
						i++;
						nNext = i + 1;
						continue;
					}
				}
				nCount++;
				continue;
			}
		}
		else break;
		strString += c;
	}
	int nFirst = m_nCurPos;
	m_nCurPos = nNext;
	if (nFirst == nNext) {
		SetError(ERROR_TRANSLATE_STRING, nFirst);
	}
	else if (nCount < 2) {
		SetError(ERROR_TRANSLATE_STRING, nFirst);
	}
	return strString;
}

/**
* IsDate
* ����������:
* ��������� �������� �� ��������� ����� �������� (��� ����� ��������) �����-����������, ����������� � ���������
* ������������ ��������:
* true,false
*/
bool CTranslateCode::IsDate() const
{
	return IsByte('\'');
}

/**
* GetDate
* ����������:
* �������� �� ������� ����, ����������� � ���������
* ���� ����� ���� ���, �� ��������� ����������
* ������������ ��������:
* ���� �� ������, ����������� � �������
*/
wxString CTranslateCode::GetDate() const
{
	if (!IsDate()) {
		SetError(ERROR_TRANSLATE_DATE, m_nCurPos);
	}
	unsigned int nCount = 0;
	SkipSpaces();
	if (m_nCurPos >= m_bufferSize) {
		SetError(ERROR_TRANSLATE_WORD, m_nCurPos);
	}
	int nNext = m_nCurPos;
	wxString strDate;
	for (unsigned int i = m_nCurPos; i < m_bufferSize; i++) {
		const wxUniChar& c = m_strBuffer[i];
		if (c == '\n') {
			nNext = m_nCurPos + 1;
			break;
		}
		if (nCount < 2) {
			if (c == '\'') {
				nCount++;
			}
			nNext = i + 1;
		}
		else break;
		strDate += c;
	}
	int nFirst = m_nCurPos;
	m_nCurPos = nNext;
	if (nFirst == nNext) {
		SetError(ERROR_TRANSLATE_DATE, nFirst);
	}
	else if (nCount < 2) {
		SetError(ERROR_TRANSLATE_DATE, nFirst);
	}
	else if (IsWord()) {
		SetError(ERROR_TRANSLATE_DATE, nFirst);
	}
	return strDate;
}

/**
 * IsEnd
 * ����������:
 * ��������� ������� ��������� �������������� (�.�. ����� �� ����� ������)
 * ������������ ��������:
 * true,false
 */
bool CTranslateCode::IsEnd() const
{
	SkipSpaces();
	if (m_nCurPos < m_bufferSize)
		return false;
	return true;
}

/**
 * IsKeyWord
 * ����������:
 * ���������� �������� �� �������� ����� ��������� ����������
 * ������������ ��������,����:
 * -1: ���
 * ������ ��� ����� 0: ����� � ������ ��������� ����
*/
#if defined(_LP64) || defined(__LP64__) || defined(__arch64__) || defined(_WIN64)
long long CTranslateCode::IsKeyWord(const wxString& strKeyWord)
#else 
int CTranslateCode::IsKeyWord(const wxString& strKeyWord)
#endif
{
	auto itHashKeyWords = m_aHashKeyWords.find(stringUtils::MakeUpper(strKeyWord));
#if defined(_LP64) || defined(__LP64__) || defined(__arch64__) || defined(_WIN64)
	if (itHashKeyWords != m_aHashKeyWords.end())
		return ((long long)itHashKeyWords->second) - 1;
#else 
	if (itHashKeyWords != m_aHashKeyWords.end())
		return ((int)itHashKeyWords->second) - 1;
#endif 
	return wxNOT_FOUND;
}

/**
 * PrepareLexem
 * ������1 - �������� ������ ��� ������������ �������� ������� ��� �������������
 */
bool CTranslateCode::PrepareLexem()
{
	m_listLexem.clear();

	if (m_defineList == nullptr) {
		m_defineList = new CDefineList();
		m_defineList->SetParent(&s_glDefineList);
		m_bAutoDeleteDefList = true;//�������, ��� ������ � ������������� ������� �� (� �� ������� � �������� ���������� �����������)
	}

	wxString strCurWord;

	while (!IsEnd()) {

		lexem_t lex;

		lex.m_strModuleName = m_strModuleName;
		lex.m_strDocPath = m_strDocPath;
		lex.m_strFileName = m_strFileName;

		lex.m_nNumberLine = m_nCurLine;
		lex.m_nNumberString = m_nCurPos;//���� � ���������� ���������� ������, �� ������ ��� ������ ����� ������ ������������

		if (IsWord()) {
			wxString strOrig;
			strCurWord = GetWord(false, false, &strOrig);

			//��������� ����������� ������������ (#define)
			if (m_defineList->HasDefine(strCurWord)) {
				CLexemList* pDef = m_defineList->GetDefine(strCurWord);
				for (unsigned int i = 0; i < pDef->size(); i++) {
					lexem_t* lex = pDef[i].data();
					lex->m_nNumberString = m_nCurPos;
					lex->m_nNumberLine = m_nCurLine;//��� ����� ��������
					lex->m_strModuleName = m_strModuleName;
					lex->m_strDocPath = m_strDocPath;
					lex->m_strFileName = m_strFileName;
					m_listLexem.push_back(*lex);
				}
				continue;
			}
			const int key_word = IsKeyWord(strCurWord);
			//undefined
			if (key_word == KEY_UNDEFINED) {
				lex.m_nType = CONSTANT;
				lex.m_vData.SetType(eValueTypes::TYPE_EMPTY);
			}
			//boolean
			else if (key_word == KEY_TRUE) {
				lex.m_nType = CONSTANT;
				lex.m_vData.SetBoolean(wxT("true"));
			}
			else if (key_word == KEY_FALSE) {
				lex.m_nType = CONSTANT;
				lex.m_vData.SetBoolean(wxT("false"));
			}
			//null
			else if (key_word == KEY_NULL) {
				lex.m_nType = CONSTANT;
				lex.m_vData.SetType(eValueTypes::TYPE_NULL);
			}

			if (lex.m_nType != CONSTANT) {
				lex.m_vData = strOrig;
				if (key_word >= 0) {
					lex.m_nType = KEYWORD;
					lex.m_nData = key_word;
				}
				else {
					lex.m_nType = IDENTIFIER;
				}
			}
		}
		else if (IsNumber() || IsString() || IsDate()) {
			lex.m_nType = CONSTANT;
			if (IsNumber()) {
				const int curPos = m_nCurPos;
				if (!lex.m_vData.SetNumber(GetNumber())) {
					SetError(ERROR_TRANSLATE_NUMBER, curPos);
				}
				int n = m_listLexem.size() - 1;
				if (n >= 0) {
					if (m_listLexem[n].m_nType == DELIMITER && (m_listLexem[n].m_nData == '-' || m_listLexem[n].m_nData == '+')) {
						n--;
						if (n >= 0) {
							if (m_listLexem[n].m_nType == DELIMITER &&
								(
									m_listLexem[n].m_nData == '[' ||
									m_listLexem[n].m_nData == '(' ||
									m_listLexem[n].m_nData == ',' ||
									m_listLexem[n].m_nData == '<' ||
									m_listLexem[n].m_nData == '>' ||
									m_listLexem[n].m_nData == '=')) {
								n++;
								if (m_listLexem[n].m_nData == '-')
									lex.m_vData.m_fData = -lex.m_vData.m_fData;
								m_listLexem[n] = lex;
								continue;
							}
						}
					}
				}
			}
			else {
				if (IsString()) {
					const int curPos = m_nCurPos;
					if (!lex.m_vData.SetString(GetString())) {
						SetError(ERROR_TRANSLATE_STRING, curPos);
					}
				}
				else if (IsDate()) {
					const int curPos = m_nCurPos;
					if (!lex.m_vData.SetDate(GetDate())) {
						SetError(ERROR_TRANSLATE_DATE, curPos);
					}
				}
			}

			m_listLexem.push_back(lex);
			continue;
		}
		else if (IsByte('~')) {
			strCurWord.clear();
			GetByte();//���������� ����������� � �������. ������ ����� (��� ������)
			continue;
		}
		else {

			strCurWord.clear();

			lex.m_nType = DELIMITER;
			lex.m_nData = GetByte();

			if (lex.m_nData <= 13) {
				continue;
			}
		}
		lex.m_strData = strCurWord;
		if (lex.m_nType == KEYWORD) {
			if (lex.m_nData == KEY_DEFINE && m_nModePreparing != LEXEM_ADDDEF) { //������� ������������� ��������������
				if (!IsWord()) {
					SetError(ERROR_IDENTIFIER_DEFINE, m_nCurPos);
				}
				const wxString& strName = GetWord();
				//��������� �������������� ��������� � ������ �����������
				if (LEXEM_ADD == m_nModePreparing)
					PrepareFromCurrent(LEXEM_ADDDEF, strName);
				else
					PrepareFromCurrent(LEXEM_IGNORE, strName);

				continue;
			}
			else if (lex.m_nData == KEY_UNDEF) {//�������� ��������������
				if (!IsWord()) {
					SetError(ERROR_IDENTIFIER_DEFINE, m_nCurPos);
				}
				const wxString& strName = GetWord();
				m_defineList->RemoveDef(strName);
				continue;
			}
			else if (lex.m_nData == KEY_IFDEF || lex.m_nData == KEY_IFNDEF) { //�������� ��������������	
				if (!IsWord()) {
					SetError(ERROR_IDENTIFIER_DEFINE, m_nCurPos);
				}
				const wxString& strName = GetWord();
				bool bHasDef = m_defineList->HasDefine(strName);
				if (lex.m_nData == KEY_IFNDEF)
					bHasDef = !bHasDef;
				//����������� ���� ���� ���� �� ���������� #else ��� #endif
				int nMode = 0;
				if (bHasDef)
					nMode = LEXEM_ADD;//��������� �������������� ��������� � ������ ������
				else
					nMode = LEXEM_IGNORE;//����� ����������
				PrepareFromCurrent(nMode);
				if (!IsWord()) {
					SetError(ERROR_USE_ENDDEF, m_nCurPos);
				}
				wxString strWord = GetWord();
				if (IsKeyWord(strWord) == KEY_ELSEDEF) {//����� #else
					//��� ��� �����������
					if (!bHasDef)
						nMode = LEXEM_ADD;//��������� �������������� ��������� � ������ ������
					else
						nMode = LEXEM_IGNORE;//����� ����������
					PrepareFromCurrent(nMode);
					if (!IsWord()) {
						SetError(ERROR_USE_ENDDEF, m_nCurPos);
					}
					strWord = GetWord();
				}
				//������� #endif
				if (IsKeyWord(strWord) != KEY_ENDIFDEF) {
					SetError(ERROR_USE_ENDDEF, m_nCurPos);
				}
				continue;
			}
			else if (lex.m_nData == KEY_ENDIFDEF) {//����� ��������� ��������������	
				m_nCurPos = lex.m_nNumberString;//����� �� ��������� ���������� ��������
				break;
			}
			else if (lex.m_nData == KEY_ELSEDEF) {//"�����" ��������� ��������������
				//������������ �� ������ ��������� ���������
				m_nCurPos = lex.m_nNumberString;//����� �� ��������� ���������� ��������
				break;
			}
			else if (lex.m_nData == KEY_REGION) {
				if (!IsWord()) {
					SetError(ERROR_IDENTIFIER_REGION, m_nCurPos);
				}
				/*const wxString &strName  = */GetWord();
				PrepareFromCurrent(LEXEM_ADD);
				if (!IsWord()) {
					SetError(ERROR_USE_ENDREGION, m_nCurPos);
				}
				const wxString& strWord = GetWord();
				//������� #endregion
				if (IsKeyWord(strWord) != KEY_ENDREGION) {
					SetError(ERROR_USE_ENDREGION, m_nCurPos);
				}
				continue;
			}
			else if (lex.m_nData == KEY_ENDREGION) {
				m_nCurPos = lex.m_nNumberString;//����� �� ��������� ���������� ��������
				break;
			}
		}
		m_listLexem.push_back(lex);
	}

	for (auto translateCode : m_listTranslateCode) {
		if (!translateCode->PrepareLexem())
			return false;
		for (auto& lex : translateCode->m_listLexem) {
			if (lex.m_nType != ENDPROGRAM) {
				m_listLexem.push_back(lex);
			}
		}
		translateCode->m_nCurPos = 0;
		translateCode->m_nCurLine = 0;
	}

	lexem_t lex;

	lex.m_nType = ENDPROGRAM;
	lex.m_nData = 0;

	lex.m_strModuleName = m_strModuleName;
	lex.m_strDocPath = m_strDocPath;
	lex.m_strFileName = m_strFileName;
	
	lex.m_nNumberLine = m_nCurLine;
	lex.m_nNumberString = m_nCurPos;

	m_listLexem.push_back(lex);
	return true;
}

/**
 * �������� ������ ������� � ������� �������
 */
void CTranslateCode::PrepareFromCurrent(int nMode, const wxString& strName)
{
	CTranslateCode translate;
	
	translate.m_defineList = m_defineList;
	translate.m_nModePreparing = nMode;
	translate.m_strModuleName = m_strModuleName;
	
	translate.Load(m_strBuffer);

	//��������� ����� ������
	translate.m_nCurLine = m_nCurLine;
	translate.m_nCurPos = m_nCurPos;

	//�������� ����� ������
	if (nMode == LEXEM_ADDDEF) {
		GetStrToEndLine();
		translate.m_bufferSize = m_nCurPos;
	}

	translate.PrepareLexem();

	if (nMode == LEXEM_ADDDEF) {
		m_defineList->SetDefine(strName, &translate.m_listLexem);
		m_nCurLine = translate.m_nCurLine;
	}
	else if (nMode == LEXEM_ADD) {
		for (unsigned int i = 0; i < translate.m_listLexem.size() - 1; i++) {//��� ����� ENDPROGRAM
			m_listLexem.push_back(translate.m_listLexem[i]);
		}

		m_nCurPos = translate.m_nCurPos;
		m_nCurLine = translate.m_nCurLine;
	}
	else {
		m_nCurPos = translate.m_nCurPos;
		m_nCurLine = translate.m_nCurLine;
	}
}

void CTranslateCode::AppendModule(CTranslateCode* module)
{
	auto& it = std::find(m_listTranslateCode.begin(), m_listTranslateCode.end(), module);
	if (it == m_listTranslateCode.end()) {
		m_listTranslateCode.push_back(module);
	}
}

void CTranslateCode::RemoveModule(CTranslateCode* module)
{
	auto& it = std::find(m_listTranslateCode.begin(), m_listTranslateCode.end(), module);
	if (it != m_listTranslateCode.end()) {
		m_listTranslateCode.erase(it);
	}
}

void CTranslateCode::OnSetParent(CTranslateCode* setParent)
{
	if (!m_defineList) {
		m_defineList = new CDefineList;
		m_defineList->SetParent(&s_glDefineList);
		m_bAutoDeleteDefList = true;//������� ������������
	}

	if (setParent) {
		m_defineList->SetParent(setParent->m_defineList);
	}
	else {
		m_defineList->SetParent(&s_glDefineList);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

#include <wx/module.h>

class wxOESKeywordModule : public wxModule
{
public:
	wxOESKeywordModule() : wxModule() {}
	virtual bool OnInit() {
		CTranslateCode::LoadKeyWords();
		return true;
	}
	virtual void OnExit() {}
private:
	wxDECLARE_DYNAMIC_CLASS(wxOESKeywordModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxOESKeywordModule, wxModule)