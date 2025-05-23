#include "propertyEditor.h"

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/props.h>

#include <wx/slider.h>
#include <wx/odcombo.h>
#include <wx/stattext.h>

// -----------------------------------------------------------------------
// wxComboBox-based property editor
// -----------------------------------------------------------------------

WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(ComboBoxAndButton,
	wxPGComboBoxAndButtonEditor,
	wxPGComboBoxEditor)

	wxPGWindowList wxPGComboBoxAndButtonEditor::CreateControls(wxPropertyGrid* propGrid,
		wxPGProperty* property,
		const wxPoint& pos,
		const wxSize& sz) const
{
	// Use one two units smaller to match size of the combo's dropbutton.
	// (normally a bigger button is used because it looks better)
	int bt_wid = sz.y;
	bt_wid -= 2;
	wxSize bt_sz(bt_wid, bt_wid);

	// Position of button.
	wxPoint bt_pos(pos.x + sz.x - bt_sz.x, pos.y);
#ifdef __WXMAC__
	bt_pos.y -= 1;
#else
	bt_pos.y += 1;
#endif

	wxWindow* bt = propGrid->GenerateEditorButton(bt_pos, bt_sz);

	// Size of choice.
	wxSize ch_sz(sz.x - bt->GetSize().x, sz.y);

#ifdef __WXMAC__
	ch_sz.x -= wxPG_TEXTCTRL_AND_BUTTON_SPACING;
#endif

	wxWindow* ch = CreateControlsBase(propGrid, property,
		pos, ch_sz, 0);

	if (!property->HasFlag(wxPG_PROP_ACTIVE_BTN)) {
		bt->Disable();
	}

#ifdef __WXMSW__
	bt->Show();
#endif

	property->SetAttribute(wxT("hyperLink_clicked"), WXVARIANT(false));

	return wxPGWindowList(ch, bt);
}

bool wxPGComboBoxAndButtonEditor::OnEvent(wxPropertyGrid* propGrid,
	wxPGProperty* property,
	wxWindow* ctrl,
	wxEvent& event) const
{
	wxOwnerDrawnComboBox* cb =
		dynamic_cast<wxOwnerDrawnComboBox*>(ctrl);

	wxWindow* textCtrl = cb ?
		cb->GetTextCtrl() : nullptr;

	wxWindow* wndEditor2 =
		propGrid->GetEditorControlSecondary();

	if (wndEditor2 != nullptr) {
		bool needButton = property->HasFlag(wxPG_PROP_ACTIVE_BTN);
		if (needButton != wndEditor2->IsEnabled()) {
			wndEditor2->Enable(needButton);
		}
	}

	if (wxPGTextCtrlEditor::OnTextCtrlEvent(propGrid, property, textCtrl, event))
		return true;

	return wxPGChoiceEditor::OnEvent(propGrid, property, ctrl, event);
}

void wxPGComboBoxAndButtonEditor::UpdateControl(wxPGProperty* property, wxWindow* ctrl) const
{
	wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;
	const int index = property->GetChoiceSelection();
	wxString s = property->GetValueAsString(wxPG_EDITABLE_VALUE);
	cb->SetSelection(index);
	property->GetGrid()->SetupTextCtrlValue(s);
	cb->SetValue(s);
}

bool wxPGComboBoxAndButtonEditor::GetValueFromControl(wxVariant& variant, wxPGProperty* property, wxWindow* ctrl) const
{
	wxOwnerDrawnComboBox* cb = (wxOwnerDrawnComboBox*)ctrl;
	wxString textVal = cb->GetValue();

	if (property->UsesAutoUnspecified() && textVal.empty()) {
		variant.MakeNull();
		return true;
	}

	int selection = cb->FindString(textVal); bool res = true;

	if (selection != wxNOT_FOUND) {
		res = property->IntToValue(variant, selection, wxPG_EDITABLE_VALUE | wxPG_PROPERTY_SPECIFIC);
	}
	else {
		res = property->StringToValue(variant, textVal, wxPG_EDITABLE_VALUE | wxPG_PROPERTY_SPECIFIC);
	}

	// Changing unspecified always causes event (returning
	// true here should be enough to trigger it).
	if (!res && variant.IsNull())
		res = true;

	return res;
}

wxPGComboBoxAndButtonEditor::~wxPGComboBoxAndButtonEditor()
{
	wxPG_EDITOR(ComboBoxAndButton) = nullptr;
}

// -----------------------------------------------------------------------
// wxSlider-based property editor
// -----------------------------------------------------------------------
#ifdef wxUSE_SLIDER

WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(Slider,
	wxPGSliderEditor,
	wxPGEditor)

	wxPGSliderEditor::~wxPGSliderEditor()
{
}

// Create controls and initialize event handling.
wxPGWindowList wxPGSliderEditor::CreateControls(wxPropertyGrid* propgrid,
	wxPGProperty* property,
	const wxPoint& pos,
	const wxSize& sz) const
{
	wxCHECK_MSG(property->IsKindOf(wxCLASSINFO(wxFloatProperty)),
		nullptr,
		wxT("Slider editor can only be used with wxFloatProperty or derivative."));

	// Use two stage creation to allow cleaner display on wxMSW
	wxSlider* ctrl = new wxSlider();
#ifdef __WXMSW__
	ctrl->Hide();
#endif
	wxString s = property->GetValueAsString();
	double v_d = 0;
	if (s.ToDouble(&v_d))
	{
		if (v_d < 0)
			v_d = 0;
		else if (v_d > 1)
			v_d = 1;
	}

	ctrl->Create(propgrid->GetPanel(),
		wxID_ANY,
		(int)(v_d * m_max),
		0,
		m_max,
		pos,
		sz,
		wxSL_HORIZONTAL);

	// Connect all required events to grid's OnCustomEditorEvent
	// (all relevenat wxTextCtrl, wxComboBox and wxButton events are
	// already connected)

#ifdef __WXMSW__
	ctrl->Show();
#endif

	return ctrl;
}

