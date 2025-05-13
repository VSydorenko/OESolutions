////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : calculate compile value
////////////////////////////////////////////////////////////////////////////

#include "codeEditorInterpreter.h"
#include "backend/metaData.h"


#pragma warning(push)
#pragma warning(disable : 4018)

//array of mathematical operation priorities
static std::array<int, 256> gs_operPriority = { 0 };

CPrecompileModule::CPrecompileModule(IMetaObjectModule* moduleObject) :
	CTranslateCode(moduleObject->GetFullName(), moduleObject->GetDocPath()),
	m_moduleObject(moduleObject), m_pContext(nullptr), m_pCurrentContext(nullptr),
	m_numCurrentCompile(wxNOT_FOUND), m_nCurrentPos(0), nLastPosition(0),
	m_bCalcValue(false)
{
	if (!gs_operPriority[gs_operPriority.size() - 1]) {

		gs_operPriority['+'] = 10;
		gs_operPriority['-'] = 10;
		gs_operPriority['*'] = 30;
		gs_operPriority['/'] = 30;
		gs_operPriority['%'] = 30;
		gs_operPriority['!'] = 50;

		gs_operPriority[KEY_OR] = 1;
		gs_operPriority[KEY_AND] = 2;

		gs_operPriority['>'] = 3;
		gs_operPriority['<'] = 3;
		gs_operPriority['='] = 3;

		gs_operPriority[gs_operPriority.size() - 1] = true;
	}

	m_strModuleName = m_moduleObject->GetFullName();
	m_strDocPath = m_moduleObject->GetDocPath();
	m_strFileName = m_moduleObject->GetFileName();

	Load(m_moduleObject->GetModuleText());
}

CPrecompileModule::~CPrecompileModule() {}

void CPrecompileModule::Clear() //����� ������ ��� ���������� ������������� �������
{
	m_pCurrentContext = nullptr;
	if (m_defineList != nullptr) m_defineList->Clear();
	m_bufferSize = m_currentPos = m_currentLine = 0;
	for (auto& function : cContext.cFunctions) wxDELETE(function.second);
	m_numCurrentCompile = wxNOT_FOUND;
	cContext.cVariables.clear();
	cContext.cFunctions.clear();

	m_valObject.Reset();
}

#include "backend/compiler/enumFactory.h"
#include "codeEditorParser.h"

void CPrecompileModule::PrepareModuleData()
{
	IModuleDataObject* contextVariable = nullptr;

	if (m_moduleObject) {
		IMetaData* metaData = m_moduleObject->GetMetaData();
		wxASSERT(metaData);
		IModuleManager* moduleManager = metaData->GetModuleManager();
		wxASSERT(moduleManager);
		if (!moduleManager->FindCompileModule(m_moduleObject, contextVariable)) {
			wxASSERT_MSG(false, "CPrecompileModule::PrepareModuleData");
		}
		for (auto pair : moduleManager->GetContextVariables()) {
			//��������� ���������� �� ���������
			CValue* managerVariable = pair.second;
			managerVariable->PrepareNames();
			for (unsigned int i = 0; i < managerVariable->GetNProps(); i++) {
				wxString sAttributeName = managerVariable->GetPropName(i);
				//determine the number and type of the variable
				CPrecompileVariable cVariables;
				cVariables.strName = sAttributeName;
				cVariables.strRealName = sAttributeName;

				cVariables.nNumber = i;
				cVariables.bContext = true;
				cVariables.bExport = true;

				cVariables.m_valContext = managerVariable;

				GetContext()->cVariables[stringUtils::MakeUpper(sAttributeName)] = cVariables;
			}
			//��������� ������ �� ���������
			for (unsigned int i = 0; i < managerVariable->GetNMethods(); i++) {
				wxString sMethodName = managerVariable->GetMethodName(i);
				CPrecompileFunction* pFunction = new CPrecompileFunction(sMethodName);
				pFunction->strRealName = sMethodName;
				pFunction->strShortDescription = managerVariable->GetMethodHelper(i);
				pFunction->nStart = i;
				pFunction->bContext = true;
				pFunction->bExport = true;

				pFunction->m_valContext = managerVariable;

				// check for typing
				GetContext()->cFunctions[stringUtils::MakeUpper(sMethodName)] = pFunction;
			}
		}
		unsigned int nNumberAttr = 0;
		unsigned int nNumberFunc = 0;
		for (auto module : moduleManager->GetCommonModules()) {
			if (module->IsGlobalModule()) {
				CParserModule cParser;
				if (cParser.ParseModule(module->GetModuleText())) {
					for (auto code : cParser.GetAllContent()) {
						if (code.eType == eExportVariable) {
							wxString sAttributeName = code.strName;
							if (cContext.FindVariable(sAttributeName))
								continue;
							//determine the number and type of the variable
							CPrecompileVariable cVariables;
							cVariables.strName = sAttributeName;
							cVariables.strRealName = sAttributeName;

							cVariables.nNumber = nNumberAttr;
							cVariables.bContext = true;
							cVariables.bExport = true;

							cVariables.m_valContext = module;

							GetContext()->cVariables[stringUtils::MakeUpper(sAttributeName)] = cVariables;	nNumberAttr++;
						}
						else if (code.eType == eExportProcedure) {
							wxString sMethodName = code.strName;
							if (cContext.FindFunction(sMethodName))
								continue;
							CPrecompileContext* procContext = new CPrecompileContext(GetContext());//������� ����� ��������, � ������� ����� ������������� ���� �������
							procContext->SetModule(this);
							procContext->nReturn = RETURN_PROCEDURE;

							CPrecompileFunction* pFunction = new CPrecompileFunction(sMethodName, procContext);
							pFunction->strRealName = sMethodName;
							pFunction->strShortDescription = code.strShortDescription;

							pFunction->nStart = nNumberFunc;
							pFunction->bContext = true;
							pFunction->bExport = true;

							pFunction->m_valContext = module;

							// check for typing
							GetContext()->cFunctions[stringUtils::MakeUpper(sMethodName)] = pFunction; nNumberFunc++;
						}
						else if (code.eType == eExportFunction) {
							wxString sMethodName = code.strName;
							if (cContext.FindFunction(sMethodName)) continue;

							CPrecompileContext* procContext = new CPrecompileContext(GetContext());//������� ����� ��������, � ������� ����� ������������� ���� �������
							procContext->SetModule(this);
							procContext->nReturn = RETURN_FUNCTION;

							CPrecompileFunction* pFunction = new CPrecompileFunction(sMethodName, procContext);
							pFunction->strRealName = sMethodName;
							pFunction->strShortDescription = code.strShortDescription;

							pFunction->nStart = nNumberFunc;
							pFunction->bContext = true;
							pFunction->bExport = true;

							pFunction->m_valContext = module;

							// check for typing
							GetContext()->cFunctions[stringUtils::MakeUpper(sMethodName)] = pFunction; nNumberFunc++;
						}
					}
				}
			}
		}
	}

	if (contextVariable != nullptr) {
		CValue* pRefData = nullptr;
		CCompileModule* compileModule = contextVariable->GetCompileModule();
		while (compileModule != nullptr) {
			IMetaObjectModule* moduleObject = compileModule->GetModuleObject();
			if (moduleObject != nullptr) {
				IMetaData* metaData = moduleObject->GetMetaData();
				wxASSERT(metaData);
				IModuleManager* moduleManager = metaData->GetModuleManager();
				if (moduleManager->FindCompileModule(moduleObject, pRefData)) {
					//adding variables from context
					for (long i = 0; i < pRefData->GetNProps(); i++) {
						wxString sAttributeName = pRefData->GetPropName(i);
						if (cContext.FindVariable(sAttributeName))
							continue;

						//determine the number and type of the variable
						CPrecompileVariable cVariables;
						cVariables.strName = sAttributeName;
						cVariables.strRealName = sAttributeName;

						cVariables.nNumber = i;
						cVariables.bContext = true;
						cVariables.bExport = true;

						cVariables.m_valContext = pRefData;

						GetContext()->cVariables[stringUtils::MakeUpper(sAttributeName)] = cVariables;
					}

					// add methods from context
					for (long i = 0; i < pRefData->GetNMethods(); i++) {
						wxString sMethodName = pRefData->GetMethodName(i);
						if (cContext.FindFunction(sMethodName))
							continue;

						CPrecompileContext* procContext = new CPrecompileContext(GetContext());//������� ����� ��������, � ������� ����� ������������� ���� �������
						procContext->SetModule(this);

						if (pRefData->HasRetVal(i))
							procContext->nReturn = RETURN_FUNCTION;
						else
							procContext->nReturn = RETURN_PROCEDURE;

						CPrecompileFunction* pFunction = new CPrecompileFunction(sMethodName, procContext);
						pFunction->strRealName = sMethodName;
						pFunction->strShortDescription = pRefData->GetMethodHelper(i);
						pFunction->nStart = i;
						pFunction->bContext = true;
						pFunction->bExport = true;

						pFunction->m_valContext = pRefData;

						// check for typing
						GetContext()->cFunctions[stringUtils::MakeUpper(sMethodName)] = pFunction;
					}

					if (moduleObject != nullptr) {
						CParserModule cParser;
						if (cParser.ParseModule(moduleObject->GetModuleText())) {
							unsigned int nNumberAttr = pRefData->GetNProps() + 1;
							unsigned int nNumberFunc = pRefData->GetNMethods() + 1;
							for (auto code : cParser.GetAllContent()) {
								if (code.eType == eExportVariable) {
									wxString sAttributeName = code.strName;
									if (cContext.FindVariable(sAttributeName))
										continue;
									//determine the number and type of the variable
									CPrecompileVariable cVariables;
									cVariables.strName = sAttributeName;
									cVariables.strRealName = sAttributeName;

									cVariables.nNumber = nNumberAttr;
									cVariables.bContext = true;
									cVariables.bExport = true;

									cVariables.m_valContext = pRefData;

									GetContext()->cVariables[stringUtils::MakeUpper(sAttributeName)] = cVariables;	nNumberAttr++;
								}
								else if (code.eType == eExportProcedure) {
									wxString sMethodName = code.strName;
									if (cContext.FindFunction(sMethodName))
										continue;

									CPrecompileContext* procContext = new CPrecompileContext(GetContext());//������� ����� ��������, � ������� ����� ������������� ���� �������
									procContext->SetModule(this);
									procContext->nReturn = RETURN_PROCEDURE;

									CPrecompileFunction* pFunction = new CPrecompileFunction(sMethodName, procContext);
									pFunction->strRealName = sMethodName;
									pFunction->strShortDescription = code.strShortDescription;

									pFunction->nStart = nNumberFunc;
									pFunction->bContext = true;
									pFunction->bExport = true;

									pFunction->m_valContext = pRefData;

									// check for typing
									GetContext()->cFunctions[stringUtils::MakeUpper(sMethodName)] = pFunction; nNumberFunc++;
								}
								else if (code.eType == eExportFunction) {
									wxString sMethodName = code.strName;
									if (cContext.FindFunction(sMethodName))
										continue;

									CPrecompileContext* procContext = new CPrecompileContext(GetContext());//������� ����� ��������, � ������� ����� ������������� ���� �������
									procContext->SetModule(this);
									procContext->nReturn = RETURN_FUNCTION;

									CPrecompileFunction* pFunction = new CPrecompileFunction(sMethodName, procContext);
									pFunction->strRealName = sMethodName;
									pFunction->strShortDescription = code.strShortDescription;

									pFunction->nStart = nNumberFunc;
									pFunction->bContext = true;
									pFunction->bExport = true;

									pFunction->m_valContext = pRefData;

									// check for typing
									GetContext()->cFunctions[stringUtils::MakeUpper(sMethodName)] = pFunction; nNumberFunc++;
								}
							}
						}
					}
				}
			}
			compileModule = compileModule->GetParent();
		}
	}
}

