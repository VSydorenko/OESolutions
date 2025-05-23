#include "enterpriseOption.h"

#include "frontend/win/theme/luna_tabart.h"
#include "backend/debugger/debugServer.h"

void CDialogEnterpriseOption::FillOption()
{
	m_enableDebugger->Enable(debugServer->AllowDebugging());
	m_enableDebugger->SetValue(debugServer->EnableDebugging()); 
}

////////////////////////////////////////////////////////////////////////////////////////////////

CDialogEnterpriseOption::CDialogEnterpriseOption(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

	m_mainNotebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP |
		wxAUI_NB_TAB_SPLIT |
		wxAUI_NB_TAB_MOVE |
		wxAUI_NB_SCROLL_BUTTONS
	);

	m_mainNotebook->SetArtProvider(new wxAuiLunaTabArt()); 
	m_generalPanel = new wxPanel(m_mainNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_mainNotebook->AddPage(m_generalPanel, _("General"), false, wxNullBitmap);
	m_systemPanel = new wxPanel(m_mainNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	m_mainNotebook->AddPage(m_systemPanel, _("System"), true, wxNullBitmap);
	mainSizer->Add(m_mainNotebook, 1, wxEXPAND | wxALL, 5);
	m_enableDebugger = new wxCheckBox(m_systemPanel, wxID_ANY, _("Debugging in the current session"), wxDefaultPosition, wxDefaultSize, 0);
	m_enableDebugger->Bind(wxEVT_CHECKBOX, &CDialogEnterpriseOption::OnEnableDebugger, this);

	m_mainNotebook->SetSelection(2); 

	this->SetSizer(mainSizer);
	this->Layout();
	this->Centre(wxBOTH);

	FillOption();
}

void CDialogEnterpriseOption::OnEnableDebugger(wxCommandEvent &event)
{
	bool enableDebugger = m_enableDebugger->GetValue();

	if (enableDebugger) {
		debugServer->CreateServer();
	}
	else {
		debugServer->ShutdownServer();
	}
	
	event.Skip();
}
