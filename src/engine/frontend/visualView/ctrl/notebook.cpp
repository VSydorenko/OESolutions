#include "notebook.h"
#include "form.h"

//***********************************************************************************
//*                           IMPLEMENT_DYNAMIC_CLASS                               *
//***********************************************************************************

wxIMPLEMENT_DYNAMIC_CLASS(CValueNotebook, IValueWindow);

//***********************************************************************************
//*                                 Special Notebook func                           *
//***********************************************************************************

void CValueNotebook::AddNotebookPage()
{
	wxASSERT(m_formOwner);

	IValueFrame* newNotebookPage = m_formOwner->NewObject(g_controlNotebookPageCLSID, this);
	g_visualHostContext->InsertControl(newNotebookPage, this);
	g_visualHostContext->RefreshEditor();
}

//***********************************************************************************
//*                                 Value Notebook                                  *
//***********************************************************************************

CValueNotebook::CValueNotebook() : IValueWindow(), m_activePage(nullptr)
{
	//set default params
	//m_minimum_size = wxSize(300, 100);
}

#include "frontend/win/theme/luna_tabart.h"

wxObject* CValueNotebook::Create(wxWindow* wxparent, IVisualHost* visualHost)
{
	long style = m_propertyOrient->GetValueAsInteger() |
		wxAUI_NB_TAB_MOVE |
		wxAUI_NB_SCROLL_BUTTONS;
	if (!visualHost->IsDesignerHost())
		style |= wxAUI_NB_TAB_SPLIT;
	wxAuiNotebook* notebook = new wxAuiNotebook(wxparent, wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize, style);
	notebook->SetArtProvider(new wxAuiLunaTabArt());
	return notebook;
}

void CValueNotebook::OnCreated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost, bool first�reated)
{
	if (visualHost->IsDesignerHost() && GetChildCount() == 0
		&& first�reated) {
		CValueNotebook::AddNotebookPage();
	}

	wxAuiNotebook* notebook = dynamic_cast<wxAuiNotebook*>(wxobject);
	if (notebook != nullptr) {
		notebook->Bind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &CValueNotebook::OnPageChanged, this);
		notebook->Bind(wxEVT_AUINOTEBOOK_END_DRAG, &CValueNotebook::OnEndDrag, this);
		notebook->Bind(wxEVT_AUINOTEBOOK_BG_DCLICK, &CValueNotebook::OnBGDClick, this);
	}
}

void CValueNotebook::OnSelected(wxObject* wxobject)
{
}

void CValueNotebook::Update(wxObject* wxobject, IVisualHost* visualHost)
{
	wxAuiNotebook* notebook = dynamic_cast<wxAuiNotebook*>(wxobject);
	if (notebook != nullptr) {
	}

	UpdateWindow(notebook);
}

void CValueNotebook::OnUpdated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost)
{
	wxAuiNotebook* notebook = dynamic_cast<wxAuiNotebook*>(wxobject);

	if (notebook != nullptr) {
		long style = m_propertyOrient->GetValueAsInteger() |
			wxAUI_NB_TAB_MOVE |
			wxAUI_NB_SCROLL_BUTTONS;
		if (!visualHost->IsDesignerHost())
			style |= wxAUI_NB_TAB_SPLIT;
		notebook->SetWindowStyle(style);
	}
}

void CValueNotebook::Cleanup(wxObject* wxobject, IVisualHost* visualHost)
{
	wxAuiNotebook* notebook = dynamic_cast<wxAuiNotebook*>(wxobject);
	if (notebook != nullptr) {
		notebook->Unbind(wxEVT_AUINOTEBOOK_PAGE_CHANGED, &CValueNotebook::OnPageChanged, this);
		notebook->Unbind(wxEVT_AUINOTEBOOK_END_DRAG, &CValueNotebook::OnEndDrag, this);
	}
}

//**********************************************************************************
//*                                   Data		                                   *
//**********************************************************************************

bool CValueNotebook::LoadData(CMemoryReader& reader)
{
	m_propertyOrient->SetValue(reader.r_s32());
	return IValueWindow::LoadData(reader);
}

bool CValueNotebook::SaveData(CMemoryWriter& writer)
{
	writer.w_s32(m_propertyOrient->GetValueAsInteger());
	return IValueWindow::SaveData(writer);
}

//**********************************************************************************

enum Func {
	enPages = 0,
	enActivePage
};

void CValueNotebook::PrepareNames() const // this method is automatically called to initialize attribute and method names.
{
	IValueFrame::PrepareNames();

	m_methodHelper->AppendFunc(wxT("pages"), "pages()");
	m_methodHelper->AppendFunc(wxT("activePage"), "activePage()");
}

#include "backend/compiler/value/valueMap.h"

bool CValueNotebook::CallAsFunc(const long lMethodNum, CValue& pvarRetValue, CValue** paParams, const long lSizeArray)       //method call
{
	switch (lMethodNum)
	{
	case enPages:
	{
		CValueStructure* pages = new CValueStructure(true);
		for (unsigned int i = 0; i < GetChildCount(); i++) {
			CValueNotebookPage* notebookPage = dynamic_cast<CValueNotebookPage*>(GetChild(i));
			if (notebookPage) {
				pages->Insert(notebookPage->GetControlName(), CValue(notebookPage));
			}
		}
#pragma message("nouverbe to nouverbe: ���������� ����������!")
		pvarRetValue = pages;
		return true; 
	}
	case enActivePage:
		pvarRetValue = m_activePage;
		return true;
	}

	return IValueWindow::CallAsFunc(lMethodNum, pvarRetValue, paParams, lSizeArray);
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

CONTROL_TYPE_REGISTER(CValueNotebook, "notebook", "notebook", g_controlNotebookCLSID);