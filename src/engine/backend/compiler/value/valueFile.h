#ifndef _VALUE_FILE_H__
#define _VALUE_FILE_H__

#include "backend/compiler/value/value.h"

class BACKEND_API CValueFile : public CValue {
	wxDECLARE_DYNAMIC_CLASS(CValueFile);
private:

	enum Prop {
		enBaseName,
		enExtension,
		enFullName,
		enName,
		enPath,
	};

	enum Func {
		enExist,
		//enGetHidden,
		enGetModificationTime,
		enGetReadOnly,
		enIsDirectory,
		enIsFile,
		//enSetHidden,
		//enSetModificationTime,
		//enSetReadOnly,
		enSize,
	};

public:

	//��� ������ ����� �������������� � ����� ���������� ��������:
	virtual CMethodHelper* GetPMethods() const { 
		//PrepareNames();
		return &m_methodHelper; 
	}

	virtual void PrepareNames() const;//���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//����� ������

	virtual bool SetPropVal(const long lPropNum, const CValue &varValue);//��������� ��������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);//�������� ��������

	CValueFile();
	virtual ~CValueFile();

	virtual inline bool IsEmpty() const { return false; }

	virtual bool Init() { return false; }
	virtual bool Init(CValue **paParams, const long lSizeArray);

private:
	wxString m_fileName; 
	static CMethodHelper m_methodHelper;
};

#endif 