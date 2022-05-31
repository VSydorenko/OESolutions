#ifndef  _VALUE_COLOUR_DIALOG_H__
#define _VALUE_COLOUR_DIALOG_H__

#include "compiler/value.h"

#include <wx/colordlg.h>

class CValueColourDialog : public CValue {
	wxDECLARE_DYNAMIC_CLASS(CValueFileDialog);
public:

	//��� ������ ����� �������������� � ����� ���������� ��������:
	virtual CMethods* GetPMethods() const { return &m_methods; }//�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;//���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t &aParams);//����� ������

	virtual void SetAttribute(attributeArg_t &aParams, CValue &cVal);//��������� ��������
	virtual CValue GetAttribute(attributeArg_t &aParams);//�������� ��������

	CValueColourDialog();
	virtual ~CValueColourDialog();

	virtual inline bool IsEmpty() const override { return false; }

	virtual wxString GetTypeString() const { return wxT("colourDialog"); }
	virtual wxString GetString() const { return wxT("colourDialog"); }

private:
	static CMethods m_methods;

	wxColourDialog *m_colourDialog;
};


#endif // ! _VALUE_FONT_DIALOG_H__
