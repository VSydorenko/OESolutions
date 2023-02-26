#ifndef _VALUECOLOUR_H__
#define _VALUECOLOUR_H__

#include "core/compiler/value.h"

//��������� ��������
class CValueColour : public CValue
{
	wxDECLARE_DYNAMIC_CLASS(CValueColour);

public:

	wxColour m_colour;

public:

	CValueColour();
	CValueColour(const wxColour& colour);
	virtual ~CValueColour();

	virtual bool Init(CValue** paParams, const long lSizeArray);

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

	//check is empty
	virtual inline bool IsEmpty() const override {
		return !m_colour.IsOk();
	}

	static CMethodHelper m_methodHelper;

	virtual bool SetPropVal(const long lPropNum, const CValue& varPropVal);        //��������� ��������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);                   //�������� ��������

	virtual CMethodHelper* GetPMethods() const {
		PrepareNames();
		return &m_methodHelper;
	}
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������

	operator wxColour() const {
		return m_colour;
	}
};

#endif