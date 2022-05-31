#ifndef _VALUE_FILEDIALOG_H_
#define _VALUE_FILEDIALOG_H_

#include "compiler/value.h"
#include "compiler/enumObject.h"

enum eFileDialogMode {
	eChooseDirectory = 1,
	eOpen,
	eSave
};

class CValueEnumFileDialogMode : public IEnumeration<eFileDialogMode>{
	wxDECLARE_DYNAMIC_CLASS(CValueEnumFileDialogMode);
public:

	CValueEnumFileDialogMode() : IEnumeration() { InitializeEnumeration(); }
	CValueEnumFileDialogMode(eFileDialogMode mode) : IEnumeration(mode) { InitializeEnumeration(mode); }

	wxString GetTypeString() const override { return wxT("fileDialogMode"); }

protected:

	void CreateEnumeration()
	{
		AddEnumeration(eChooseDirectory, _("chooseDirectory"));
		AddEnumeration(eOpen, _("open"));
		AddEnumeration(eSave, _("save"));
	}

	virtual void InitializeEnumeration() override
	{
		CreateEnumeration();
		IEnumeration::InitializeEnumeration();
	}

	virtual void InitializeEnumeration(eFileDialogMode value) override
	{
		CreateEnumeration();
		IEnumeration::InitializeEnumeration(value);
	}
};

#include <wx/filedlg.h>

class CValueFileDialog : public CValue {
	wxDECLARE_DYNAMIC_CLASS(CValueFileDialog);
public:

	//��� ������ ����� �������������� � ����� ���������� ��������:
	virtual CMethods* GetPMethods() const { return &m_methods; }//�������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;//���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t &aParams);//����� ������

	virtual void SetAttribute(attributeArg_t &aParams, CValue &cVal);//��������� ��������
	virtual CValue GetAttribute(attributeArg_t &aParams);//�������� ��������

	CValueFileDialog();
	virtual ~CValueFileDialog();

	virtual inline bool IsEmpty() const override { return false; }

	virtual bool Init() { return false; }
	virtual bool Init(CValue **aParams);

	virtual wxString GetTypeString() const { return wxT("fileDialog"); }
	virtual wxString GetString() const { return wxT("fileDialog"); }

private:
	static CMethods m_methods;
	eFileDialogMode m_dialogMode;

	wxDirDialog *m_dirDialog;
	wxFileDialog *m_fileDialog;
};

#endif 