bool CPrecompileModule::PrepareLexem()
{
	wxString s;
	m_listLexem.clear();

	while (!IsEnd()) {
		CLexem bytecode;
		bytecode.m_numLine = m_currentLine;
		bytecode.m_numString = m_currentPos;//���� � ���������� ���������� ������, �� ������ ��� ������ ����� ������ ������������
		bytecode.m_strModuleName = m_strModuleName;

		if (IsWord()) {
			wxString sOrig;
			s = GetWord(false, false, &sOrig);

			//undefined
			if (s.Lower() == wxT("undefined")) {
				bytecode.m_lexType = CONSTANT;
				bytecode.m_valData.SetType(eValueTypes::TYPE_EMPTY);
			}
			//boolean
			else if (s.Lower() == wxT("true") || s.Lower() == wxT("false")) {
				bytecode.m_lexType = CONSTANT;
				bytecode.m_valData.SetBoolean(s);
			}
			//null
			else if (s.Lower() == wxT("null")) {
				bytecode.m_lexType = CONSTANT;
				bytecode.m_valData.SetType(eValueTypes::TYPE_NULL);
			}

			if (bytecode.m_lexType != CONSTANT) {
				int n = IsKeyWord(s);
				bytecode.m_valData = sOrig;
				if (n >= 0) {
					bytecode.m_lexType = KEYWORD;
					bytecode.m_numData = n;
				}
				else {
					bytecode.m_lexType = IDENTIFIER;
				}
			}
		}
		else if (IsNumber() || IsString() || IsDate()) {
			bytecode.m_lexType = CONSTANT;
			if (IsNumber()) {
				bytecode.m_valData.SetNumber(GetNumber());
				int n = m_listLexem.size() - 1;
				if (n >= 0) {
					if (m_listLexem[n].m_lexType == DELIMITER && (m_listLexem[n].m_numData == '-' || m_listLexem[n].m_numData == '+')) {
						n--;
						if (n >= 0) {
							if (m_listLexem[n].m_lexType == DELIMITER && (m_listLexem[n].m_numData == '[' || m_listLexem[n].m_numData == '(' || m_listLexem[n].m_numData == ',' || m_listLexem[n].m_numData == '<' || m_listLexem[n].m_numData == '>' || m_listLexem[n].m_numData == '='))
							{
								n++;
								if (m_listLexem[n].m_numData == '-')
									bytecode.m_valData.m_fData = -bytecode.m_valData.m_fData;
								m_listLexem[n] = bytecode;
								continue;
							}
						}
					}
				}
			}
			else {
				if (IsString()) {
					bytecode.m_valData.SetString(GetString());
				}
				else if (IsDate()) {
					bytecode.m_valData.SetDate(GetDate());
				}
			}

			m_listLexem.push_back(bytecode);
			continue;
		}
		else if (IsByte('~')) {
			s.clear();

			GetByte();//���������� ����������� � �������. ������ ����� (��� ������)
			continue;
		}
		else {
			s.clear();
			bytecode.m_lexType = DELIMITER;
			bytecode.m_numData = GetByte();
			if (bytecode.m_numData <= 13) {
				continue;
			}
		}
		bytecode.m_strData = s;
		if (bytecode.m_lexType == KEYWORD)
		{
			if (bytecode.m_numData == KEY_DEFINE)continue; //������� ������������� ��������������
			else if (bytecode.m_numData == KEY_UNDEF) continue; //�������� ��������������
			else if (bytecode.m_numData == KEY_IFDEF || bytecode.m_numData == KEY_IFNDEF) continue; //�������� ��������������
			else if (bytecode.m_numData == KEY_ENDIFDEF) continue; //����� ��������� ��������������
			else if (bytecode.m_numData == KEY_ELSEDEF) continue; //"�����" ��������� ��������������
			else if (bytecode.m_numData == KEY_REGION) continue;
			else if (bytecode.m_numData == KEY_ENDREGION) continue;
		}
		m_listLexem.push_back(bytecode);
	}

	CLexem bytecode;
	bytecode.m_lexType = ENDPROGRAM;
	bytecode.m_numData = 0;
	bytecode.m_numString = m_currentPos;
	m_listLexem.push_back(bytecode);

	return true;
}

