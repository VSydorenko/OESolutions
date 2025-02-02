#ifndef _VALUEPOINT_H__
#define _VALUEPOINT_H__

#include "backend/compiler/value/value.h"

//��������� ��������
class BACKEND_API CValuePoint : public CValue
{
	wxDECLARE_DYNAMIC_CLASS(CValuePoint);

public:

	wxPoint m_point;

public:

	CValuePoint();
	CValuePoint(const wxPoint& point);
	virtual ~CValuePoint() {}

	virtual bool Init(CValue** paParams, const long lSizeArray);
	virtual wxString GetString() const {
		return typeConv::PointToString(m_point); 
	}

	//check is empty
	virtual inline bool IsEmpty() const {
		return m_point == wxDefaultPosition;
	}

	static CMethodHelper m_methodHelper;

	virtual bool SetPropVal(const long lPropNum, const CValue& varPropVal);        //��������� ��������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);                   //�������� ��������

	virtual CMethodHelper* GetPMethods() const { //�������� ������ �� ����� �������� ������� ���� ��������� � �������
		//PrepareNames(); 
		return &m_methodHelper;
	}
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������

	operator wxPoint() const { return m_point; }
};

#endif