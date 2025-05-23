
#include "widgets.h"
#include "backend/compiler/procUnit.h"

wxIMPLEMENT_DYNAMIC_CLASS(CValueRadioButton, IValueWindow)

//****************************************************************************
//*                             Radiobutton                                  *
//****************************************************************************

CValueRadioButton::CValueRadioButton() : IValueWindow()
{
}

wxObject* CValueRadioButton::Create(wxWindow* wxparent, IVisualHost *visualHost) 
{
	wxRadioButton *radioButton = new wxRadioButton(wxparent, wxID_ANY,
		m_propertyCaption->GetValueAsString(),
		wxDefaultPosition,
		wxDefaultSize);

	return radioButton;
}

void CValueRadioButton::OnCreated(wxObject* wxobject, wxWindow* wxparent, IVisualHost *visualHost, bool first�reated)
{
}

void CValueRadioButton::Update(wxObject* wxobject, IVisualHost *visualHost)
{
	wxRadioButton *radioButton = dynamic_cast<wxRadioButton *>(wxobject);

	if (radioButton != nullptr) {
		radioButton->SetLabel(m_propertyCaption->GetValueAsString());
		radioButton->SetValue(m_propertySelected->GetValueAsBoolean() != false);
	}

	UpdateWindow(radioButton);
}

void CValueRadioButton::Cleanup(wxObject* obj, IVisualHost *visualHost)
{
}

//*******************************************************************
//*                             Property                            *
//*******************************************************************

bool CValueRadioButton::LoadData(CMemoryReader &reader)
{
	return IValueWindow::LoadData(reader);
}

bool CValueRadioButton::SaveData(CMemoryWriter &writer)
{
	return IValueWindow::SaveData(writer);
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

CONTROL_TYPE_REGISTER(CValueRadioButton, "radiobutton", "widget", string_to_clsid("CT_RDBT"));