void CPrecompileModule::PatchLexem(unsigned int line, int offsetLine, unsigned int offsetString, unsigned int modFlags)
{
	unsigned int nLexPos = m_listLexem.size() > 1 ? m_listLexem.size() - 2 : 0;
	for (unsigned int i = 0; i <= m_listLexem.size() - 1; i++) {
		if (m_listLexem[i].m_numLine >= line) {
			nLexPos = i;
			break;
		}
	}

	for (unsigned int i = nLexPos; i < m_listLexem.size() - 1; i++) {
		if ((modFlags & wxSTC_MOD_BEFOREINSERT) != 0 && m_listLexem[i].m_numLine <= line) {
			if (m_listLexem[i].m_numLine != line && m_listLexem[i + 1].m_lexType == ENDPROGRAM) {
				m_currentLine = m_listLexem[i].m_numLine;
				m_currentPos = m_listLexem[i].m_numString;
			}
			m_listLexem.erase(m_listLexem.begin() + i); i--;
		}
		else if ((modFlags & wxSTC_MOD_BEFOREDELETE) != 0 && m_listLexem[i].m_numLine <= (line - offsetLine)) {
			m_listLexem.erase(m_listLexem.begin() + i); i--;
		}
		else break;
	}

	while (!IsEnd()) {
		if ((modFlags & wxSTC_MOD_BEFOREINSERT) != 0 && m_currentLine > (line + offsetLine)) break;
		else if ((modFlags & wxSTC_MOD_BEFOREDELETE) != 0 && (m_currentLine > line)) break;

		CLexem bytecode;
		bytecode.m_numLine = m_currentLine;
		bytecode.m_numString = m_currentPos; //���� � ���������� ���������� ������, �� ������ ��� ������ ����� ������ ������������
		bytecode.m_strModuleName = m_strModuleName;

		wxString s;

		if (IsWord())
		{
			wxString sOrig;
			s = GetWord(false, false, &sOrig);

			//undefined
			if (s.Lower() == wxT("undefined")) {
				bytecode.m_lexType = CONSTANT;
				bytecode.m_valData.SetType(eValueTypes::TYPE_EMPTY);
			}
			//boolean
			else if (s.Lower() == wxT("true") || s.Lower() == wxT("false")) {
				bytecode.m_lexType = CONSTANT;
				bytecode.m_valData.SetBoolean(s);
			}
			//null
			else if (s.Lower() == wxT("null")) {
				bytecode.m_lexType = CONSTANT;
				bytecode.m_valData.SetType(eValueTypes::TYPE_NULL);
			}

			if (bytecode.m_lexType != CONSTANT) {
				int n = IsKeyWord(s);

				bytecode.m_valData = sOrig;

				if (n >= 0) {
					bytecode.m_lexType = KEYWORD;
					bytecode.m_numData = n;
				}
				else
				{
					bytecode.m_lexType = IDENTIFIER;
				}
			}
		}
		else if (IsNumber() || IsString() || IsDate())
		{
			bytecode.m_lexType = CONSTANT;

			if (IsNumber()) {
				bytecode.m_valData.SetNumber(GetNumber());

				int n = nLexPos;

				if (n >= 0)
				{
					if (m_listLexem[n].m_lexType == DELIMITER && (m_listLexem[n].m_numData == '-' || m_listLexem[n].m_numData == '+'))
					{
						n--;
						if (n >= 0)
						{
							if (m_listLexem[n].m_lexType == DELIMITER && (m_listLexem[n].m_numData == '[' || m_listLexem[n].m_numData == '(' || m_listLexem[n].m_numData == ',' || m_listLexem[n].m_numData == '<' || m_listLexem[n].m_numData == '>' || m_listLexem[n].m_numData == '='))
							{
								n++;
								if (m_listLexem[n].m_numData == '-')
									bytecode.m_valData.m_fData = -bytecode.m_valData.m_fData;
								m_listLexem[n] = bytecode;
								continue;
							}
						}
					}
				}
			}
			else {
				if (IsString()) {
					bytecode.m_valData.SetString(GetString());
				}
				else if (IsDate()) {
					bytecode.m_valData.SetDate(GetDate());
				}
			}
			m_listLexem.insert(m_listLexem.begin() + nLexPos, bytecode);
			nLexPos++;
			continue;
		}
		else if (IsByte('~')) {
			s.clear();

			GetByte();//���������� ����������� � �������. ������ ����� (��� ������)
			nLexPos++;

			continue;
		}
		else {
			s.clear();
			bytecode.m_lexType = DELIMITER;
			bytecode.m_numData = GetByte();
			if (bytecode.m_numData <= 13) {
				nLexPos++;
				continue;
			}
		}
		bytecode.m_strData = s;
		if (bytecode.m_lexType == KEYWORD) {
			if (
				bytecode.m_numData == KEY_DEFINE //������� ������������� ��������������
				|| bytecode.m_numData == KEY_UNDEF //�������� ��������������
				|| (bytecode.m_numData == KEY_IFDEF || bytecode.m_numData == KEY_IFNDEF)  //�������� ��������������
				|| bytecode.m_numData == KEY_ENDIFDEF  //����� ��������� ��������������
				|| bytecode.m_numData == KEY_ELSEDEF  //"�����" ��������� ��������������
				|| bytecode.m_numData == KEY_REGION
				|| bytecode.m_numData == KEY_ENDREGION
				)
			{
				continue;
			}
		}

		m_listLexem.insert(m_listLexem.begin() + nLexPos, bytecode); nLexPos++;
	}

	for (unsigned int i = nLexPos; i < m_listLexem.size() - 1; i++) {
		m_listLexem[i].m_numLine += offsetLine;
		if ((modFlags & wxSTC_MOD_BEFOREINSERT) != 0) {
			m_listLexem[i].m_numString += offsetString;
		}
		else {
			m_listLexem[i].m_numString -= offsetString;
		}
	}

	if ((modFlags & wxSTC_MOD_BEFOREINSERT) != 0) {
		m_listLexem[m_listLexem.size() - 1].m_numString += offsetString;
	}
	else {
		m_listLexem[m_listLexem.size() - 1].m_numString -= offsetString;
	}
}

bool CPrecompileModule::Compile()
{
	Clear();

	//���������� ���������� ��������
	IMetaData* metaData = m_moduleObject->GetMetaData();
	wxASSERT(metaData);
	IModuleManager* moduleManager = metaData->GetModuleManager();
	wxASSERT(moduleManager);

	for (auto variable : moduleManager->GetGlobalVariables()) {
		AddVariable(variable.first, variable.second);
	}

	PrepareModuleData();

	return CompileModule();
}

