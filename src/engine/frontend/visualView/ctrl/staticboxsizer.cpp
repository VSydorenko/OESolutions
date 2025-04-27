
#include "sizers.h"

wxIMPLEMENT_DYNAMIC_CLASS(CValueStaticBoxSizer, IValueSizer)

//****************************************************************************
//*                             StaticBoxSizer                               *
//****************************************************************************

CValueStaticBoxSizer::CValueStaticBoxSizer() : IValueSizer()
{
}

wxObject* CValueStaticBoxSizer::Create(wxWindow* wxparent, IVisualHost* visualHost)
{
	wxStaticBox* m_staticBox = new wxStaticBox(wxparent, wxID_ANY, m_propertyCaption->GetValueAsString());
	return new wxStaticBoxSizer(m_staticBox, m_propertyOrient->GetValueAsInteger());
}

void CValueStaticBoxSizer::OnCreated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost, bool first�reated)
{
	wxStaticBoxSizer* staticboxsizer = dynamic_cast<wxStaticBoxSizer*>(wxobject);
	wxStaticBox* wndBox = staticboxsizer->GetStaticBox();
	wxASSERT(wndBox);
	
	if (visualHost->IsDesignerHost()) {
		wndBox->PushEventHandler(g_visualHostContext->GetHighlightPaintHandler(wndBox));
	}
}

void CValueStaticBoxSizer::Update(wxObject* wxobject, IVisualHost* visualHost)
{
	wxStaticBoxSizer* m_staticboxsizer = dynamic_cast<wxStaticBoxSizer*>(wxobject);
	wxStaticBox* m_wndBox = m_staticboxsizer->GetStaticBox();
	wxASSERT(m_wndBox);
	if (m_staticboxsizer != nullptr) {
		m_wndBox->SetLabel(m_propertyCaption->GetValueAsString());
		m_wndBox->SetMinSize(m_propertyMinSize->GetValueAsSize());

		m_wndBox->SetFont(m_propertyFont->GetValueAsFont());
		m_wndBox->SetForegroundColour(m_propertyFG->GetValueAsColour());
		m_wndBox->SetBackgroundColour(m_propertyBG->GetValueAsColour());
		m_wndBox->Enable(m_propertyEnabled->GetValueAsBoolean());
		m_wndBox->Show(m_propertyVisible->GetValueAsBoolean());
		m_wndBox->SetToolTip(m_propertyTooltip->GetValueAsString());

		m_staticboxsizer->SetOrientation(m_propertyOrient->GetValueAsInteger());
		m_staticboxsizer->SetMinSize(m_propertyMinSize->GetValueAsSize());

		//after lay out 
		if (m_propertyMinSize->GetValueAsSize() != wxDefaultSize) {
			m_wndBox->Layout();
		}
	}

	UpdateSizer(m_staticboxsizer);
}

void CValueStaticBoxSizer::Cleanup(wxObject* wxobject, IVisualHost* visualHost)
{
	wxStaticBoxSizer* m_staticboxsizer = dynamic_cast<wxStaticBoxSizer*>(wxobject);
	wxStaticBox* m_wndBox = m_staticboxsizer->GetStaticBox();
	wxASSERT(m_wndBox);
	if (visualHost->IsDesignerHost()) {
		m_wndBox->PopEventHandler(true);
	}
}

//**********************************************************************************
//*                                    Data										   *
//**********************************************************************************



bool CValueStaticBoxSizer::LoadData(CMemoryReader& reader)
{
	m_propertyOrient->SetValue(reader.r_u16());	
	wxString propValue = wxEmptyString;
	reader.r_stringZ(propValue);
	m_propertyCaption->SetValue(propValue);
	reader.r_stringZ(propValue);
	m_propertyFont->SetValue(typeConv::StringToFont(propValue));
	reader.r_stringZ(propValue);
	m_propertyFG->SetValue(typeConv::StringToColour(propValue));
	reader.r_stringZ(propValue);
	m_propertyBG->SetValue(typeConv::StringToColour(propValue));

	reader.r_stringZ(propValue);
	m_propertyTooltip->SetValue(propValue);
	reader.r_stringZ(propValue);
	m_propertyContextHelp->SetValue(propValue);

	m_propertyContextMenu->SetValue(reader.r_u8());
	m_propertyEnabled->SetValue(reader.r_u8());
	m_propertyVisible->SetValue(reader.r_u8());

	return IValueSizer::LoadData(reader);
}

bool CValueStaticBoxSizer::SaveData(CMemoryWriter& writer)
{
	writer.w_u16(m_propertyOrient->GetValueAsInteger());
	writer.w_stringZ(m_propertyCaption->GetValueAsString());
	writer.w_stringZ(m_propertyFont->GetValueAsString());
	writer.w_stringZ(m_propertyFG->GetValueAsString());
	writer.w_stringZ(m_propertyBG->GetValueAsString());
	writer.w_stringZ(m_propertyTooltip->GetValueAsString());
	writer.w_stringZ(m_propertyContextHelp->GetValueAsString());
	writer.w_u8(m_propertyContextMenu->GetValueAsBoolean());
	writer.w_u8(m_propertyEnabled->GetValueAsBoolean());
	writer.w_u8(m_propertyVisible->GetValueAsBoolean());

	return IValueSizer::SaveData(writer);
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

CONTROL_TYPE_REGISTER(CValueStaticBoxSizer, "staticboxsizer", "sizer", string_to_clsid("CT_SSZER"));