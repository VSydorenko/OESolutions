#ifndef _QUERYUNIT_H__
#define _QUERYUNIT_H__

#include "core/compiler/value.h"

class CQueryUnit
{
	struct queryLexem_t {

		//��� �������:
		short       m_nType;

		//���������� �������:
		short	    m_nData;		//����� ���������� �����(KEYWORD) ��� ������ �����������(DELIMITER)
		CValue	    m_vData;		//��������, ���� ��� ��������� ��� �������� ��� ��������������
		wxString	m_sData;		//��� ��� �������������� (����������, ������� � ��.)

		unsigned int m_nNumberLine;	  //����� ������ ��������� ������ (��� ����� ��������)
		unsigned int m_nNumberString;	  //����� ��������� ������ (��� ������ ������)

	public:

		//�����������: 
		queryLexem_t() :
			m_nType(0),
			m_nData(0),
			m_nNumberString(0),
			m_nNumberLine(0) {
		}
	};


	wxString queryText;
	std::map<wxString, CValue> paParams;

public:

	CQueryUnit();
	CQueryUnit(const wxString& queryText);

	void SetQueryText(const wxString& queryText);
	wxString GetQueryText();

	void SetQueryParam(const wxString& sParamName, CValue cParam);
	CValue GetQueryParam(const wxString& sParamName);

	void Execute();

protected:

	void Reset();
};

#endif