bool CPrecompileModule::CompileModule()
{
	m_pContext = GetContext();// context of the module itself

	CLexem lex;

	while ((lex = PreviewGetLexem()).m_lexType != ERRORTYPE)
	{
		if ((KEYWORD == lex.m_lexType && KEY_VAR == lex.m_numData) || (IDENTIFIER == lex.m_lexType && IsTypeVar(lex.m_strData)))
		{
			CompileDeclaration();// load variable declaration
		}
		else if (KEYWORD == lex.m_lexType && (KEY_PROCEDURE == lex.m_numData || KEY_FUNCTION == lex.m_numData))
		{
			CompileFunction();// load function declaration
			// don't forget to restore the current module context (if necessary)...
		}
		else
		{
			break;
		}
	}

	int nStartContext = m_numCurrentCompile >= 0 ? m_listLexem[m_numCurrentCompile].m_numString : 0;

	// load the executable body of the module
	m_pContext = GetContext();// context of the module itself

	CompileBlock();

	if (m_numCurrentCompile + 1 < m_listLexem.size() - 1) return false;

	if (m_nCurrentPos >= nStartContext && m_nCurrentPos <= m_listLexem[m_numCurrentCompile].m_numString)
	{
		m_pCurrentContext = m_pContext;
	}

	return true;
}

bool CPrecompileModule::CompileFunction()
{
	// we are now at the token level, where the FUNCTION or PROCEDURE keyword is specified
	CLexem lex;
	if (IsNextKeyWord(KEY_FUNCTION))
	{
		GETKeyWord(KEY_FUNCTION);

		m_pContext = new CPrecompileContext(GetContext());//������� ����� ��������, � ������� ����� ������������� ���� �������
		m_pContext->SetModule(this);
		m_pContext->nReturn = RETURN_FUNCTION;
	}
	else if (IsNextKeyWord(KEY_PROCEDURE))
	{
		GETKeyWord(KEY_PROCEDURE);

		m_pContext = new CPrecompileContext(GetContext());//������� ����� ��������, � ������� ����� ������������� ���� ���������
		m_pContext->SetModule(this);
		m_pContext->nReturn = RETURN_PROCEDURE;
	}
	else
	{
		/*SetError(ERROR_FUNC_DEFINE);*/
		return false;
	}

	// pull out the text of the function declaration
	lex = PreviewGetLexem();
	wxString strShortDescription;
	int m_numLine = lex.m_numLine;
	int nRes = m_strBuffer.find('\n', lex.m_numString);
	if (nRes >= 0)
	{
		strShortDescription = m_strBuffer.Mid(lex.m_numString, nRes - lex.m_numString - 1);
		nRes = strShortDescription.find_first_of('/');
		if (nRes > 0)
		{
			if (strShortDescription[nRes - 1] == '/')// so this is a comment
			{
				strShortDescription = strShortDescription.Mid(nRes + 1);
			}
		}
		else
		{
			nRes = strShortDescription.find_first_of(')');
			strShortDescription = strShortDescription.Left(nRes + 1);
		}
	}

	// get the function name
	wxString csFuncName0 = GETIdentifier(true);
	wxString strFuncName = stringUtils::MakeUpper(csFuncName0);
	int nError = m_numCurrentCompile;

	CPrecompileFunction* pFunction = new CPrecompileFunction(strFuncName, m_pContext);

	pFunction->strRealName = csFuncName0;
	pFunction->strShortDescription = strShortDescription;
	pFunction->nNumberLine = m_numLine;

	// compile the list of formal parameters + register them as local
	GETDelimeter('(');
	while (m_numCurrentCompile + 1 < m_listLexem.size()
		&& !IsNextDelimeter(')'))
	{
		while (m_numCurrentCompile + 1 < m_listLexem.size())
		{
			wxString strType;
			// check for typing
			if (IsTypeVar())
			{
				strType = GetTypeVar();
			}

			CParamValue sVariable;

			if (IsNextKeyWord(KEY_VAL))
			{
				GETKeyWord(KEY_VAL);
			}

			wxString strRealName = GETIdentifier(true);
			sVariable.m_paramName = stringUtils::MakeUpper(strRealName);

			// register this variable as local
			if (m_pContext->FindVariable(sVariable.m_paramName)) return false;//���� ���������� + ��������� ���������� = ������

			if (IsNextDelimeter('[')) { // this is an array
				GETDelimeter('[');
				GETDelimeter(']');
			}
			else if (IsNextDelimeter('=')) {
				GETDelimeter('=');
				GETConstant();
			}

			CValue valObject;

			if (!strType.IsEmpty()) {
				try {
					valObject = CValue::CreateObject(strType);
				}
				catch (...)
				{
				}
			}

			m_pContext->AddVariable(strRealName, strType, false, false, valObject);
			sVariable.m_paramType = strType;

			pFunction->aParamList.push_back(sVariable);

			if (IsNextDelimeter(')'))
				break;

			GETDelimeter(',');
		}
	}

	GETDelimeter(')');

	if (IsNextKeyWord(KEY_EXPORT)) {
		GETKeyWord(KEY_EXPORT);
		pFunction->bExport = true;
	}

	// check for typing
	GetContext()->cFunctions[strFuncName] = pFunction;

	int nStartContext = m_listLexem[m_numCurrentCompile].m_numString;

	GetContext()->sCurFuncName = strFuncName;
	CompileBlock();
	GetContext()->sCurFuncName = wxEmptyString;

	if (m_pContext->nReturn == RETURN_FUNCTION) GETKeyWord(KEY_ENDFUNCTION);
	else GETKeyWord(KEY_ENDPROCEDURE);

	if (m_nCurrentPos >= nStartContext && m_nCurrentPos <= m_listLexem[m_numCurrentCompile].m_numString) m_pCurrentContext = m_pContext;
	return true;
}

bool CPrecompileModule::CompileDeclaration()
{
	wxString strType;
	const CLexem& lex = PreviewGetLexem();

	if (IDENTIFIER == lex.m_lexType) strType = GetTypeVar(); // typed setting of variables
	else GETKeyWord(KEY_VAR);

	while (m_numCurrentCompile + 1 < m_listLexem.size())
	{
		wxString strName = GETIdentifier(true);

		int nArrayCount = wxNOT_FOUND;
		if (IsNextDelimeter('['))// this is an array declaration
		{
			nArrayCount = 0;
			GETDelimeter('[');
			if (!IsNextDelimeter(']')) {
				CValue vConst = GETConstant();
				if (vConst.GetType() != eValueTypes::TYPE_NUMBER || vConst.GetNumber() < 0)
					return false;
				nArrayCount = vConst.GetInteger();
			}
			GETDelimeter(']');
		}

		bool bExport = false;

		if (IsNextKeyWord(KEY_EXPORT)) {
			if (bExport) break;// there was an Export announcement
			GETKeyWord(KEY_EXPORT);
			bExport = true;
		}

		// there was no variable declaration yet - add
		m_pContext->AddVariable(strName, strType, bExport);

		if (IsNextDelimeter('='))// initial initialization - works only inside the text of modules (but not re-declaring procedures and functions)
		{
			if (nArrayCount >= 0) GETDelimeter(',');//Error!
			GETDelimeter('=');
		}

		if (!IsNextDelimeter(','))
			break;

		GETDelimeter(',');
	}

	return true;
}