// Copies value from property to control
void wxPGSliderEditor::UpdateControl(wxPGProperty* property, wxWindow* wnd) const
{
	wxSlider* ctrl = (wxSlider*)wnd;
	assert(ctrl && ctrl->IsKindOf(CLASSINFO(wxSlider)));

	double val = property->GetValue().GetDouble();
	if (val < 0)
		val = 0;
	else if (val > 1)
		val = 1;

	ctrl->SetValue((int)(val * m_max));
}

// Control's events are redirected here
bool wxPGSliderEditor::OnEvent(wxPropertyGrid* WXUNUSED(propgrid),
	wxPGProperty* property, wxWindow* wnd, wxEvent& event) const
{
	if (event.GetEventType() == wxEVT_SCROLL_THUMBTRACK)
	{
		wxSlider* ctrl = wxDynamicCast(wnd, wxSlider);
		if (ctrl)
		{
			double val = (double)(ctrl->GetValue()) / (double)(m_max);
			property->SetValue(WXVARIANT(val));
			return true;
		}
	}

	return false;
}

bool wxPGSliderEditor::GetValueFromControl(wxVariant& variant, wxPGProperty* WXUNUSED(property), wxWindow* wnd) const
{
	wxSlider* ctrl = (wxSlider*)wnd;
	assert(ctrl && ctrl->IsKindOf(CLASSINFO(wxSlider)));

	variant = WXVARIANT((double)ctrl->GetValue() / (double)(m_max));

	return true;
}

void wxPGSliderEditor::SetValueToUnspecified(wxPGProperty* WXUNUSED(property), wxWindow* WXUNUSED(ctrl)) const
{
}

#endif //wxUSE_SLIDER

WX_PG_IMPLEMENT_INTERNAL_EDITOR_CLASS(HyperLink,
	wxPGHyperLinkEditor,
	wxPGEditor)

	wxPGWindowList wxPGHyperLinkEditor::CreateControls(wxPropertyGrid* propGrid,
		wxPGProperty* property,
		const wxPoint& pos,
		const wxSize& sz) const
{

	// Use two stage creation to allow cleaner display on wxMSW
	wxStaticText* ctrl = new wxStaticText();

#ifdef __WXMSW__
	ctrl->Hide();
#endif

	ctrl->Create(propGrid->GetPanel(),
		wxID_ANY,
		property->GetValueAsString(),
		{ pos.x + 3, pos.y + 2 },
		{ sz.x - 3, sz.y - 2 },
		wxBORDER_NONE);

	ctrl->SetBackgroundColour(propGrid->GetPropertyBackgroundColour(property));
	ctrl->SetForegroundColour(propGrid->GetPropertyTextColour(property));

	wxFont font = ctrl->GetFont();
	font.SetUnderlined(true);
	ctrl->SetFont(font);

	ctrl->SetCursor(wxCURSOR_HAND);

	// Connect all required events to grid's OnCustomEditorEvent
	// (all relevenat wxTextCtrl, wxComboBox and wxButton events are
	// already connected)

#ifdef __WXMSW__
	ctrl->Show();
#endif

	return ctrl;
}

bool wxPGHyperLinkEditor::OnEvent(wxPropertyGrid* propGrid,
	wxPGProperty* property,
	wxWindow* wnd,
	wxEvent& event) const
{
	if (event.GetEventType() == wxEVT_LEFT_DOWN)
	{
		wxStaticText* ctrl = wxDynamicCast(wnd, wxStaticText);
		if (ctrl)
		{
			property->SetValue(wxVariant(true, wxT("hyperLink_clicked")));
			return true;
		}
	}
	return false;
}

void wxPGHyperLinkEditor::UpdateControl(wxPGProperty* property, wxWindow* wnd) const
{
	wxStaticText* ctrl = (wxStaticText*)wnd;
	assert(ctrl && ctrl->IsKindOf(CLASSINFO(wxStaticText)));
}

bool wxPGHyperLinkEditor::GetValueFromControl(wxVariant& variant, wxPGProperty* property, wxWindow* wnd) const
{
	wxStaticText* ctrl = (wxStaticText*)wnd;
	assert(ctrl && ctrl->IsKindOf(CLASSINFO(wxStaticText)));
	return false;
}

wxPGHyperLinkEditor::~wxPGHyperLinkEditor()
{
	wxPG_EDITOR(HyperLink) = nullptr;
}

#include <wx/module.h>

class wxOESEditorModule : public wxModule
{
public:
	wxOESEditorModule() : wxModule() {}
	virtual bool OnInit() {
		//register new editor 
		wxPGRegisterEditorClass(ComboBoxAndButton);
		wxPGRegisterEditorClass(Slider);
		wxPGRegisterEditorClass(HyperLink);
		return true;
	}
	virtual void OnExit() {}
private:
	wxDECLARE_DYNAMIC_CLASS(wxOESEditorModule);
};

wxIMPLEMENT_DYNAMIC_CLASS(wxOESEditorModule, wxModule)