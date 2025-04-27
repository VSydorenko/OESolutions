
#include "widgets.h"
#include "backend/compiler/procUnit.h"

wxIMPLEMENT_DYNAMIC_CLASS(CValueStaticLine, IValueWindow)

//****************************************************************************
//*                             StaticLine                                   *
//****************************************************************************

CValueStaticLine::CValueStaticLine() : IValueWindow()
{
}

wxObject* CValueStaticLine::Create(wxWindow* wxparent, IVisualHost* visualHost)
{
	wxStaticLine* staticline = new wxStaticLine(wxparent, wxID_ANY,
		wxDefaultPosition,
		wxDefaultSize,
		m_propertyOrient->GetValueAsEnum()
	);

	return staticline;
}

void CValueStaticLine::OnCreated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost, bool first�reated)
{
}

void CValueStaticLine::Update(wxObject* wxobject, IVisualHost* visualHost)
{
	wxStaticLine* staticline = dynamic_cast<wxStaticLine*>(wxobject);

	if (staticline != nullptr) {
		wxWindow* winParent = staticline->GetParent();
		bool isShown = staticline->IsShown();
		if (isShown) staticline->Hide();
		staticline->SetParent(nullptr); winParent->RemoveChild(staticline);
		staticline->DissociateHandle();
		staticline->Create(winParent, wxID_ANY,
			wxDefaultPosition,
			wxDefaultSize,
			m_propertyOrient->GetValueAsEnum()
		);
		staticline->Show(isShown);
	}

	UpdateWindow(staticline);
}

void CValueStaticLine::OnUpdated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost)
{
}

void CValueStaticLine::Cleanup(wxObject* obj, IVisualHost* visualHost)
{
}

//*******************************************************************
//*                             Property                            *
//*******************************************************************

bool CValueStaticLine::LoadData(CMemoryReader& reader)
{
	m_propertyOrient->SetValue(reader.r_s32());
	return IValueWindow::LoadData(reader);
}

bool CValueStaticLine::SaveData(CMemoryWriter& writer)
{
	writer.w_s32(m_propertyOrient->GetValueAsInteger());
	return IValueWindow::SaveData(writer);
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

CONTROL_TYPE_REGISTER(CValueStaticLine, "staticline", "widget", string_to_clsid("CT_STLI"));