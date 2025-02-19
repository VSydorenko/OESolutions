////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : parser for autocomplete 
////////////////////////////////////////////////////////////////////////////

#include "codeEditorParser.h"

#pragma warning(disable : 4018)

CParserModule::CParserModule() : CTranslateCode(), m_numCurrentCompile(wxNOT_FOUND)
{
}

bool CParserModule::ParseModule(const wxString& sModule)
{
	m_aContentModule.clear();

	//1. ����� ��������� �� �������
	Load(sModule);

	try {
		PrepareLexem();
	}
	catch (const CBackendException*)
	{
		return false;
	};

	lexem_t lex;

	while ((lex = GETLexem()).m_lexType != ERRORTYPE)
	{
		//���������� �������
		if (lex.m_lexType == KEYWORD && lex.m_numData == KEY_IF)
		{
			while (m_numCurrentCompile + 1 < m_listLexem.size())
			{
				lex = GETLexem();
				if (lex.m_lexType == KEYWORD && lex.m_numData == KEY_THEN) break;
			}

			lex = GETLexem();
		}

		//���������� ��������� ������ WHILE
		if (lex.m_lexType == KEYWORD && lex.m_numData == KEY_WHILE)
		{
			while (m_numCurrentCompile + 1 < m_listLexem.size())
			{
				lex = GETLexem();
				if (lex.m_lexType == KEYWORD && lex.m_numData == KEY_DO) break;
			}
			lex = GETLexem();
		}

		//���������� ��������� ���������
		if (lex.m_lexType == DELIMITER && lex.m_numData == '?')
		{
			while (m_numCurrentCompile + 1 < m_listLexem.size())
			{
				lex = GETLexem();
				if (lex.m_lexType == DELIMITER && lex.m_numData == ';') break;
			}
			lex = GETLexem();
		}

		//���������� ����������
		if (lex.m_lexType == KEYWORD && lex.m_numData == KEY_VAR) {
			while (m_numCurrentCompile + 1 < m_listLexem.size()) {
				wxString strName = GETIdentifier(true);
				int nArrayCount = -1;
				if (IsNextDelimeter('[')) { // this is an array declaration
					nArrayCount = 0;
					GETDelimeter('[');
					if (!IsNextDelimeter(']')) {
						CValue vConst = GETConstant();
						if (vConst.GetType() != eValueTypes::TYPE_NUMBER || vConst.GetNumber() < 0)
							continue;
						nArrayCount = vConst.GetInteger();
					}
					GETDelimeter(']');
				}

				bool bExport = false;

				if (IsNextKeyWord(KEY_EXPORT))
				{
					GETKeyWord(KEY_EXPORT);
					bExport = true;
				}

				if (IsNextDelimeter('='))// initial initialization - works only inside the text of modules (but not re-declaring procedures and functions)
				{
					if (nArrayCount >= 0) GETDelimeter(',');//Error!
					GETDelimeter('=');
				}

				moduleElement_t data;
				data.strName = strName;
				data.nLineStart = lex.m_numLine;
				data.nLineEnd = lex.m_numLine;
				data.nImage = 358;

				if (bExport)
					data.eType = eContentType::eExportVariable;
				else data.eType = eContentType::eVariable;

				m_aContentModule.push_back(data);

				if (!IsNextDelimeter(',')) break;
				GETDelimeter(',');
			}
		}

		//���������� ������� � �������� 
		if (lex.m_lexType == KEYWORD && (lex.m_numData == KEY_FUNCTION || lex.m_numData == KEY_PROCEDURE)) {
			bool isFunction = lex.m_numData == KEY_FUNCTION;
			// pull out the text of the function declaration
			lex = PreviewGetLexem();
			wxString strShortDescription;
			int m_numLine = lex.m_numLine;
			int nRes = m_strBuffer.find('\n', lex.m_numString);
			if (nRes >= 0) {
				strShortDescription = m_strBuffer.Mid(lex.m_numString, nRes - lex.m_numString - 1);
				nRes = strShortDescription.find_first_of('/');
				if (nRes > 0)
				{
					if (strShortDescription[nRes - 1] == '/') {// so this is a comment
						strShortDescription = strShortDescription.Mid(nRes + 1);
					}
				}
				else
				{
					nRes = strShortDescription.find_first_of(')');
					strShortDescription = strShortDescription.Left(nRes + 1);
				}
			}

			wxString strFuncName = GETIdentifier(true);

			// compile the list of formal parameters + register them as local
			GETDelimeter('(');
			if (!IsNextDelimeter(')'))
			{
				while (m_numCurrentCompile + 1 < m_listLexem.size())
				{
					if (IsNextKeyWord(KEY_VAL))
					{
						GETKeyWord(KEY_VAL);
					}

					/*wxString strName =*/ (void)GETIdentifier(true);

					if (IsNextDelimeter('['))// this is an array
					{
						GETDelimeter('[');
						GETDelimeter(']');
					}
					else if (IsNextDelimeter('='))
					{
						GETDelimeter('=');
						CValue vConstant = GETConstant();
					}

					if (IsNextDelimeter(')')) break;

					GETDelimeter(',');
				}
			}

			GETDelimeter(')');

			bool bExport = false;

			if (IsNextKeyWord(KEY_EXPORT))
			{
				GETKeyWord(KEY_EXPORT); bExport = true;
			}

			moduleElement_t data;
			data.strName = strFuncName;
			data.strShortDescription = strShortDescription;
			data.nLineStart = lex.m_numLine;
			data.nLineEnd = lex.m_numLine;

			if (isFunction) {
				data.nImage = 353;
				if (bExport) {
					data.eType = eContentType::eExportFunction;
				}
				else {
					data.eType = eContentType::eFunction;
				}
			}
			else {
				data.nImage = 352;
				if (bExport) { data.eType = eContentType::eExportProcedure; }
				else { data.eType = eContentType::eProcedure; }
			}

			while (m_numCurrentCompile < (m_listLexem.size() - 1)) {

				if (IsNextKeyWord(KEY_ENDFUNCTION)) {
					data.nLineEnd = m_listLexem[m_numCurrentCompile + 1].m_numLine;
					GETKeyWord(KEY_ENDFUNCTION); break;
				}
				else if (IsNextKeyWord(KEY_ENDPROCEDURE)) {
					data.nLineEnd = m_listLexem[m_numCurrentCompile + 1].m_numLine;
					GETKeyWord(KEY_ENDPROCEDURE); break;
				}

				lex = GETLexem();
			}

			m_aContentModule.push_back(data);
		}
	}

	if (m_numCurrentCompile + 1 < m_listLexem.size() - 1)
		return false;
	return true;
}

