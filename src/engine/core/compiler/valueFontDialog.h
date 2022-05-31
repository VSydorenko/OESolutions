#ifndef  _VALUE_FONT_DIALOG_H__
#define _VALUE_FONT_DIALOG_H__

#include "compiler/value.h"

#include <wx/fontdlg.h>

class CValueFontDialog : public CValue {
	wxDECLARE_DYNAMIC_CLASS(CValueFileDialog);
public:

	//��� ������ ����� �������������� � ����� ���������� ��������:
	virtual CMethods* GetPMethods() const { return &m_methods; }//�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;//���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t &aParams);//����� ������

	virtual void SetAttribute(attributeArg_t &aParams, CValue &cVal);//��������� ��������
	virtual CValue GetAttribute(attributeArg_t &aParams);//�������� ��������

	CValueFontDialog();
	virtual ~CValueFontDialog();

	virtual inline bool IsEmpty() const override { return false; }

	virtual wxString GetTypeString() const { return wxT("fontDialog"); }
	virtual wxString GetString() const { return wxT("fontDialog"); }

private:
	static CMethods m_methods;

	wxFontDialog *m_fontDialog;
};


#endif // ! _VALUE_FONT_DIALOG_H__
