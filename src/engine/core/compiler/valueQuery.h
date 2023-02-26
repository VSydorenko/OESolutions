#ifndef _VALUEQUERY_H__
#define _VALUEQUERY_H__

#include "value.h"

//��������� ��������
class CValueQuery : public CValue
{
	wxDECLARE_DYNAMIC_CLASS(CValueQuery);

private:

	wxString m_sQueryText;

private:

	void Execute();

public:

	CValueQuery();
	virtual ~CValueQuery();

	virtual wxString GetTypeString() const { return wxT("query"); }
	virtual wxString GetString() const { return wxT("query"); }

	static CMethodHelper m_methodHelper;

	virtual CMethodHelper* GetPMethods() const { return &m_methodHelper; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);       //����� ������
};

#endif