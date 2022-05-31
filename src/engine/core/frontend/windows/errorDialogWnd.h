#ifndef _ERROR_DLG_H__
#define _ERROR_DLG_H__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/stc/stc.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/checkbox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

#include "frontend/settings/editorsettings.h"
#include "frontend/settings/fontcolorsettings.h"

class CErrorDialog : public wxDialog
{
	wxStyledTextCtrl* m_errorWnd;

	wxButton* m_buttonCloseProgram;
	wxButton* m_buttonGotoDesigner;
	wxButton* m_buttonCloseWindow;

public:

	//Editor setting 
	void SetEditorSettings(const EditorSettings& settings);
	//Font setting 
	void SetFontColorSettings(const FontColorSettings& settings);

	CErrorDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Critical failure"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION | wxSTAY_ON_TOP);
	virtual ~CErrorDialog();

	void SetErrorMessage(const wxString &sError) { 
		m_errorWnd->SetReadOnly(false);
		m_errorWnd->SetText(sError);
		m_errorWnd->SetReadOnly(true); 
	}

protected:

	void OnButtonCloseProgramClick(wxCommandEvent& event);
	void OnButtonGotoDesignerClick(wxCommandEvent& event);
	void OnButtonCloseWindowClick(wxCommandEvent& event);
};

#endif 