//variables
wxArrayString CParserModule::GetVariables(bool bOnlyExport)
{
	wxArrayString aVariables;
	for (auto code : m_aContentModule) {
		if (bOnlyExport && code.eType == eContentType::eExportVariable) {
			aVariables.push_back(code.strName);
		}
		else if (!bOnlyExport && (code.eType == eContentType::eExportVariable ||
			code.eType == eContentType::eVariable)) {
			aVariables.push_back(code.strName);
		}
	}
	return aVariables;
}

//functions & procedures 
wxArrayString CParserModule::GetFunctions(bool bOnlyExport)
{
	wxArrayString aFunctions;
	for (auto code : m_aContentModule) {
		if (bOnlyExport && code.eType == eContentType::eExportFunction) {
			aFunctions.push_back(code.strName);
		}
		else if (!bOnlyExport && (code.eType == eContentType::eExportFunction ||
			code.eType == eContentType::eFunction)) {
			aFunctions.push_back(code.strName);
		}
	}
	return aFunctions;
}

wxArrayString CParserModule::GetProcedures(bool bOnlyExport)
{
	wxArrayString aProcedures;

	for (auto code : m_aContentModule) {
		if (bOnlyExport && code.eType == eContentType::eExportProcedure) {
			aProcedures.push_back(code.strName);
		}
		else if (!bOnlyExport && (code.eType == eContentType::eExportProcedure ||
			code.eType == eContentType::eProcedure)) {
			aProcedures.push_back(code.strName);
		}
	}

	return aProcedures;
}

/**
 * GetLexem
 * ����������:
 * �������� ��������� ������� �� ������ ���� ���� � �������� ������� ������� ������� �� 1
 * ������������ ��������:
 * 0 ��� ��������� �� �������
 */
lexem_t CParserModule::GetLexem()
{
	lexem_t lex;
	if (m_numCurrentCompile + 1 < m_listLexem.size()) {
		lex = m_listLexem[++m_numCurrentCompile];
	}
	return lex;
}

