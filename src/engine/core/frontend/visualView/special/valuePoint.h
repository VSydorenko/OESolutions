#ifndef _VALUEPOINT_H__
#define _VALUEPOINT_H__

#include "compiler/value.h"

//��������� ��������
class CValuePoint : public CValue
{
	wxDECLARE_DYNAMIC_CLASS(CValuePoint);

public:

	wxPoint m_point;

public:

	CValuePoint();
	CValuePoint(const wxPoint &point);

	virtual bool Init(CValue **aParams);

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

	//check is empty
	virtual inline bool IsEmpty() const override { return m_point == wxDefaultPosition; }

	static CMethods m_methods;

	virtual void SetAttribute(attributeArg_t &aParams, CValue &cVal);        //��������� ��������
	virtual CValue GetAttribute(attributeArg_t &aParams);                   //�������� ��������

	virtual CMethods* GetPMethods() const { return &m_methods; } //�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������

	operator wxPoint() { return m_point; }
	virtual ~CValuePoint();
};

#endif