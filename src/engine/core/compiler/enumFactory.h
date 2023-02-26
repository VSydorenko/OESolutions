#ifndef _ENUMFACTORY_H__
#define _ENUMFACTORY_H__

#include "value.h"

#define enumFactory         	(CEnumFactory::Get())
#define enumFactoryDestroy()  	(CEnumFactory::Destroy())

//realization factory pattern 
class CEnumFactory : public CValue {
	static CEnumFactory *s_instance;
private:
	CEnumFactory();
public:

	virtual ~CEnumFactory();

	static CEnumFactory* Get();
	static void Destroy();

	//��� ������ ����� �������������� � ����� ���������� ��������:
	virtual CMethodHelper* GetPMethods() const { //�������� ������ �� ����� �������� ������� ���� ��������� � �������	
		PrepareNames(); 
		return m_methodHelper; 
	}

	virtual void PrepareNames() const; //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������

	//���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);                   //�������� ��������

	virtual wxString GetString() const { 
		return wxT("enumFactory"); 
	}
	
	virtual wxString GetTypeString() const {
		return wxT("enumFactory"); 
	}

protected:
	CMethodHelper *m_methodHelper;
};

#endif 