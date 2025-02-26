////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : code runner app
////////////////////////////////////////////////////////////////////////////

#include "mainApp.h"

#include "backend/appData.h"

bool CCodeRunnerApp::OnInit()
{
	wxDateTime::SetCountry(wxDateTime::Country::Country_Default);

	m_locale.AddCatalogLookupPathPrefix(_T("lang"));
	m_locale.AddCatalog(m_locale.GetCanonicalName());
	m_codeRunner->Show();

	CApplicationData::CreateAppDataEnv();
	return m_locale.Init(wxLANGUAGE_ENGLISH) && wxApp::OnInit();
}

int CCodeRunnerApp::OnExit()
{
	CApplicationData::DestroyAppDataEnv();
	return wxApp::OnExit();
}

void CCodeRunnerApp::AppendOutput(const wxString& str)
{
	m_codeRunner->AppendOutput(str);
}

wxIMPLEMENT_APP(CCodeRunnerApp);   