#ifndef _VALUE_FILE_H__
#define _VALUE_FILE_H__

#include "core/compiler/value.h"

class CORE_API CValueFile : public CValue {
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
		PrepareNames();
		return &m_methodHelper; 
	}

	virtual void PrepareNames() const;//���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//����� ������

	virtual bool SetPropVal(const long lPropNum, const CValue &varValue);//��������� ��������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);//�������� ��������

	CValueFile();
	virtual ~CValueFile();

	virtual inline bool IsEmpty() const override { return false; }

	virtual bool Init() { return false; }
	virtual bool Init(CValue **paParams, const long lSizeArray);

	virtual wxString GetTypeString() const { return wxT("file"); }
	virtual wxString GetString() const { return wxT("file"); }

private:
	wxString m_fileName; 
	static CMethodHelper m_methodHelper;
};

#endif 