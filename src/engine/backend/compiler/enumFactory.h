#ifndef _ENUM_FACTORY_H__
#define _ENUM_FACTORY_H__

#include "value/value.h"

//realization factory pattern 
class CEnumFactory : public CValue {
public:

	CEnumFactory();
	virtual ~CEnumFactory();

	//��� ������ ����� �������������� � ����� ���������� ��������:
	virtual CMethodHelper* GetPMethods() const { //�������� ������ �� ����� �������� ������� ���� ��������� � �������	
		//PrepareNames(); 
		return m_methodHelper; 
	}

	virtual void PrepareNames() const; //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������

	//���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);                   //�������� ��������

protected:
	CMethodHelper *m_methodHelper;
};

#endif 