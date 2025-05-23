////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko, wxwidgets community
//	Description : main frame window
////////////////////////////////////////////////////////////////////////////

#include "mainFrameEnterprise.h"
#include <wx/config.h>

//********************************************************************************
//*                                Hotkey support                                *
//********************************************************************************

void CDocEnterpriseMDIFrame::SetDefaultHotKeys()
{
	// Setup the hotkeys.
	m_keyBinder.SetShortcut(wxID_NEW, wxT("Ctrl+N"));
	m_keyBinder.SetShortcut(wxID_SAVE, wxT("Ctrl+S"));
	m_keyBinder.SetShortcut(wxID_UNDO, wxT("Ctrl+Z"));
	m_keyBinder.SetShortcut(wxID_REDO, wxT("Ctrl+Y"));

	m_keyBinder.SetShortcut(wxID_CUT, wxT("Ctrl+X"));
	m_keyBinder.SetShortcut(wxID_COPY, wxT("Ctrl+C"));
	m_keyBinder.SetShortcut(wxID_PASTE, wxT("Ctrl+V"));
	m_keyBinder.SetShortcut(wxID_SELECTALL, wxT("Ctrl+A"));
	m_keyBinder.SetShortcut(wxID_FIND, wxT("Ctrl+F"));

	m_keyBinder.SetShortcut(wxID_ENTERPRISE_ABOUT, wxT("F1"));
}

//********************************************************************************
//*                                Default menu                                  *
//********************************************************************************

void CDocEnterpriseMDIFrame::InitializeDefaultMenu()
{
	wxMenuBar *menuBar = new wxMenuBar(wxMB_DOCKABLE);
	
	// and its menu bar
	m_menuFile = new wxMenu();

	m_menuFile->Append(wxID_NEW);
	m_menuFile->Append(wxID_OPEN);

	m_menuFile->Append(wxID_CLOSE);
	m_menuFile->Append(wxID_SAVE);
	m_menuFile->Append(wxID_SAVEAS);
	m_menuFile->Append(wxID_REVERT, _("Re&vert..."));

	m_menuFile->AppendSeparator();
	m_menuFile->Append(wxID_PRINT);
	m_menuFile->Append(wxID_PRINT_SETUP, "Print &Setup...");
	m_menuFile->Append(wxID_PREVIEW);

	m_menuFile->AppendSeparator();
	m_menuFile->Append(wxID_EXIT);

	menuBar->Append(m_menuFile, wxGetStockLabel(wxID_FILE));

	// A nice touch: a history of files visited. Use this menu.
	m_docManager->FileHistoryUseMenu(m_menuFile);

#if wxUSE_CONFIG
	m_docManager->FileHistoryLoad(*wxConfig::Get());
#endif // wxUSE_CONFIG

	m_menuEdit = new wxMenu;
	m_menuEdit->Append(wxID_UNDO);
	m_menuEdit->Append(wxID_REDO);
	m_menuEdit->AppendSeparator();
	m_menuEdit->Append(wxID_CUT);
	m_menuEdit->Append(wxID_COPY);
	m_menuEdit->Append(wxID_PASTE);
	m_menuEdit->Append(wxID_DELETE);
	m_menuEdit->Append(wxID_SELECTALL);
	m_menuEdit->AppendSeparator();
	m_menuEdit->Append(wxID_FIND);

	menuBar->Append(m_menuEdit, wxGetStockLabel(wxID_EDIT));

	m_menuOperations = new wxMenu;
	m_menuOperations->Append(wxID_ENTERPRISE_ALL_OPERATIONS, _("All operations..."));
	menuBar->Append(m_menuOperations, _("Operations"));

	m_menuSetting = new wxMenu;
	menuBar->Append(m_menuSetting, _("Tools"));
	m_menuSetting->Append(wxID_ENTERPRISE_SETTING, _("Options..."));

	m_menuAdministration = new wxMenu;
	m_menuAdministration->Append(wxID_ENTERPRISE_ACTIVE_USERS, _("Active users"));
	menuBar->Append(m_menuAdministration, _("Administration"));

	m_menuHelp = new wxMenu;
	m_menuHelp->Append(wxID_ENTERPRISE_ABOUT, _("About"));
	menuBar->Append(m_menuHelp, wxGetStockLabel(wxID_HELP, wxSTOCK_NOFLAGS));

	m_keyBinder.AddCommandsFromMenuBar(menuBar);

	SetDefaultHotKeys();
	SetMenuBar(menuBar);

	Bind(wxEVT_MENU, &CDocEnterpriseMDIFrame::OnClickAllOperation, this, wxID_ENTERPRISE_ALL_OPERATIONS);
	Bind(wxEVT_MENU, &CDocEnterpriseMDIFrame::OnToolsSettings, this, wxID_ENTERPRISE_SETTING);
	Bind(wxEVT_MENU, &CDocEnterpriseMDIFrame::OnActiveUsers, this, wxID_ENTERPRISE_ACTIVE_USERS);
	Bind(wxEVT_MENU, &CDocEnterpriseMDIFrame::OnAbout, this, wxID_ENTERPRISE_ABOUT);

	m_keyBinder.UpdateWindow(this);
	m_keyBinder.UpdateMenuBar(GetMenuBar());
}