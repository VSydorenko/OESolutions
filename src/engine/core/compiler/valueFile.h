#ifndef _VALUE_FILE_H__
#define _VALUE_FILE_H__

#include "compiler/value.h"

class CValueFile : public CValue {
	wxDECLARE_DYNAMIC_CLASS(CValueFile);
public:

	//��� ������ ����� �������������� � ����� ���������� ��������:
	virtual CMethods* GetPMethods() const { return &m_methods; }//�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;//���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t &aParams);//����� ������

	virtual void SetAttribute(attributeArg_t &aParams, CValue &cVal);//��������� ��������
	virtual CValue GetAttribute(attributeArg_t &aParams);//�������� ��������

	CValueFile();
	virtual ~CValueFile();

	virtual inline bool IsEmpty() const override { return false; }

	virtual bool Init() { return false; }
	virtual bool Init(CValue **aParams);

	virtual wxString GetTypeString() const { return wxT("file"); }
	virtual wxString GetString() const { return wxT("file"); }

private:
	wxString m_fileName; 
	static CMethods m_methods;
};

#endif 