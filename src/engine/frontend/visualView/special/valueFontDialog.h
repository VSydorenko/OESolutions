#ifndef  _VALUE_FONT_DIALOG_H__
#define _VALUE_FONT_DIALOG_H__

#include "core/compiler/value.h"

#include <wx/fontdlg.h>

class CValueFontDialog : public CValue {
	wxDECLARE_DYNAMIC_CLASS(CValueFileDialog);
private:
	enum Prop {
		enFont,
	};
	enum Func {
		enChoose
	};
public:

	//��� ������ ����� �������������� � ����� ���������� ��������:
	virtual CMethodHelper* GetPMethods() const { 
		PrepareNames();
		return &m_methodHelper; 
	}
	virtual void PrepareNames() const;//���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual bool CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray);//����� ������

	virtual bool SetPropVal(const long lPropNum, CValue &varPropVal);//��������� ��������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);//�������� ��������

	CValueFontDialog();
	virtual ~CValueFontDialog();

	virtual inline bool IsEmpty() const override {
		return false; 
	}

	virtual wxString GetTypeString() const { return wxT("fontDialog"); }
	virtual wxString GetString() const { return wxT("fontDialog"); }

private:
	static CMethodHelper m_methodHelper;

	wxFontDialog *m_fontDialog;
};

#endif // ! _VALUE_FONT_DIALOG_H__
