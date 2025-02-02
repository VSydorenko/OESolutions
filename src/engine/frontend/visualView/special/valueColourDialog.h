#ifndef  _VALUE_COLOUR_DIALOG_H__
#define _VALUE_COLOUR_DIALOG_H__

#include "core/compiler/value.h"

#include <wx/colordlg.h>

class CValueColourDialog : public CValue {
	wxDECLARE_DYNAMIC_CLASS(CValueFileDialog);
public:

	//��� ������ ����� �������������� � ����� ���������� ��������:
	virtual CMethodHelper* GetPMethods() const { 
		PrepareNames();
		return &m_methodHelper; 
	}
	//�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;//���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//����� ������

	virtual bool SetPropVal(const long lPropNum, CValue &varPropVal);//��������� ��������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);//�������� ��������

	CValueColourDialog();
	virtual ~CValueColourDialog();

	virtual inline bool IsEmpty() const override { 
		return false;
	}

	virtual wxString GetTypeString() const { 
		return wxT("colourDialog");
	}
	
	virtual wxString GetString() const { 
		return wxT("colourDialog"); 
	}

private:
	static CMethodHelper m_methodHelper;

	wxColourDialog *m_colourDialog;
};


#endif // ! _VALUE_FONT_DIALOG_H__
