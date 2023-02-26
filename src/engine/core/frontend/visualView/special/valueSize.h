#ifndef _VALUESIZE_H__
#define _VALUESIZE_H__

#include "core/compiler/value.h"

//��������� ��������
class CValueSize : public CValue
{
	wxDECLARE_DYNAMIC_CLASS(CValueSize);

public:

	wxSize m_size;

public:

	CValueSize();
	CValueSize(const wxSize& size);

	virtual bool Init(CValue** paParams, const long lSizeArray);

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

	virtual inline bool IsEmpty() const override {
		return m_size == wxDefaultSize;
	}

	static CMethodHelper m_methodHelper;

	virtual bool SetPropVal(const long lPropNum, const CValue& varPropVal);        //��������� ��������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);                   //�������� ��������

	virtual CMethodHelper* GetPMethods() const { //�������� ������ �� ����� �������� ������� ���� ��������� � �������
		PrepareNames();
		return &m_methodHelper;
	}

	virtual void PrepareNames() const; //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������

	operator wxSize() const {
		return m_size;
	}

	virtual ~CValueSize();

};

#endif