bool CPrecompileModule::CompileBlock()
{
	CLexem lex;

	while ((lex = PreviewGetLexem()).m_lexType != ERRORTYPE)
	{
		if (IDENTIFIER == lex.m_lexType && IsTypeVar(lex.m_strData)) CompileDeclaration();

		if (KEYWORD == lex.m_lexType)
		{
			switch (lex.m_numData)
			{
			case KEY_VAR:// setting variables and arrays
				CompileDeclaration();
				break;
			case KEY_NEW:
				CompileNewObject();
				break;
			case KEY_IF:
				CompileIf();
				break;
			case KEY_WHILE:
				CompileWhile();
				break;
			case KEY_FOREACH:
				CompileForeach();
				break;
			case KEY_FOR:
				CompileFor();
				break;
			case KEY_GOTO:
				CompileGoto();
				break;
			case KEY_RETURN:
			{
				GETKeyWord(KEY_RETURN);

				if (m_pContext->nReturn == RETURN_NONE)
					return false;

				if (m_pContext->nReturn == RETURN_FUNCTION)//������������ �����-�� ��������
				{
					if (IsNextDelimeter(';')) return false;

					CParamValue returnValue = GetExpression();

					if (!cContext.sCurFuncName.IsEmpty())
					{
						CPrecompileFunction* m_precompile = static_cast<CPrecompileFunction*>(cContext.cFunctions[cContext.sCurFuncName]);
						m_precompile->RealRetValue = returnValue;
					}
				}
				break;
			}
			case KEY_TRY:
			{
				GETKeyWord(KEY_TRY);
				CompileBlock();

				GETKeyWord(KEY_EXCEPT);
				CompileBlock();
				GETKeyWord(KEY_ENDTRY);

				break;
			}

			case KEY_RAISE: GETKeyWord(KEY_RAISE); break;
			case KEY_CONTINUE: GETKeyWord(KEY_CONTINUE); break;
			case KEY_BREAK: GETKeyWord(KEY_BREAK); break;

			case KEY_FUNCTION:
			case KEY_PROCEDURE: GetLexem(); break;

			default: return true;// means the operator bracket ending this block has been encountered (for example, ENDIF, ENDDO, ENDFUNCTION, etc.)
			}
		}
		else
		{
			lex = GetLexem();
			if (IDENTIFIER == lex.m_lexType)
			{
				if (IsNextDelimeter(':'))// this is a label task encountered
				{
					// write the address of the label:
					GETDelimeter(':');
				}
				else//����� �������������� ������ �������, �������, ������������� ���������
				{
					m_numCurrentCompile--;// step back

					int nSet = 1;
					CParamValue sVariable = GetCurrentIdentifier(nSet);//�������� ����� ����� ��������� (�� ����� '=')
					if (nSet)//���� ���� ������ �����, �.�. ���� '='
					{
						GETDelimeter('=');//��� ������������ ���������� ������-�� ���������

						CParamValue expression = GetExpression();
						sVariable.m_paramType = expression.m_paramType;
						sVariable.m_paramObject = expression.m_paramObject;

						if (m_pContext->FindVariable(sVariable.m_paramName)) {
							m_pContext->cVariables[sVariable.m_paramName].m_valObject = expression.m_paramObject;
						}
						else
						{
							m_pContext->AddVariable(sVariable.m_paramName, expression.m_paramType, false, false, expression.m_paramObject);
						}
					}
				}
			}
			else if (DELIMITER == lex.m_lexType && ';' == lex.m_numData) break;
			else if (ENDPROGRAM == lex.m_lexType) break;
			else return false;
		}
	}//while

	return true;
}

bool CPrecompileModule::CompileNewObject()
{
	GETKeyWord(KEY_NEW);

	wxString objectName = GETIdentifier(true);
	int nNumber = GetConstString(objectName);

	std::vector <CParamValue> aParamList;

	if (IsNextDelimeter('('))// this is a method call
	{
		GETDelimeter('(');

		while (m_numCurrentCompile + 1 < m_listLexem.size()
			&& !IsNextDelimeter(')'))
		{
			if (IsNextDelimeter(','))
			{
				CParamValue data; // missing parameter
				aParamList.push_back(data);
			}
			else
			{
				aParamList.emplace_back(GetExpression());

				if (IsNextDelimeter(')')) break;
			}
			GETDelimeter(',');
		}

		GETDelimeter(')');
	}

	return true;
}

bool CPrecompileModule::CompileGoto()
{
	GETKeyWord(KEY_GOTO);
	return true;
}

bool CPrecompileModule::CompileIf()
{
	GETKeyWord(KEY_IF);

	GetExpression();

	GETKeyWord(KEY_THEN);
	CompileBlock();

	while (IsNextKeyWord(KEY_ELSEIF))
	{
		// write the output from all checks for the previous block
		GETKeyWord(KEY_ELSEIF);

		GetExpression();

		GETKeyWord(KEY_THEN);
		CompileBlock();
	}

	if (IsNextKeyWord(KEY_ELSE))
	{
		// write the output from all checks for the previous block
		GETKeyWord(KEY_ELSE);
		CompileBlock();
	}

	GETKeyWord(KEY_ENDIF);
	return true;
}

bool CPrecompileModule::CompileWhile()
{
	GETKeyWord(KEY_WHILE);

	GetExpression();

	GETKeyWord(KEY_DO);
	CompileBlock();
	GETKeyWord(KEY_ENDDO);

	return true;
}

bool CPrecompileModule::CompileFor()
{
	GETKeyWord(KEY_FOR);

	int nStartPos = m_listLexem[m_numCurrentCompile].m_numString;

	wxString strRealName = GETIdentifier(true);
	//wxString strName = stringUtils::MakeUpper(strRealName);

	CParamValue sVariable = GetVariable(strRealName);

	if (IsNextDelimeter('='))
		GETDelimeter('=');

	GetExpression();

	GETKeyWord(KEY_TO);
	CParamValue VariableTo = GetExpression();

	GETKeyWord(KEY_DO);
	CompileBlock();
	GETKeyWord(KEY_ENDDO);

	if (!(nStartPos < m_nCurrentPos && m_listLexem[m_numCurrentCompile].m_numString > m_nCurrentPos))
		m_pContext->RemoveVariable(strRealName);

	return true;
}

bool CPrecompileModule::CompileForeach()
{
	GETKeyWord(KEY_FOREACH);

	int nStartPos = m_listLexem[m_numCurrentCompile].m_numString;

	wxString strRealName = GETIdentifier(true);
	wxString strName = stringUtils::MakeUpper(strRealName);

	CParamValue sVariable = GetVariable(strRealName);

	GETKeyWord(KEY_IN);

	CParamValue VariableTo = GetExpression();
	m_pContext->cVariables[strName].m_valObject = VariableTo.m_paramObject.GetIteratorEmpty();

	GETKeyWord(KEY_DO);
	CompileBlock();
	GETKeyWord(KEY_ENDDO);

	if (!(nStartPos < m_nCurrentPos && m_listLexem[m_numCurrentCompile].m_numString > m_nCurrentPos))
		m_pContext->RemoveVariable(strRealName);

	return true;
}

//////////////////////////////////////////////////////////////////////
// Compiling
//////////////////////////////////////////////////////////////////////

/**
 * GetLexem
 * ����������:
 * �������� ��������� ������� �� ������ ���� ���� � �������� ������� ������� ������� �� 1
 * ������������ ��������:
 * 0 ��� ��������� �� �������
 */
CLexem CPrecompileModule::GetLexem()
{
	CLexem lex;
	if (m_numCurrentCompile + 1 < m_listLexem.size()) {
		lex = m_listLexem[++m_numCurrentCompile];
	}
	return lex;
}