//�������� ��������� ������� �� ������ ���� ���� ��� ���������� �������� ������� �������
lexem_t CParserModule::PreviewGetLexem()
{
	lexem_t lex;
	while (true) {
		lex = GetLexem();
		if (!(lex.m_lexType == DELIMITER && (lex.m_numData == ';' || lex.m_numData == '\n')))
			break;
	}
	m_numCurrentCompile--;
	return lex;
}

/**
 * GETLexem
 * ����������:
 * �������� ��������� ������� �� ������ ���� ���� � �������� ������� ������� ������� �� 1
 * ������������ ��������:
 * ��� (� ������ ������� ��������� ����������)
 */
lexem_t CParserModule::GETLexem()
{
	const lexem_t& lex = GetLexem();
	if (lex.m_lexType == ERRORTYPE) {}
	return lex;
}
/**
 * GETDelimeter
 * ����������:
 * �������� ��������� ������� ��� �������� �����������
 * ������������ ��������:
 * ��� (� ������ ������� ��������� ����������)
 */
void CParserModule::GETDelimeter(const wxUniChar& c)
{
	lexem_t lex = GETLexem();
	while (!(lex.m_lexType == DELIMITER && c == lex.m_numData)) {
		if (m_numCurrentCompile + 1 >= m_listLexem.size())
			break;
		lex = GETLexem();
	}
}
/**
 * IsNextDelimeter
 * ����������:
 * ��������� �������� �� ��������� ������� ����-���� �������� ������������
 * ������������ ��������:
 * true,false
 */
bool CParserModule::IsNextDelimeter(const wxUniChar& c)
{
	if (m_numCurrentCompile + 1 < m_listLexem.size()) {
		const lexem_t& lex = m_listLexem[m_numCurrentCompile + 1];
		if (lex.m_lexType == DELIMITER && c == lex.m_numData)
			return true;
	}
	return false;
}

/**
 * IsNextKeyWord
 * ����������:
 * ��������� �������� �� ��������� ������� ����-���� �������� �������� ������
 * ������������ ��������:
 * true,false
 */
bool CParserModule::IsNextKeyWord(int nKey)
{
	if (m_numCurrentCompile + 1 < m_listLexem.size()) {
		const lexem_t& lex = m_listLexem[m_numCurrentCompile + 1];
		if (lex.m_lexType == KEYWORD && lex.m_numData == nKey)
			return true;
	}
	return false;
}

/**
 * GETKeyWord
 * �������� ��������� ������� ��� �������� �������� �����
 * ������������ ��������:
 * ��� (� ������ ������� ��������� ����������)
 */
void CParserModule::GETKeyWord(int nKey)
{
	lexem_t lex = GETLexem();
	while (!(lex.m_lexType == KEYWORD && lex.m_numData == nKey)) {
		if (m_numCurrentCompile + 1 >= m_listLexem.size())
			break;
		lex = GETLexem();
	}
}

/**
 * GETIdentifier
 * �������� ��������� ������� ��� �������� �������� �����
 * ������������ ��������:
 * ������-�������������
 */
wxString CParserModule::GETIdentifier(bool strRealName)
{
	const lexem_t& lex = GETLexem();

	if (lex.m_lexType != IDENTIFIER) {
		if (strRealName && lex.m_lexType == KEYWORD)
			return lex.m_strData;
		return wxEmptyString;
	}

	if (strRealName)
		return lex.m_valData.m_sData;
	else
		return lex.m_strData;
}

/**
 * GETConstant
 * �������� ��������� ������� ��� ���������
 * ������������ ��������:
 * ���������
 */
CValue CParserModule::GETConstant()
{
	lexem_t lex;
	int iNumRequire = 0;
	if (IsNextDelimeter('-') || IsNextDelimeter('+')) {
		iNumRequire = 1;
		if (IsNextDelimeter('-'))
			iNumRequire = -1;
		lex = GETLexem();
	}

	lex = GETLexem();

	if (lex.m_lexType != CONSTANT)
		return lex.m_valData;

	if (iNumRequire) {
		// check that the constant is of numeric type	
		if (lex.m_valData.GetType() != eValueTypes::TYPE_NUMBER)
			return lex.m_valData;
		// change sign for minus
		if (iNumRequire == -1)
			lex.m_valData.m_fData = -lex.m_valData.m_fData;
	}
	return lex.m_valData;
}

