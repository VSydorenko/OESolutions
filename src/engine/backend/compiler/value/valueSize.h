#ifndef _VALUESIZE_H__
#define _VALUESIZE_H__

#include "backend/compiler/value/value.h"

//��������� ��������
class BACKEND_API CValueSize : public CValue
{
	wxDECLARE_DYNAMIC_CLASS(CValueSize);

public:

	wxSize m_size;

public:

	CValueSize();
	CValueSize(const wxSize& size);
	virtual ~CValueSize() {}

	virtual bool Init(CValue** paParams, const long lSizeArray);
	virtual wxString GetString() const {
		return typeConv::SizeToString(m_size);
	}

	virtual inline bool IsEmpty() const {
		return m_size == wxDefaultSize;
	}

	static CMethodHelper m_methodHelper;

	virtual bool SetPropVal(const long lPropNum, const CValue& varPropVal);        //��������� ��������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);                   //�������� ��������

	virtual CMethodHelper* GetPMethods() const { //�������� ������ �� ����� �������� ������� ���� ��������� � �������
		//PrepareNames(); 
		return &m_methodHelper;
	}
	virtual void PrepareNames() const; //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������

	operator wxSize() const {
		return m_size;
	}


};

#endif