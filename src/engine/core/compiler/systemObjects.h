#ifndef _SYSTEMOBJECTS_H__
#define _SYSTEMOBJECTS_H__

#include "value.h"

//--���������:
#define PageBreak wxT("\n\n")
#define LineBreak wxT("\n")
#define TabSymbol wxT("\t")

#include "systemEnums.h"

class CSystemObjects : public CValue {
	static wxDateTime m_workDate;
public:

	//--- �������:
	static bool Boolean(const CValue &cValue);
	static number_t Number(const CValue &cValue);
	static wxLongLong_t Date(const CValue &cValue);
	static wxString String(const CValue &cValue);

	//--- ��������������:
	static number_t Round(const CValue &cValue, int precision = 0, eRoundMode mode = eRoundMode::eRoundMode_Round15as20);
	static CValue Int(const CValue &cNumber);
	static number_t Log10(const CValue &cValue);
	static number_t Ln(const CValue &cValue);
	static CValue Max(CValue **p);
	static CValue Min(CValue **p);
	static CValue Sqrt(const CValue &cValue);

	//--- ���������:
	static int StrLen(const CValue &cValue);
	static bool IsBlankString(const CValue &cValue);
	static wxString TrimL(const CValue &cValue);
	static wxString TrimR(const CValue &cValue);
	static wxString TrimAll(const CValue &cValue);
	static wxString Left(const CValue &cValue, unsigned int nCount);
	static wxString Right(const CValue &cValue, unsigned int nCount);
	static wxString Mid(const CValue &cValue, unsigned int nFirst, unsigned int nCount);
	static unsigned int Find(const CValue &cValue, const CValue &cValue2, unsigned int nStart);
	static wxString StrReplace(const CValue &cSource, const CValue &cValue1, const CValue &cValue2);
	static int StrCountOccur(const CValue &cSource, const CValue &cValue1);
	static int StrLineCount(const CValue &cSource);
	static wxString StrGetLine(const CValue &cValue, unsigned int nLine);
	static wxString Upper(const CValue &cSource);
	static wxString Lower(const CValue &cSource);
	static wxString Chr(short nCode);
	static short Asc(const CValue &cSource);

	//--- ������ � ����� � ��������:
	static CValue CurrentDate();
	static CValue WorkingDate();
	static CValue AddMonth(const CValue &cData, int nMonthAdd = 1);
	static CValue BegOfMonth(const CValue &cData);
	static CValue EndOfMonth(const CValue &cData);
	static CValue BegOfQuart(const CValue &cData);
	static CValue EndOfQuart(const CValue &cData);
	static CValue BegOfYear(const CValue &cData);
	static CValue EndOfYear(const CValue &cData);
	static CValue BegOfWeek(const CValue &cData);
	static CValue EndOfWeek(const CValue &cData);
	static CValue BegOfDay(const CValue &cData);
	static CValue EndOfDay(const CValue &cData);
	static int GetYear(const CValue &cData);
	static int GetMonth(const CValue &cData);
	static int GetDay(const CValue &cData);
	static int GetHour(const CValue &cData);
	static int GetMinute(const CValue &cData);
	static int GetSecond(const CValue &cData);
	static int GetWeekOfYear(const CValue &cData);
	static int GetDayOfYear(const CValue &cData);
	static int GetDayOfWeek(const CValue &cData);
	static int GetQuartOfYear(const CValue &cData);

	//--- ������ � �������: 
	static bool CopyFile(const wxString&src, const wxString&dst);
	static bool DeleteFile(const wxString&file);
	static wxString GetTempDir();
	static wxString GetTempFileName();

	//--- ������ � ������: 
	static class CValueForm* ActiveWindow();

	//--- �����������:
	static void Message(const wxString &sMessage, eStatusMessage status = eStatusMessage::eStatusMessage_Information);
	static void Alert(const wxString &sMessage);
	static CValue Question(const wxString &sMessage, eQuestionMode mode = eQuestionMode::eQuestionMode_OK);
	static void SetStatus(const wxString &sStatus);
	static void ClearMessages();
	static void SetError(const wxString &sError);
	static void Raise(const wxString &sError);
	static wxString ErrorDescription();
	static bool IsEmptyValue(const CValue &cData);
	static CValue Evaluate(const wxString &sExpression);
	static void Execute(const wxString &sCode);
	static wxString Format(CValue &cData, const wxString &fmt = wxEmptyString);
	static CValue Type(const CValue &cTypeName);
	static CValue TypeOf(const CValue &cData);
	static int Rand();
	static int ArgCount();
	static wxString ArgValue(int n);
	static wxString ComputerName();
	static void RunApp(const wxString &sCommand);
	static void SetAppTitle(const wxString &sTitle);
	static wxString UserDir();
	static wxString UserName();
	static wxString UserPassword();
	static bool SingleMode();
	static int GeneralLanguage();
	static void EndJob(bool force = false);

	static void UserInterruptProcessing();

	static class CValueForm *GetCommonForm(const wxString &sFormName, class IValueFrame *owner, class CValueGuid *unique);
	static void ShowCommonForm(const wxString &sFormName, class IValueFrame *owner, class CValueGuid *unique);

	static void BeginTransaction();
	static void CommitTransaction();
	static void RollBackTransaction();

public:

	CSystemObjects();

	//****************************************************************************
	//*                              Support methods                             *
	//****************************************************************************

	static CMethods m_methods;

	virtual CMethods* GetPMethods() const { PrepareNames(); return &m_methods; } 	// �������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                             // ���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t &aParams);       // ����� ������

	//check is empty
	virtual inline bool IsEmpty() const override { return false; }
};

#endif 