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
	virtual CMethods* GetPMethods() const { //�������� ������ �� ����� �������� ������� ���� ��������� � �������
		return m_methods; 
	}

	virtual void PrepareNames() const; //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������

	//���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue GetAttribute(attributeArg_t &aParams);                   //�������� ��������

	virtual wxString GetString() const { 
		return wxT("enumFactory"); 
	}
	
	virtual wxString GetTypeString() const {
		return wxT("enumFactory"); 
	}

protected:
	CMethods *m_methods;
};

#endif 