//�������� ��������� ������� �� ������ ���� ���� ��� ���������� �������� ������� �������
CLexem CPrecompileModule::PreviewGetLexem()
{
	CLexem lex;
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
CLexem CPrecompileModule::GETLexem()
{
	const CLexem& lex = GetLexem();
	if (lex.m_lexType == ERRORTYPE) {}
	return lex;
}
/**
 * GETDELIMITER
 * ����������:
 * �������� ��������� ������� ��� �������� �����������
 * ������������ ��������:
 * ��� (� ������ ������� ��������� ����������)
 */
void CPrecompileModule::GETDelimeter(const wxUniChar& c)
{
	CLexem lex = GETLexem();

	if (lex.m_lexType == DELIMITER && c == lex.m_numData)
		sLastExpression += c;

	while (!(lex.m_lexType == DELIMITER && c == lex.m_numData)) {
		if (m_numCurrentCompile + 1 >= m_listLexem.size()) break;
		lex = GETLexem();
	}
}
/**
 * IsNextDELIMITER
 * ����������:
 * ��������� �������� �� ��������� ������� ����-���� �������� ������������
 * ������������ ��������:
 * true,false
 */
bool CPrecompileModule::IsNextDelimeter(const wxUniChar& c)
{
	if (m_numCurrentCompile + 1 < m_listLexem.size()) {
		CLexem lex = m_listLexem[m_numCurrentCompile + 1];
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
bool CPrecompileModule::IsNextKeyWord(int nKey)
{
	if (m_numCurrentCompile + 1 < m_listLexem.size()) {
		const CLexem& lex = m_listLexem[m_numCurrentCompile + 1];
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
void CPrecompileModule::GETKeyWord(int nKey)
{
	CLexem lex = GETLexem();
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
wxString CPrecompileModule::GETIdentifier(bool strRealName)
{
	const CLexem& lex = GETLexem();
	if (lex.m_lexType != IDENTIFIER) {
		if (strRealName && lex.m_lexType == KEYWORD)
			return lex.m_strData;
		return wxEmptyString;
	}

	if (strRealName) return lex.m_valData.m_sData;
	else return lex.m_strData;
}

/**
 * GETConstant
 * �������� ��������� ������� ��� ���������
 * ������������ ��������:
 * ���������
 */
CValue CPrecompileModule::GETConstant()
{
	CLexem lex;
	int iNumRequire = 0;
	if (IsNextDelimeter('-') || IsNextDelimeter('+')) {
		iNumRequire = 1;
		if (IsNextDelimeter('-'))
			iNumRequire = wxNOT_FOUND;
		lex = GETLexem();
	}

	lex = GETLexem();

	if (iNumRequire) {
		// check that the constant is of numeric type	
		if (lex.m_valData.GetType() != eValueTypes::TYPE_NUMBER) {}
		// change sign for minus
		if (iNumRequire == wxNOT_FOUND)
			lex.m_valData.m_fData = -lex.m_valData.m_fData;
	}
	return lex.m_valData;
}

// getting the number with a string constant (to determine the method number)
int CPrecompileModule::GetConstString(const wxString& sMethod)
{
	if (!m_aHashConstList[sMethod])
	{
		m_aHashConstList[sMethod] = m_aHashConstList.size();
	}
	return ((int)m_aHashConstList[sMethod]) - 1;
}

int CPrecompileModule::IsTypeVar(const wxString& strType)
{
	if (!strType.IsEmpty()) {
		if (CValue::IsRegisterCtor(strType, eCtorObjectType::eCtorObjectType_object_primitive))
			return true;
	}
	else {
		const CLexem& lex = PreviewGetLexem();
		if (CValue::IsRegisterCtor(lex.m_strData, eCtorObjectType::eCtorObjectType_object_primitive))
			return true;
	}

	return false;
}

wxString CPrecompileModule::GetTypeVar(const wxString& strType)
{
	if (!strType.IsEmpty()) {
		if (CValue::IsRegisterCtor(strType, eCtorObjectType::eCtorObjectType_object_primitive))
			return strType.Upper();
	}
	else {
		const CLexem& lex = GETLexem();
		if (CValue::IsRegisterCtor(lex.m_strData, eCtorObjectType::eCtorObjectType_object_primitive))
			return lex.m_strData.Upper();
	}

	return wxEmptyString;
}

#define SetOper(x) nOper=x;

/**
 * ���������� ������������� ��������� (��������� ������ �� ����� �������)
 */
CParamValue CPrecompileModule::GetExpression(int nPriority)
{
	CParamValue sVariable;
	CLexem lex = GETLexem();

	// first we process Left operators
	if ((lex.m_lexType == KEYWORD && lex.m_numData == KEY_NOT) ||
		(lex.m_lexType == DELIMITER && lex.m_numData == '!')) {
		sVariable = GetVariable();
		CParamValue sVariable2 = GetExpression(gs_operPriority['!']);
		sVariable.m_paramType = wxT("NUMBER");
	}
	else if ((lex.m_lexType == KEYWORD && lex.m_numData == KEY_NEW)) {

		const wxString& objectName = GETIdentifier();
		std::vector <CParamValue> aParamList;


		if (IsNextDelimeter('(')) { // this is a method call	
			GETDelimeter('(');
			while (m_numCurrentCompile + 1 < m_listLexem.size()
				&& !IsNextDelimeter(')')) {
				if (IsNextDelimeter(',')) {
					CParamValue data;
					//data.nArray = DEF_VAR_SKIP;// missing parameter
					//data.nIndex = DEF_VAR_SKIP;
					aParamList.push_back(data);
				}
				else {
					aParamList.emplace_back(GetExpression());
					if (IsNextDelimeter(')')) break;
				}
				GETDelimeter(',');
			}
			GETDelimeter(')');
		}

		CValue** pRefLocVars = aParamList.size() ? new CValue * [aParamList.size()] : nullptr;
		for (unsigned int i = 0; i < aParamList.size(); i++) {
			pRefLocVars[i] = &aParamList[i].m_paramObject;
		}

		try {
			sVariable.m_paramObject = CValue::CreateObject(objectName,
				pRefLocVars, aParamList.size()
			);
		}
		catch (...) {
		}

		if (pRefLocVars != nullptr)
			delete[]pRefLocVars;

		return sVariable;
	}
	else if (lex.m_lexType == DELIMITER && lex.m_numData == '(')
	{
		sVariable = GetExpression();
		GETDelimeter(')');
	}
	else if (lex.m_lexType == DELIMITER && lex.m_numData == '?')
	{
		sVariable = GetVariable();
		//CByteUnit code;
		//AddLineInfo(code);
		//code.nOper = OPER_ITER;
		/*code.Param1 = sVariable;*/
		GETDelimeter('(');
		/*code.Param2 =*/ GetExpression();
		GETDelimeter(',');
		/*code.Param3 *=*/ GetExpression();
		GETDelimeter(',');
		/*code.Param4 = */GetExpression();
		GETDelimeter(')');
		//cByteCode.CodeList.push_back(code);
	}
	else if (lex.m_lexType == IDENTIFIER)
	{
		m_numCurrentCompile--;// step back
		int nSet = 0;
		sVariable = GetCurrentIdentifier(nSet);
	}
	else if (lex.m_lexType == CONSTANT)
	{
		sVariable = FindConst(lex.m_valData);
	}
	else if ((lex.m_lexType == DELIMITER && lex.m_numData == '+') || (lex.m_lexType == DELIMITER && lex.m_numData == '-'))
	{
		//��������� ������������ ������ // check the admissibility of such assignment
		int nCurPriority = gs_operPriority[lex.m_numData];

		if (nPriority >= nCurPriority)
			return sVariable; //���������� ���������� ����� (���������� ��������) � ������� ����������� ��������

		//��� ������� ������������� ����� ���������
		if (lex.m_numData == '+')// do nothing (ignore)
		{
			sVariable = GetExpression(nPriority);
			sVariable.m_paramType = wxT("NUMBER");
			return sVariable;
		}
		else
		{
			sVariable = GetExpression(100);//super high priority!
			sVariable = GetVariable();
			sVariable.m_paramType = wxT("NUMBER");
		}
	}

	//������ ������������ ������ ���������
	//���� � sVariable ����� ������ ������ ���������� ���������

MOperation:

	lex = PreviewGetLexem();

	if (lex.m_lexType == DELIMITER && lex.m_numData == ')') return sVariable;

	//������� ���� �� ����� ��������� ���������� �������� ��� ������ ����������
	if ((lex.m_lexType == DELIMITER && lex.m_numData != ';') || (lex.m_lexType == KEYWORD && lex.m_numData == KEY_AND) || (lex.m_lexType == KEYWORD && lex.m_numData == KEY_OR))
	{
		if (lex.m_numData >= 0 && lex.m_numData <= 255)
		{
			int nCurPriority = gs_operPriority[lex.m_numData]; int nOper = 0;

			if (nPriority < nCurPriority)//���������� ���������� ����� (���������� ��������) � ������� ����������� ��������
			{
				lex = GetLexem();

				if (lex.m_numData == '*')
				{
					SetOper(OPER_MULT);
				}
				else if (lex.m_numData == '/')
				{
					SetOper(OPER_DIV);
				}
				else if (lex.m_numData == '+')
				{
					SetOper(OPER_ADD);
				}
				else if (lex.m_numData == '-')
				{
					SetOper(OPER_SUB);
				}
				else if (lex.m_numData == '%')
				{
					SetOper(OPER_MOD);
				}
				else if (lex.m_numData == KEY_AND)
				{
					SetOper(OPER_AND);
				}
				else if (lex.m_numData == KEY_OR)
				{
					SetOper(OPER_OR);
				}
				else if (lex.m_numData == '>')
				{
					SetOper(OPER_GT);

					if (IsNextDelimeter('='))
					{
						GETDelimeter('=');
						SetOper(OPER_GE);
					}
				}
				else if (lex.m_numData == '<')
				{
					SetOper(OPER_LS);
					if (IsNextDelimeter('='))
					{
						GETDelimeter('=');
						SetOper(OPER_LE);
					}
					else if (IsNextDelimeter('>'))
					{
						GETDelimeter('>');
						SetOper(OPER_NE);
					}

				}
				else if (lex.m_numData == '=')
				{
					SetOper(OPER_EQ);
				}
				else return sVariable;

				CParamValue sVariable1 = GetVariable();
				CParamValue sVariable2 = sVariable;
				CParamValue sVariable3 = GetExpression(nCurPriority);

				//���. �������� �� ����������� ��������
				if (sVariable2.m_paramType == wxT("STRING")) {
					if (OPER_DIV == nOper ||
						OPER_MOD == nOper ||
						OPER_MULT == nOper ||
						OPER_AND == nOper ||
						OPER_OR == nOper)
						return sVariable;
				}

				sVariable1.m_paramType = sVariable2.m_paramType;

				if (nOper >= OPER_GT && nOper <= OPER_NE) {
					sVariable1.m_paramType = wxT("NUMBER");
				}

				sVariable = sVariable1;
				goto MOperation;
			}
		}
	}
	return sVariable;
}

/*
 * GetCurrentIdentifier
 * ����������:
 * �������������� �������������� (����������� ��� ���� ��� ����������,�������� ��� �������,������)
 * nIsSet - �� �����:  1 - ������� ���� ��� �������� ��������� ������������ ��������� (���� ���������� ���� '=')
 * ������������ ��������:
 * nIsSet - �� ������: 1 - ������� ���� ��� ����� ��������� ������������ ��������� (�.�. ������ ����������� ���� '=')
 * ����� ������ ����������, ��� ����� �������� ��������������
*/
CParamValue CPrecompileModule::GetCurrentIdentifier(int& nIsSet)
{
	int nPrevSet = nIsSet;

	CParamValue sVariable = GetVariable();

	wxString strRealName = GETIdentifier(true);
	wxString strName = stringUtils::MakeUpper(strRealName);

	int nStartPos = m_listLexem[m_numCurrentCompile].m_numString;

	if (!m_bCalcValue && (nStartPos + strRealName.length() == m_nCurrentPos ||
		nStartPos + strRealName.length() == m_nCurrentPos - 1)) {
		unsigned int endContext = 0;
		for (unsigned int i = m_numCurrentCompile; i < m_listLexem.size(); i++) {
			if (m_listLexem[i].m_lexType == KEYWORD && (m_listLexem[i].m_numData == KEY_ENDPROCEDURE || m_listLexem[i].m_numData == KEY_ENDFUNCTION))
				endContext = i;
			if (m_listLexem[i].m_lexType == ENDPROGRAM)
				endContext = i;
		}
		nIsSet = 0; m_numCurrentCompile = endContext; return sVariable;
	}

	sLastExpression = strRealName;

	if (IsNextDelimeter('('))// this is a function call
	{
		CValue valContext;
		if (cContext.FindFunction(strRealName, valContext, true))
		{
			std::vector <CParamValue> aParamList;
			GETDelimeter('(');
			while (m_numCurrentCompile + 1 < m_listLexem.size()
				&& !IsNextDelimeter(')'))
			{
				if (IsNextDelimeter(','))
				{
					CParamValue data;
					aParamList.push_back(data);
				}
				else
				{
					aParamList.emplace_back(GetExpression());
					if (IsNextDelimeter(')')) break;
				}
				GETDelimeter(',');
			}

			GETDelimeter(')');

			const long lMethodNum = valContext.FindMethod(strName);
			if (lMethodNum != wxNOT_FOUND && valContext.HasRetVal(lMethodNum)) {
				CValue** pRefLocVars = new CValue * [aParamList.size() ? aParamList.size() : 1];
				for (unsigned int i = 0; i < aParamList.size(); i++) {
					pRefLocVars[i] = &aParamList[i].m_paramObject;
				}
				if (aParamList.size() == 0) {
					CValue cValue = eValueTypes::TYPE_EMPTY;
					pRefLocVars[0] = &cValue;
				}
				try {
					valContext.CallAsFunc(lMethodNum, sVariable.m_paramObject, pRefLocVars, aParamList.size());
				}
				catch (...) {
				}
				SetVariable(sVariable.m_paramName, sVariable.m_paramObject);
				delete[]pRefLocVars;
			}
		}
		else
		{
			sVariable = GetCallFunction(strName);
		}

		if (IsTypeVar(strName)) { // this is a type cast
			sVariable.m_paramObject = GetTypeVar(strName);
		}

		nIsSet = 0;
	}
	else//��� ����� ����������
	{
		sLastParentKeyword = strRealName;

		bool bCheckError = !nPrevSet;

		if (IsNextDelimeter('.'))//��� ���������� �������� ����� ������
			bCheckError = true;

		CValue valContext;
		if (cContext.FindVariable(strRealName, valContext, true)) {
			nIsSet = 0;
			if (IsNextDelimeter('=') && nPrevSet == 1) {
				GETDelimeter('=');
				CParamValue sParam = GetExpression();
				sVariable.m_paramObject = sParam.m_paramObject;
				return sVariable;
			}
			else {
				const long lPropNum = valContext.FindProp(strName);
				if (lPropNum != wxNOT_FOUND) {
					try {
						valContext.GetPropVal(lPropNum, sVariable.m_paramObject);
					}
					catch (...) {
					}
					SetVariable(sVariable.m_paramName, sVariable.m_paramObject);
				}
			}
		}
		else {
			nIsSet = 1;
			sVariable = GetVariable(strRealName, bCheckError);
		}
	}

MLabel:

	if (IsNextDelimeter('['))// this is an array
	{
		GETDelimeter('[');
		CParamValue sKey = GetExpression();
		GETDelimeter(']');

		//���������� ��� ������ (�.�. ��� ��������� �������� ������� ��� ���������)
		//������:
		//���[10]=12; - Set
		//�=���[10]; - Get
		//���[10][2]=12; - Get,Set

		nIsSet = 0;

		if (IsNextDelimeter('[')) {}//�������� ���� ���������� ������� (��������� ����������� ��������)

		if (IsNextDelimeter('=') && nPrevSet == 1)
		{
			GETDelimeter('=');

			CParamValue sData = GetExpression();
			return sVariable;
		}
		else sVariable = GetVariable();

		goto MLabel;
	}

	if (IsNextDelimeter('.'))// this is a method call ��� �������� ����������� �������
	{
		wxString sTempExpression = sLastExpression;

		GETDelimeter('.');

		wxString strRealMethod = GETIdentifier(true);
		wxString sMethod = stringUtils::MakeUpper(strRealMethod);

		if (m_listLexem[m_numCurrentCompile].m_numString > m_nCurrentPos
			|| m_listLexem[m_numCurrentCompile].m_lexType == KEYWORD) {
			strRealMethod = sMethod = wxEmptyString;
		}

		sLastExpression += strRealMethod;

		if (m_listLexem[m_numCurrentCompile].m_numString > (m_nCurrentPos - strRealMethod.length() - 1))
		{
			sLastExpression = sTempExpression; nLastPosition = m_numCurrentCompile; sLastKeyword = strRealMethod;
			m_valObject = sVariable.m_paramObject; m_numCurrentCompile = m_listLexem.size() - 1; nIsSet = 0;
			return sVariable;
		}
		else if (m_listLexem[m_numCurrentCompile].m_lexType == ENDPROGRAM)
		{
			sLastExpression = sTempExpression; nLastPosition = m_numCurrentCompile; sLastKeyword = strRealMethod;
			m_valObject = sVariable.m_paramObject; m_numCurrentCompile = m_listLexem.size() - 1; nIsSet = 0;
			return sVariable;
		}

		if (IsNextDelimeter('('))// this is a method call
		{
			std::vector <CParamValue> aParamList;
			GETDelimeter('(');
			while (m_numCurrentCompile + 1 < m_listLexem.size()
				&& !IsNextDelimeter(')'))
			{
				if (IsNextDelimeter(','))
				{
					CParamValue data;
					//data.nArray = DEF_VAR_SKIP;// missing parameter
					//data.nIndex = DEF_VAR_SKIP;
					aParamList.push_back(data);
				}
				else
				{
					aParamList.emplace_back(GetExpression());
					if (IsNextDelimeter(')')) break;
				}
				GETDelimeter(',');
			}

			GETDelimeter(')');

			CValue parentValue = sVariable.m_paramObject;
			sVariable = GetVariable();

			const long lMethodNum = parentValue.FindMethod(sMethod);
			if (lMethodNum != wxNOT_FOUND && parentValue.HasRetVal(lMethodNum)) {
				CValue** paParams = new CValue * [aParamList.size() ? aParamList.size() : 1];
				if (aParamList.size() == 0) {
					CValue cValue = eValueTypes::TYPE_EMPTY;
					paParams[0] = &cValue;
				}
				for (unsigned int i = 0; i < aParamList.size(); i++) {
					paParams[i] = &aParamList[i].m_paramObject;
				}

				try {
					parentValue.CallAsFunc(lMethodNum, sVariable.m_paramObject, paParams, aParamList.size());
				}
				catch (...) {
				}
				SetVariable(sVariable.m_paramName, sVariable.m_paramObject);
				delete[]paParams;
			}

			nIsSet = 0;
		}
		else//����� - ����� ��������
		{
			//���������� ��� ������ (�.�. ��� ��������� �������� ��� ���������)
			//������:
			//�=���.�����; - Get
			//���.�����=0; - Set
			//���.�����.���=0;  - Get,Set

			nIsSet = 0;

			if (IsNextDelimeter('=') && nPrevSet == 1) {
				GETDelimeter('=');
				CValue parentValue = sVariable.m_paramObject;
				CParamValue sParam = GetExpression();
				const long lPropNum = parentValue.FindProp(strRealMethod);
				if (lPropNum != wxNOT_FOUND) {
					try {
						parentValue.SetPropVal(lPropNum, sParam.m_paramObject);
					}
					catch (...) {
					}
				}
				return sVariable;
			}
			else {
				CValue parentValue = sVariable.m_paramObject;
				sVariable = GetVariable();
				const long lPropNum = parentValue.FindProp(sMethod);
				if (lPropNum != wxNOT_FOUND) {
					try {
						parentValue.GetPropVal(lPropNum, sVariable.m_paramObject);
					}
					catch (...)
					{
					}
				}
				SetVariable(sVariable.m_paramName, sVariable.m_paramObject);
			}
		}
		goto MLabel;
	}

	return sVariable;
}//GetCurrentIdentifier

/**
 * ��������� ������ ������� ��� ���������
 */
CParamValue CPrecompileModule::GetCallFunction(const wxString& strName)
{
	std::vector<CParamValue> aParamList;

	GETDelimeter('(');

	while (m_numCurrentCompile + 1 < m_listLexem.size()
		&& !IsNextDelimeter(')'))
	{
		if (IsNextDelimeter(','))
		{
			CParamValue data;
			//data.nArray = DEF_VAR_SKIP;// missing parameter
			//data.nIndex = DEF_VAR_SKIP;
			aParamList.push_back(data);
		}
		else
		{
			aParamList.emplace_back(GetExpression());

			if (IsNextDelimeter(')')) break;
		}
		GETDelimeter(',');
	}
	GETDelimeter(')');

	CValue retValue;

	if (cContext.cFunctions.find(strName) != cContext.cFunctions.end())
	{
		CPrecompileFunction* pDefFunction = cContext.cFunctions[strName];
		pDefFunction->aParamList = aParamList;
		retValue = pDefFunction->RealRetValue.m_paramObject;
	}

	CParamValue sVariable = GetVariable();
	sVariable.m_paramObject = retValue;
	return sVariable;
}

/**
 * AddVariable
 * ����������:
 * �������� ��� � ����� ������� ��������� � ����������� ������ ��� ����������� �������������
 */
void CPrecompileModule::AddVariable(const wxString& strVarName, const CValue& varVal)
{
	if (strVarName.IsEmpty())
		return;

	// take into account external variables during compilation
	cContext.GetVariable(strVarName, false, false, varVal);
}

/**
 * ������� ���������� ����� ���������� �� ���������� �����
 */
CParamValue CPrecompileModule::GetVariable(const wxString& strName, bool bCheckError)
{
	return m_pContext->GetVariable(strName, true, bCheckError);
}

/**
 * C������ ����� ������������� ����������
 */
CParamValue CPrecompileModule::GetVariable()
{
	const wxString& strVarName = wxString::Format("@%d", m_pContext->nTempVar); //@ - ��� �������� ������������ �����
	CParamValue sVariable = m_pContext->GetVariable(strVarName, false);//��������� ���������� ���� ������ � ��������� ���������
	m_pContext->nTempVar++;
	return sVariable;
}

void CPrecompileModule::SetVariable(const wxString& strVarName, const CValue& varVal)
{
	m_pContext->SetVariable(strVarName, varVal);
}

/**
 * �������� ����� ��������� �� ����������� ������ ��������
 * (���� ������ �������� � ������ ���, �� ��� ���������)
 */
CParamValue CPrecompileModule::FindConst(CValue& vData)
{
	CParamValue Const;
	wxString strType = vData.GetClassName();
	Const.m_paramType = GetTypeVar(strType);
	Const.m_paramObject = vData;
	return Const;
}


#pragma warning(pop)