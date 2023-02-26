#ifndef _VALUEFONT_H__
#define _VALUEFONT_H__

#include "core/compiler/value.h"

//��������� ��������
class CValueFont : public CValue
{
	wxDECLARE_DYNAMIC_CLASS(CValueFont);
private:
	wxFont m_font;
public:

	CValueFont();
	CValueFont(const wxFont& font);
	virtual ~CValueFont();

	virtual bool Init(CValue** paParams, const long lSizeArray);

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

	//check is empty
	virtual inline bool IsEmpty() const override {
		return !m_font.IsOk();
	}

	static CMethodHelper m_methodHelper;

	virtual bool SetPropVal(const long lPropNum, const CValue& varPropVal);        //��������� ��������
	virtual bool GetPropVal(const long lPropNum, CValue& pvarPropVal);                   //�������� ��������

	virtual CMethodHelper* GetPMethods() const {
		PrepareNames();
		return &m_methodHelper;
	}
	virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������

	operator wxFont() {
		return m_font;
	}
};

#endif