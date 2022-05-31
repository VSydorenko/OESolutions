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

	static CMethods m_methods;

	virtual CMethods* GetPMethods() const { return &m_methods; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t &aParams);       //����� ������
};

#endif