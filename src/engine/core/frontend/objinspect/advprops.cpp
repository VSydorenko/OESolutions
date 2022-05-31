#include "advprops.h"

#include "appData.h"
#include "utils/typeconv.h"
#include "metadata/metaObjects/objects/baseObject.h"

#include <wx/regex.h>
#include <wx/spinctrl.h>

// -----------------------------------------------------------------------
// wxPGSizeProperty
// -----------------------------------------------------------------------
#if wxCHECK_VERSION(3, 1, 0)
wxPG_IMPLEMENT_PROPERTY_CLASS(wxPGSizeProperty, wxPGProperty, TextCtrl)
#else
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxPGSizeProperty, wxPGProperty, wxSize, const wxSize&, TextCtrl)
#endif

wxPGSizeProperty::wxPGSizeProperty(const wxString& label,
	const wxString& name,
	const wxSize& value) : wxPGProperty(label, name)
{
	DoSetValue(value);
	AddPrivateChild(new wxIntProperty(wxT("Width"), wxPG_LABEL, value.x));
	AddPrivateChild(new wxIntProperty(wxT("Height"), wxPG_LABEL, value.y));
}

void wxPGSizeProperty::RefreshChildren()
{
	if (GetChildCount() < 2) return;

	const wxSize& size = wxSizeRefFromVariant(m_value);

	Item(0)->SetValue((long)size.x);
	Item(1)->SetValue((long)size.y);
}

wxVariant wxPGSizeProperty::ChildChanged(wxVariant& thisValue, const int childIndex,
	wxVariant& childValue) const {
	wxSize& size = wxSizeRefFromVariant(thisValue);

	int val = childValue.GetLong();
	switch (childIndex)
	{
	case 0:
		size.x = val;
		break;
	case 1:
		size.y = val;
		break;
	}

	wxVariant newVariant;
	newVariant << size;
	return newVariant;
}

// -----------------------------------------------------------------------
// wxPGPointProperty
// -----------------------------------------------------------------------

#if wxCHECK_VERSION(3, 1, 0)
wxPG_IMPLEMENT_PROPERTY_CLASS(wxPGPointProperty, wxPGProperty, TextCtrl)
#else
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxPGPointProperty, wxPGProperty, wxPoint, const wxPoint&, TextCtrl)
#endif

wxPGPointProperty::wxPGPointProperty(const wxString& label,
	const wxString& name,
	const wxPoint& value) : wxPGProperty(label, name)
{
	DoSetValue(value);
	AddPrivateChild(new wxIntProperty(wxT("X"), wxPG_LABEL, value.x));
	AddPrivateChild(new wxIntProperty(wxT("Y"), wxPG_LABEL, value.y));
}

wxPGPointProperty::~wxPGPointProperty() { }

void wxPGPointProperty::RefreshChildren()
{
	if (GetChildCount() < 2) return;

	const wxPoint& point = wxPointRefFromVariant(m_value);

	Item(0)->SetValue((long)point.x);
	Item(1)->SetValue((long)point.y);
}

wxVariant wxPGPointProperty::ChildChanged(wxVariant& thisValue, const int childIndex,
	wxVariant& childValue) const {
	wxPoint& point = wxPointRefFromVariant(thisValue);

	int val = childValue.GetLong();
	switch (childIndex)
	{
	case 0:
		point.x = val;
		break;
	case 1:
		point.y = val;
		break;
	}

	wxVariant newVariant;
	newVariant << point;
	return newVariant;
}

// -----------------------------------------------------------------------
// wxPGBitmapProperty
// -----------------------------------------------------------------------

// static long gs_imageFilterIndex = -1; TODO: new wxPropertyGrid misses the
//                                             wxPG_FILE_FILTER_INDEX attribute ID
static wxString gs_imageInitialPath = wxEmptyString;

#if wxCHECK_VERSION(3, 1, 0)
wxPG_IMPLEMENT_PROPERTY_CLASS(wxPGBitmapProperty, wxPGProperty, TextCtrl)
#else
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxPGBitmapProperty, wxPGProperty,
	wxString, const wxString&, TextCtrl)
#endif

	void wxPGBitmapProperty::GetChildValues(const wxString& parentValue, wxArrayString& childValues) const
{
	// some properties can contain value like "[-1;-1]" which must be modified due to use of ";" as a
	// string separator
	wxString values = parentValue;

	wxRegEx regex(wxT("\\[.+;.+\\]"));
	if (regex.IsValid())
	{
		if (regex.Matches(values))
		{
			wxString sizeVal = regex.GetMatch(values);
			sizeVal.Replace(wxT(";"), wxT("<semicolon>"));
			sizeVal.Replace(wxT("["), wxT(""));
			sizeVal.Replace(wxT("]"), wxT(""));
			regex.Replace(&values, sizeVal);
		}
	}

	childValues = wxStringTokenize(values, wxT(';'), wxTOKEN_RET_EMPTY_ALL);
	for (wxArrayString::iterator value = childValues.begin(); value != childValues.end(); ++value)
	{
		value->Trim(false);
		value->Replace(wxT("<semicolon>"), wxT(";"));
	}
}

wxPGBitmapProperty::wxPGBitmapProperty(const wxString& label,
	const wxString& name,
	const wxString& value) : wxPGProperty(label, name)
{
	SetValue(WXVARIANT(value));
}

void wxPGBitmapProperty::CreateChildren()
{
	wxString  propValue = m_value.GetString();
	wxVariant thisValue = WXVARIANT(propValue);
	wxVariant childValue;
	int       childIndex = 0;
	wxArrayString childVals;
	GetChildValues(propValue, childVals);
	wxString  source;
	if (childVals.Count() > 0)
	{
		source = childVals.Item(0);
	}
	else
	{
		source = _("Load From File");
	}
	prevSrc = -1;
	if (source == wxString(_("Load From Art Provider")))
	{
		childIndex = 1;
	}

	childValue = WXVARIANT(childIndex);

	CreatePropertySource(childIndex);

	ChildChanged(thisValue, 0, childValue);
}

wxPGProperty* wxPGBitmapProperty::CreatePropertySource(int sourceIndex)
{
	wxPGChoices sourceChoices;

	// Add 'source' property (common for all other children)
	sourceChoices.Add(_("Load From Art Provider"));

	wxPGProperty* srcProp = new wxEnumProperty(wxT("source"), wxPG_LABEL, sourceChoices, sourceIndex);
	srcProp->SetHelpString(
		wxString(_("Load From Art Provider:\n")) +
		wxString(_("Query registered providers for bitmap with given ID.\n\n")));
	AppendChild(srcProp);

	return srcProp;
}

wxPGProperty* wxPGBitmapProperty::CreatePropertyArtId()
{
	wxPGChoices artIdChoices;

	// Create 'id' property ('Load From Art Provider' only)
	artIdChoices.Add(wxT("wxART_ADD_BOOKMARK"));
	artIdChoices.Add(wxT("wxART_DEL_BOOKMARK"));
	artIdChoices.Add(wxT("wxART_HELP_SIDE_PANEL"));
	artIdChoices.Add(wxT("wxART_HELP_SETTINGS"));
	artIdChoices.Add(wxT("wxART_HELP_BOOK"));
	artIdChoices.Add(wxT("wxART_HELP_FOLDER"));
	artIdChoices.Add(wxT("wxART_HELP_PAGE"));
	artIdChoices.Add(wxT("wxART_GO_BACK"));
	artIdChoices.Add(wxT("wxART_GO_FORWARD"));
	artIdChoices.Add(wxT("wxART_GO_UP"));
	artIdChoices.Add(wxT("wxART_GO_DOWN"));
	artIdChoices.Add(wxT("wxART_GO_TO_PARENT"));
	artIdChoices.Add(wxT("wxART_GO_HOME"));
	artIdChoices.Add(wxT("wxART_FILE_OPEN"));
	artIdChoices.Add(wxT("wxART_FILE_SAVE"));
	artIdChoices.Add(wxT("wxART_FILE_SAVE_AS"));
	artIdChoices.Add(wxT("wxART_GOTO_FIRST"));
	artIdChoices.Add(wxT("wxART_GOTO_LAST"));
	artIdChoices.Add(wxT("wxART_PRINT"));
	artIdChoices.Add(wxT("wxART_HELP"));
	artIdChoices.Add(wxT("wxART_TIP"));
	artIdChoices.Add(wxT("wxART_REPORT_VIEW"));
	artIdChoices.Add(wxT("wxART_LIST_VIEW"));
	artIdChoices.Add(wxT("wxART_NEW_DIR"));
	artIdChoices.Add(wxT("wxART_HARDDISK"));
	artIdChoices.Add(wxT("wxART_FLOPPY"));
	artIdChoices.Add(wxT("wxART_CDROM"));
	artIdChoices.Add(wxT("wxART_REMOVABLE"));
	artIdChoices.Add(wxT("wxART_FOLDER"));
	artIdChoices.Add(wxT("wxART_FOLDER_OPEN"));
	artIdChoices.Add(wxT("wxART_GO_DIR_UP"));
	artIdChoices.Add(wxT("wxART_EXECUTABLE_FILE"));
	artIdChoices.Add(wxT("wxART_NORMAL_FILE"));
	artIdChoices.Add(wxT("wxART_TICK_MARK"));
	artIdChoices.Add(wxT("wxART_CROSS_MARK"));
	artIdChoices.Add(wxT("wxART_ERROR"));
	artIdChoices.Add(wxT("wxART_QUESTION"));
	artIdChoices.Add(wxT("wxART_WARNING"));
	artIdChoices.Add(wxT("wxART_INFORMATION"));
	artIdChoices.Add(wxT("wxART_MISSING_IMAGE"));
	artIdChoices.Add(wxT("wxART_COPY"));
	artIdChoices.Add(wxT("wxART_CUT"));
	artIdChoices.Add(wxT("wxART_PASTE"));
	artIdChoices.Add(wxT("wxART_DELETE"));
	artIdChoices.Add(wxT("wxART_NEW"));
	artIdChoices.Add(wxT("wxART_UNDO"));
	artIdChoices.Add(wxT("wxART_REDO"));
	artIdChoices.Add(wxT("wxART_PLUS"));
	artIdChoices.Add(wxT("wxART_MINUS"));
	artIdChoices.Add(wxT("wxART_CLOSE"));
	artIdChoices.Add(wxT("wxART_QUIT"));
	artIdChoices.Add(wxT("wxART_FIND"));
	artIdChoices.Add(wxT("wxART_FIND_AND_REPLACE"));
	artIdChoices.Add(wxT("wxART_FULL_SCREEN"));
	artIdChoices.Add(wxT("wxART_EDIT"));

	wxPGProperty* propArtId = new wxEditEnumProperty(wxT("id"), wxPG_LABEL, artIdChoices);
	propArtId->SetHelpString(_("Choose a wxArtID unique identifier of the bitmap or enter a wxArtID for your custom wxArtProvider. IDs with prefix 'gtk-' are available under wxGTK only."));

	return propArtId;
}

wxPGProperty* wxPGBitmapProperty::CreatePropertyArtClient()
{
	wxPGChoices artClientChoices;

	// Create 'client' property ('Load From Art Provider' only)
	artClientChoices.Add(wxT("wxART_TOOLBAR"));
	artClientChoices.Add(wxT("wxART_MENU"));
	artClientChoices.Add(wxT("wxART_BUTTON"));
	artClientChoices.Add(wxT("wxART_FRAME_ICON"));
	artClientChoices.Add(wxT("wxART_CMN_DIALOG"));
	artClientChoices.Add(wxT("wxART_HELP_BROWSER"));
	artClientChoices.Add(wxT("wxART_MESSAGE_BOX"));
	artClientChoices.Add(wxT("wxART_OTHER"));

	wxPGProperty* propArtClient = new wxEditEnumProperty(wxT("client"), wxPG_LABEL, artClientChoices);
	propArtClient->SetHelpString(_("Choose a wxArtClient identifier of the client (i.e. who is asking for the bitmap) or enter a wxArtClient for your custom wxArtProvider."));

	return propArtClient;
}

wxPGBitmapProperty::~wxPGBitmapProperty()
{
}

wxVariant wxPGBitmapProperty::ChildChanged(wxVariant& thisValue, const int childIndex,
	wxVariant& childValue) const {
	wxPGBitmapProperty* bp = (wxPGBitmapProperty*)this;

	wxString val = thisValue.GetString();
	wxArrayString childVals;
	GetChildValues(val, childVals);
	wxString newVal = val;

	// Find the appropriate new state
	switch (childIndex)
	{
		// source
	case 0:
	{
		unsigned int count = GetChildCount();

		if (prevSrc != 4)
		{
			for (unsigned int i = 1; i < count; i++)
			{
				wxPGProperty* p = Item(i);
				if (p)
				{
					wxLogDebug(wxT("wxOESBP::ChildChanged: Removing:%s"), p->GetLabel().c_str());
					GetGrid()->DeleteProperty(p);
				}
			}
			bp->AppendChild(bp->CreatePropertyArtId());
			bp->AppendChild(bp->CreatePropertyArtClient());
		}

		if (childVals.GetCount() == 3)
			newVal = childVals.Item(0) + wxT("; ") + childVals.Item(1) + wxT("; ") + childVals.Item(2);
		else if (childVals.GetCount() > 1)
			newVal = childVals.Item(0) + wxT("; ; ");
		break;
	}

	// file_path || id || resource_name
	case 1:
	{
		if (Item(0)->GetValueAsString() == _("Load From Art Provider")) {

			Item(1)->SetValue(childValue);

			if (childVals.GetCount() == 3)
				newVal = childVals.Item(0) + wxT("; ") + Item(1)->GetValueAsString() + wxT("; ") + Item(2)->GetValueAsString();
			else if (childVals.GetCount() > 1)
				newVal = childVals.Item(0) + wxT("; ; ");
		}
		break;
	}
	case 2:

		Item(2)->SetValue(childValue);

		if (childVals.GetCount() == 3)
			newVal = childVals.Item(0) + wxT("; ") + Item(1)->GetValueAsString() + wxT("; ") + Item(2)->GetValueAsString();
		else if (childVals.GetCount() > 1)
			newVal = childVals.Item(0) + wxT("; ; ");

		break;
	}

	bp->SetPrevSource(childValue.GetInteger());

	if (newVal != val)
	{
		wxVariant ret = WXVARIANT(newVal);
		bp->SetValue(ret);

		return ret;
	}
	return thisValue;
}

void wxPGBitmapProperty::UpdateChildValues(const wxString& value)
{
	wxArrayString childVals;
	GetChildValues(value, childVals);

	if (childVals[0].Contains(_("Load From Art Provider")))
	{
		Item(0)->SetValue(childVals[0]);

		if (childVals.Count() > 1)
		{
			wxString img = childVals[1];
			Item(1)->SetValue(childVals[1]);
		}

		if (childVals.Count() > 2)
		{
			wxString img = childVals[2];
			Item(2)->SetValue(childVals[2]);
		}
	}
}

void wxPGBitmapProperty::OnSetValue()
{
}

wxString wxPGBitmapProperty::SetupImage(const wxString& imgPath)
{
	if (!imgPath.IsEmpty())
	{
		wxFileName imgName = wxFileName(imgPath);

		// Allow user to specify any file path he needs (even if it seemingly doesn't exist)
		if (!imgName.FileExists()) return imgPath;

		wxString   res = wxT("");
		wxImage    img = wxImage(imgPath);

		if (!img.IsOk()) return res;

		// Setup for correct file_path
		if (imgName.IsAbsolute())
		{
			return TypeConv::MakeRelativeURL(imgPath, appData->GetProjectPath());
		}
		else
		{
			imgName.MakeAbsolute(appData->GetProjectPath());

			if (!imgName.FileExists()) return res;
		}
	}
	return imgPath;
}

wxString wxPGBitmapProperty::SetupResource(const wxString& resName)
{
	wxString res = wxEmptyString;
	// Keep old value from an icon resource only
	if (resName.Contains(wxT(";")) && resName.Contains(wxT("[")))
	{
		return resName.BeforeFirst(wxT(';'));
	}
	else if (resName.Contains(wxT(";")))
	{
		return res;
	}
	return resName;
}

// -----------------------------------------------------------------------
// wxPGFontProperty
// -----------------------------------------------------------------------

#include <wx/fontdlg.h>
#include <wx/fontenum.h>

static const wxChar* gs_fp_es_family_labels[] = {
	wxT("Default"), wxT("Decorative"),
	wxT("Roman"), wxT("Script"),
	wxT("Swiss"), wxT("Modern"),
	wxT("Teletype"), wxT("Unknown"),
	(const wxChar*)NULL
};

static long gs_fp_es_family_values[] = {
	wxFONTFAMILY_DEFAULT, wxFONTFAMILY_DECORATIVE,
	wxFONTFAMILY_ROMAN, wxFONTFAMILY_SCRIPT,
	wxFONTFAMILY_SWISS, wxFONTFAMILY_MODERN,
	wxFONTFAMILY_TELETYPE, wxFONTFAMILY_UNKNOWN
};

static const wxChar* gs_fp_es_style_labels[] = {
	wxT("Normal"),
	wxT("Slant"),
	wxT("Italic"),
	(const wxChar*)NULL
};

static long gs_fp_es_style_values[] = {
	wxFONTSTYLE_NORMAL,
	wxFONTSTYLE_SLANT,
	wxFONTSTYLE_ITALIC
};

static const wxChar* gs_fp_es_weight_labels[] = {
	wxT("Normal"),
	wxT("Light"),
	wxT("Bold"),
	(const wxChar*)NULL
};

static long gs_fp_es_weight_values[] = {
	wxFONTWEIGHT_NORMAL,
	wxFONTWEIGHT_LIGHT,
	wxFONTWEIGHT_BOLD
};

#if wxCHECK_VERSION(3, 1, 0)
wxPG_IMPLEMENT_PROPERTY_CLASS(wxPGFontProperty, wxPGProperty, TextCtrlAndButton)
#else
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxPGFontProperty, wxPGProperty,
	wxFont, const wxFont&, TextCtrlAndButton)
#endif


	wxPGFontProperty::wxPGFontProperty(const wxString& label, const wxString& name,
		const wxFontContainer& value)
	: wxPGProperty(label, name)
{
	SetValue(WXVARIANT(TypeConv::FontToString(value)));

	// Initialize font family choices list
	if (!wxPGGlobalVars->m_fontFamilyChoices)
	{
		wxFontEnumerator enumerator;
		enumerator.EnumerateFacenames();

		wxArrayString faceNames = enumerator.GetFacenames();

		faceNames.Sort();
		faceNames.Insert(wxEmptyString, 0);

		wxPGGlobalVars->m_fontFamilyChoices = new wxPGChoices(faceNames);
	}

	//wxString emptyString(wxEmptyString);

	AddPrivateChild(new wxIntProperty(_("Point Size"), wxT("Point Size"),
		value.m_pointSize));

	AddPrivateChild(new wxEnumProperty(_("Family"), wxT("Family"),
		gs_fp_es_family_labels, gs_fp_es_family_values,
		value.m_family));

	wxString faceName = value.m_faceName;
	// If font was not in there, add it now
	if (faceName.length() &&
		wxPGGlobalVars->m_fontFamilyChoices->Index(faceName) == wxNOT_FOUND)
		wxPGGlobalVars->m_fontFamilyChoices->AddAsSorted(faceName);

	wxPGProperty* p = new wxEnumProperty(_("Face Name"), wxT("Face Name"),
		*wxPGGlobalVars->m_fontFamilyChoices);

	p->SetValueFromString(faceName, wxPG_FULL_VALUE);

	AddPrivateChild(p);

	AddPrivateChild(new wxEnumProperty(_("Style"), wxT("Style"),
		gs_fp_es_style_labels, gs_fp_es_style_values, value.m_style));

	AddPrivateChild(new wxEnumProperty(_("Weight"), wxT("Weight"),
		gs_fp_es_weight_labels, gs_fp_es_weight_values, value.m_weight));

	AddPrivateChild(new wxBoolProperty(_("Underlined"), wxT("Underlined"),
		value.m_underlined));
}

wxPGFontProperty::~wxPGFontProperty() { }

void wxPGFontProperty::OnSetValue()
{
	// do nothing
}

wxString wxPGFontProperty::GetValueAsString(int argFlags) const
{
	return wxPGProperty::GetValueAsString(argFlags);
}

bool wxPGFontProperty::OnEvent(wxPropertyGrid* propgrid, wxWindow* WXUNUSED(primary),
	wxEvent& event)
{
	if (propgrid->IsMainButtonEvent(event))
	{
		// Update value from last minute changes

		wxFontData data;
		wxFont font = TypeConv::StringToFont(m_value.GetString());

		data.SetInitialFont(font);
		data.SetColour(*wxBLACK);

		wxFontDialog dlg(propgrid, data);
		if (dlg.ShowModal() == wxID_OK)
		{
			propgrid->EditorsValueWasModified();

			wxFontContainer fcont(dlg.GetFontData().GetChosenFont());

			wxVariant variant = WXVARIANT(TypeConv::FontToString(fcont));
			SetValueInEvent(variant);

			return true;
		}
	}
	return false;
}

void wxPGFontProperty::RefreshChildren()
{
	wxString fstr = m_value.GetString();
	wxFontContainer font = TypeConv::StringToFont(fstr);

	Item(0)->SetValue(font.m_pointSize);
	Item(1)->SetValue(font.m_family);
	Item(2)->SetValueFromString(font.m_faceName, wxPG_FULL_VALUE);
	Item(3)->SetValue(font.m_style);
	Item(4)->SetValue(font.m_weight);
	Item(5)->SetValue(font.m_underlined);
}

wxVariant wxPGFontProperty::ChildChanged(wxVariant& thisValue, int ind, wxVariant& childValue) const
{
	wxFontContainer font = TypeConv::StringToFont(thisValue.GetString());

	if (ind == 0)
	{
		font.m_pointSize = childValue.GetLong();
	}
	else if (ind == 1)
	{
		int fam = childValue.GetLong();
		if (fam < wxFONTFAMILY_DEFAULT || fam > wxFONTFAMILY_TELETYPE) {
			fam = wxFONTFAMILY_DEFAULT;
		}
		font.m_family = static_cast<wxFontFamily>(fam);
	}
	else if (ind == 2)
	{
		wxString faceName;
		int faceIndex = childValue.GetLong();

		if (faceIndex >= 0)
			faceName = wxPGGlobalVars->m_fontFamilyChoices->GetLabel(faceIndex);

		font.m_faceName = faceName;
	}
	else if (ind == 3)
	{
		int st = childValue.GetLong();
		if (st != wxFONTSTYLE_NORMAL && st != wxFONTSTYLE_SLANT && st != wxFONTSTYLE_ITALIC) {
			st = wxFONTSTYLE_NORMAL;
		}
		font.m_style = static_cast<wxFontStyle>(st);
	}
	else if (ind == 4)
	{
		int wt = childValue.GetLong();
		if (wt != wxFONTWEIGHT_NORMAL &&
			wt != wxFONTWEIGHT_LIGHT &&
			wt != wxFONTWEIGHT_BOLD)
			wt = wxFONTWEIGHT_NORMAL;
		font.m_weight = static_cast<wxFontWeight>(wt);
	}
	else if (ind == 5)
	{
		font.m_underlined = childValue.GetBool();
	}

	thisValue = WXVARIANT(TypeConv::FontToString(font));

	return thisValue;
}

// -----------------------------------------------------------------------
// wxStringControlProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxStringControlProperty, wxStringProperty, TextCtrl)

wxString wxStringControlProperty::ValueToString(wxVariant& value, int argFlags) const
{
	wxString s = value.GetString();

	if (GetChildCount() && HasFlag(wxPG_PROP_COMPOSED_VALUE))
	{
		// Value stored in m_value is non-editable, non-full value
		if ((argFlags & wxPG_FULL_VALUE) ||
			(argFlags & wxPG_EDITABLE_VALUE) ||
			s.empty())
		{
			// Calling this under incorrect conditions will fail
			wxASSERT_MSG(argFlags & wxPG_VALUE_IS_CURRENT,
				wxS("Sorry, currently default wxPGProperty::ValueToString() ")
				wxS("implementation only works if value is m_value."));

			DoGenerateComposedValue(s, argFlags);
		}

		return s;
	}

	// If string is password and value is for visual purposes,
	// then return asterisks instead the actual string.
	if ((m_flags & wxPG_PROP_PASSWORD) && !(argFlags & (wxPG_FULL_VALUE | wxPG_EDITABLE_VALUE)))
		return wxString(wxS('*'), s.Length());

	return s;
}

#include "utils/stringUtils.h"

bool wxStringControlProperty::StringToValue(wxVariant& variant,
	const wxString& text,
	int argFlags) const
{
	if (StringUtils::CheckCorrectName(text) >= 0)
		return false;

	if (GetChildCount() && HasFlag(wxPG_PROP_COMPOSED_VALUE))
		return wxPGProperty::StringToValue(variant, text, argFlags);

	if (variant != text) {
		variant = text;
		return text.length() > 0;
	}

	return false;
}

// -----------------------------------------------------------------------
// wxEventControlProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxEventControlProperty, wxStringProperty, TextCtrlAndButton)

wxString wxEventControlProperty::ValueToString(wxVariant& value, int argFlags) const
{
	wxString s = value.GetString();

	if (GetChildCount() && HasFlag(wxPG_PROP_COMPOSED_VALUE))
	{
		// Value stored in m_value is non-editable, non-full value
		if ((argFlags & wxPG_FULL_VALUE) ||
			(argFlags & wxPG_EDITABLE_VALUE) ||
			s.empty())
		{
			// Calling this under incorrect conditions will fail
			wxASSERT_MSG(argFlags & wxPG_VALUE_IS_CURRENT,
				wxS("Sorry, currently default wxPGProperty::ValueToString() ")
				wxS("implementation only works if value is m_value."));

			DoGenerateComposedValue(s, argFlags);
		}

		return s;
	}

	// If string is password and value is for visual purposes,
	// then return asterisks instead the actual string.
	if ((m_flags & wxPG_PROP_PASSWORD) && !(argFlags & (wxPG_FULL_VALUE | wxPG_EDITABLE_VALUE)))
		return wxString(wxS('*'), s.Length());

	return s;
}

bool wxEventControlProperty::StringToValue(wxVariant& variant,
	const wxString& text,
	int argFlags) const
{
	if (StringUtils::CheckCorrectName(text) > 0)
		return false;

	if (GetChildCount() && HasFlag(wxPG_PROP_COMPOSED_VALUE))
		return wxPGProperty::StringToValue(variant, text, argFlags);

	if (variant != text)
	{
		variant = text;
		return true;
	}

	return false;
}

wxPGEditorDialogAdapter* wxEventControlProperty::GetEditorDialog() const
{
	class wxPGEventAdapter : public wxPGEditorDialogAdapter {
	public:
		wxPGEventAdapter() : wxPGEditorDialogAdapter()
		{
		}

		virtual ~wxPGEventAdapter()
		{
		}

		virtual bool DoShowDialog(wxPropertyGrid* pg, wxPGProperty* prop) wxOVERRIDE
		{
			wxEventControlProperty* dlgProp = wxDynamicCast(prop, wxEventControlProperty);
			wxCHECK_MSG(dlgProp, false, "Function called for incompatible property");
			wxString eventName = pg->GetUncommittedPropertyValue();
			if (eventName.IsEmpty()) {
				wxPGProperty* pgProp = pg->GetPropertyByLabel(wxT("name"));

				prop->SetValueFromString(
					(pgProp ? pgProp->GetDisplayedString() : wxEmptyString) + wxT("_") + prop->GetLabel());

				SetValue(prop->GetValue());
			}
			else {
				SetValue(eventName);
			}

			return true;
		}
	};

	return new wxPGEventAdapter();
}

// -----------------------------------------------------------------------
// wxPGToolActionProperty
// -----------------------------------------------------------------------

#include "metadata/metadata.h"
#include "frontend/visualView/controls/form.h"

#include "editors.h"

wxPG_IMPLEMENT_PROPERTY_CLASS(wxPGToolActionProperty, wxPGProperty, ComboBoxAndButton)

wxPGToolActionProperty::wxPGToolActionProperty(const wxString& label, const wxString& name, wxPGChoices& choices,
	const wxString& value, IObjectBase* curObject) : wxPGProperty(label, name),
	m_curObject(curObject)
{
	wxVariant selValue = value; long dataSource = wxNOT_FOUND;
	SetChoices(choices);
	if (selValue.Convert(&dataSource)) {
		for (unsigned int i = 0; i < m_choices.GetCount(); i++) {
			int val = m_choices.GetValue(i);
			if (dataSource == val) {
				m_flags &= ~(wxPG_PROP_ACTIVE_BTN);
				SetValue(m_choices.GetLabel(i));
				m_toolData.SetNumber(val);
				return;
			}
		}
		m_toolData.SetString(value);
		m_flags |= wxPG_PROP_ACTIVE_BTN; // Property button always enabled.
		SetValue(value);
	}
	else {
		m_toolData.SetString(value);
		m_flags |= wxPG_PROP_ACTIVE_BTN; // Property button always enabled.
		SetValue(value);
	}
}

wxPGToolActionProperty::~wxPGToolActionProperty()
{
}

wxString wxPGToolActionProperty::ValueToString(wxVariant& value,
	int WXUNUSED(argFlags)) const
{
	return value;
}

bool wxPGToolActionProperty::StringToValue(wxVariant& variant, const wxString& text, int argFlags) const
{
	if (StringUtils::CheckCorrectName(text) > 0)
		return false;

	if (GetChildCount() && HasFlag(wxPG_PROP_COMPOSED_VALUE))
		return wxPGProperty::StringToValue(variant, text, argFlags);
	m_toolData.SetString(text);
	if (variant != text) {
		variant = text;
		return true;
	}

	return false;
}

bool wxPGToolActionProperty::IntToValue(wxVariant& value, int number, int argFlags) const
{
	value = m_choices.GetLabel(number);
	m_toolData.SetNumber(
		m_choices.GetValue(number)
	);
	return true;
}

void wxPGToolActionProperty::OnSetValue()
{
	if (m_toolData.IsCustomEvent()) {
		SetFlag(wxPG_PROP_ACTIVE_BTN); // Property button always enabled.
	}
	else {
		ClearFlag(wxPG_PROP_ACTIVE_BTN); // Property button always disabled.
	}
}

wxPGEditorDialogAdapter* wxPGToolActionProperty::GetEditorDialog() const
{
	class wxPGEventAdapter : public wxPGEditorDialogAdapter {
	public:
		wxPGEventAdapter() : wxPGEditorDialogAdapter()
		{
		}

		virtual ~wxPGEventAdapter()
		{
		}

		virtual bool DoShowDialog(wxPropertyGrid* pg, wxPGProperty* prop) wxOVERRIDE
		{
			wxPGToolActionProperty* dlgProp = wxDynamicCast(prop, wxPGToolActionProperty);
			wxCHECK_MSG(dlgProp, false, "Function called for incompatible property");
			wxString eventName = pg->GetUncommittedPropertyValue();
			if (eventName.IsEmpty()) {
				wxPGProperty* pgProp = pg->GetPropertyByLabel(wxT("name"));

				prop->SetValueFromString(
					(pgProp ? pgProp->GetDisplayedString() : wxEmptyString) + wxT("_") + prop->GetLabel());

				SetValue(prop->GetValue());
			}
			else {
				SetValue(eventName);
			}

			return true;
		}
	};

	return new wxPGEventAdapter();
}

#define ICON_SIZE 16

// -----------------------------------------------------------------------
// wxPGTypeSelectorProperty
// -----------------------------------------------------------------------

#include "metadata/singleMetaTypes.h"

wxPG_IMPLEMENT_PROPERTY_CLASS(wxPGTypeSelectorProperty, wxStringProperty, ComboBoxAndButton)

wxPGChoices wxPGTypeSelectorProperty::GetDateTime()
{
	wxPGChoices choices;
	choices.Add(_("date"), eDateFractions::eDateFractions_Date);
	choices.Add(_("date and time"), eDateFractions::eDateFractions_DateTime);
	choices.Add(_("time"), eDateFractions::eDateFractions_Time);
	return choices;
}

void wxPGTypeSelectorProperty::FillByClsid(const CLASS_ID& clsid)
{
	IObjectValueAbstract* so = CValue::GetAvailableObject(clsid);
	wxASSERT(so);

	if (so->GetObjectType() == eObjectType::eObjectType_metadata) {
		IMetadata* metaData = m_curObject->GetMetaData();
		wxASSERT(metaData);

		if (m_selectorDataType == eSelectorDataType::eSelectorDataType_reference) {
			for (auto so : metaData->GetAvailableSingleObjects(clsid, eMetaObjectType::enReference)) {
				auto metaObject = so->GetMetaObject();
				auto choice = m_choices.Add(so->GetClassName(), metaObject->GetIcon());
				m_valChoices.insert_or_assign(
					choice.GetValue(), so->GetClassType()
				);
			}
		}
		else if (m_selectorDataType == eSelectorDataType::eSelectorDataType_table) {

			for (auto so : metaData->GetAvailableSingleObjects(clsid, eMetaObjectType::enList)) {
				auto metaObject = so->GetMetaObject();
				auto choice = m_choices.Add(so->GetClassName(), metaObject->GetIcon());
				m_valChoices.insert_or_assign(
					choice.GetValue(), so->GetClassType()
				);
			}
		}
		else if (m_selectorDataType == eSelectorDataType::eSelectorDataType_any) {

			for (auto so : metaData->GetAvailableSingleObjects(clsid, eMetaObjectType::enObject)) {
				auto metaObject = so->GetMetaObject();
				auto choice = m_choices.Add(so->GetClassName(), metaObject->GetIcon());
				m_valChoices.insert_or_assign(
					choice.GetValue(), so->GetClassType()
				);
			}

			for (auto so : metaData->GetAvailableSingleObjects(clsid, eMetaObjectType::enReference)) {
				auto metaObject = so->GetMetaObject();
				auto choice = m_choices.Add(so->GetClassName(), metaObject->GetIcon());
				m_valChoices.insert_or_assign(
					choice.GetValue(), so->GetClassType()
				);
			}

			for (auto so : metaData->GetAvailableSingleObjects(clsid, eMetaObjectType::enRecordManager)) {
				auto metaObject = so->GetMetaObject();
				auto choice = m_choices.Add(so->GetClassName(), metaObject->GetIcon());
				m_valChoices.insert_or_assign(
					choice.GetValue(), so->GetClassType()
				);
			}
		}
	}
	else {
		auto choice = m_choices.Add(so->GetClassName(), so->GetClassIcon());
		m_valChoices.insert_or_assign(
			choice.GetValue(), so->GetClassType()
		);
	}
}

#include "compiler/valueTable.h"

wxPGTypeSelectorProperty::wxPGTypeSelectorProperty(const wxString& label, const wxString& name, const wxVariant& value,
	eSelectorDataType dataType,
	IObjectBase* curObject) :
	wxStringProperty(label, name), m_selectorDataType(dataType),
	m_curObject(curObject)
{
	m_precision = new wxUIntProperty(wxT("precision"), wxPG_LABEL, 0);
	AddPrivateChild(m_precision);
	m_scale = new wxUIntProperty(wxT("scale"), wxPG_LABEL, 0);
	AddPrivateChild(m_scale);
	m_date_time = new wxEnumProperty(wxT("date_time"), wxPG_LABEL, GetDateTime(), eDateFractions::eDateFractions_Date);
	AddPrivateChild(m_date_time);
	m_length = new wxUIntProperty(wxT("length"), wxPG_LABEL, 0);
	AddPrivateChild(m_length);

	if (m_selectorDataType == eSelectorDataType::eSelectorDataType_any) {
		FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_EMPTY));
	}

	if (m_selectorDataType == eSelectorDataType::eSelectorDataType_any
		|| m_selectorDataType == eSelectorDataType::eSelectorDataType_reference) {
		FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_BOOLEAN));
		FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_NUMBER));
		FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_DATE));
		FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_STRING));
	}
	else if (m_selectorDataType == eSelectorDataType::eSelectorDataType_resource) {
		FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_NUMBER));
	}

	if (m_selectorDataType == eSelectorDataType::eSelectorDataType_any) {
		FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_NULL));
	}

	/////////////////////////////////////////////////

	if (m_selectorDataType == eSelectorDataType::eSelectorDataType_table) {
		FillByClsid(g_valueTableCLSID);
	}

	/////////////////////////////////////////////////

	FillByClsid(g_metaCatalogCLSID);
	FillByClsid(g_metaDocumentCLSID);
	FillByClsid(g_metaEnumerationCLSID);

	if (m_selectorDataType == eSelectorDataType::eSelectorDataType_any) {
		FillByClsid(g_metaDataProcessorCLSID);
		FillByClsid(g_metaReportCLSID);
	}

	if (m_selectorDataType == eSelectorDataType::eSelectorDataType_table) {
		FillByClsid(g_metaInformationRegisterCLSID);
		FillByClsid(g_metaAccumulationRegisterCLSID);
	}

	SetValue(value);

	//m_flags |= wxPG_PROP_READONLY;
	m_flags |= wxPG_PROP_ACTIVE_BTN;
}

wxString wxPGTypeSelectorProperty::ValueToString(wxVariant& value, int argFlags) const
{
	return value.GetString();
}

bool wxPGTypeSelectorProperty::StringToValue(wxVariant& variant,
	const wxString& text,
	int argFlags) const
{
	return false;
}

bool wxPGTypeSelectorProperty::IntToValue(wxVariant& value, int number, int argFlags) const
{
	IMetadata* metaData = m_curObject->GetMetaData();
	wxASSERT(metaData);
	wxVariantAttributeData* list =
		new wxVariantAttributeData(metaData);
	list->AppendRecord(
		m_valChoices.at(number)
	);
	value = list;
	return true;
}

wxVariant wxPGTypeSelectorProperty::ChildChanged(wxVariant& thisValue, int childIndex, wxVariant& childValue) const
{
	wxVariantAttributeData* currList =
		dynamic_cast<wxVariantAttributeData*>(thisValue.GetData());

	if (currList != NULL) {

		wxVariantAttributeData* list =
			new wxVariantAttributeData(*currList);

		if (childIndex == 0 || childIndex == 1) {
			long precision = (childIndex == 0)
				? childValue : m_precision->GetValue(),
				scale = (childIndex == 1)
				? childValue : m_scale->GetValue();

			if (precision > MAX_PRECISION_NUMBER) {
				precision = m_precision->GetValue();
				scale = m_scale->GetValue();
			}
			else if (precision == 0 || precision < scale) {
				precision = m_precision->GetValue();
				scale = m_scale->GetValue();
			}

			list->SetNumber(precision, scale);
		}
		else if (childIndex == 2) {
			long dateTime = childValue;
			list->SetDate((eDateFractions)dateTime);
		}
		else if (childIndex == 3) {
			long length = childValue;
			if (length > MAX_LENGTH_STRING) {
				length = m_length->GetValue();
			}
			list->SetString(length);
		}

		return list;
	}

	return wxVariant();
}

void wxPGTypeSelectorProperty::RefreshChildren()
{
	wxVariantAttributeData* currList =
		dynamic_cast<wxVariantAttributeData*>(m_value.GetData());

	if (currList && currList->GetTypeCount() < 2) {
		eValueTypes id = CValue::GetVTByID(currList->GetById(0));
		if (id == eValueTypes::TYPE_NUMBER) {
			m_precision->Hide(false);
			m_scale->Hide(false);
			m_date_time->Hide(true);
			m_length->Hide(true);
		}
		else if (id == eValueTypes::TYPE_DATE) {
			m_precision->Hide(true);
			m_scale->Hide(true);
			m_date_time->Hide(false);
			m_length->Hide(true);
		}
		else if (id == eValueTypes::TYPE_STRING) {
			m_precision->Hide(true);
			m_scale->Hide(true);
			m_date_time->Hide(true);
			m_length->Hide(false);
		}
		else {
			m_precision->Hide(true);
			m_scale->Hide(true);
			m_date_time->Hide(true);
			m_length->Hide(true);
		}
	}
	else {
		m_precision->Hide(true);
		m_scale->Hide(true);
		m_date_time->Hide(true);
		m_length->Hide(true);
	}

	if (currList != NULL) {
		for (unsigned int idx = 0; idx < currList->GetTypeCount(); idx++) {
			eValueTypes id = CValue::GetVTByID(currList->GetById(idx));
			if (id == eValueTypes::TYPE_NUMBER) {
				m_precision->SetValue(currList->GetPrecision());
				m_scale->SetValue(currList->GetScale());
			}
			else if (id == eValueTypes::TYPE_DATE) {
				m_date_time->SetValue(currList->GetDateTime());
			}
			else if (id == eValueTypes::TYPE_STRING) {
				m_length->SetValue(currList->GetLength());
			}
		}
	}
}

#include "frontend/controls/checktree.h"
#include "utils/stringUtils.h"

wxPGEditorDialogAdapter* wxPGTypeSelectorProperty::GetEditorDialog() const
{
	class wxPGEventAdapter : public wxPGEditorDialogAdapter {

		class optionTreeItemData : public wxTreeItemData {
			IObjectValueAbstract* m_currOpt;
		public:
			optionTreeItemData(IObjectValueAbstract* so)
				: wxTreeItemData(), m_currOpt(so)
			{
			};
			IObjectValueAbstract* GetOption() const {
				return m_currOpt;
			}
		};

		void FillByClsid(const CLASS_ID& clsid,
			wxCheckTree* tc, wxVariantAttributeData* oldList) {

			wxImageList* imageList = tc->GetImageList();
			wxASSERT(imageList);
			IObjectValueAbstract* so = CValue::GetAvailableObject(clsid);
			int groupIcon = imageList->Add(so->GetClassIcon());

			optionTreeItemData* itemData = new optionTreeItemData(so);
			wxTreeItemId newItem = tc->AppendItem(tc->GetRootItem(), so->GetClassName(),
				groupIcon, groupIcon,
				itemData);

			if (oldList != NULL) {
				tc->SetItemState(newItem, oldList->Contains(so->GetClassType()) ? wxCheckTree::CHECKED : wxCheckTree::UNCHECKED);
				tc->Check(newItem, oldList->Contains(so->GetClassType()));
			}
			else {
				tc->SetItemState(newItem, wxCheckTree::UNCHECKED);
				tc->Check(newItem, false);
			}
		}

		void FillByClsid(IMetadata* metaData, const CLASS_ID& clsid,
			wxCheckTree* tc, wxVariantAttributeData* oldList) {

			wxImageList* imageList = tc->GetImageList();
			wxASSERT(imageList);
			IObjectValueAbstract* so = metaData ? metaData->GetAvailableObject(clsid) : CValue::GetAvailableObject(clsid);
			int groupIcon = imageList->Add(so->GetClassIcon());

			optionTreeItemData* itemData = new optionTreeItemData(so);
			wxTreeItemId newItem = tc->AppendItem(tc->GetRootItem(), so->GetClassName(),
				groupIcon, groupIcon,
				itemData);

			if (oldList != NULL) {
				tc->SetItemState(newItem, oldList->Contains(so->GetClassType()) ? wxCheckTree::CHECKED : wxCheckTree::UNCHECKED);
				tc->Check(newItem, oldList->Contains(so->GetClassType()));
			}
			else {
				tc->SetItemState(newItem, wxCheckTree::UNCHECKED);
				tc->Check(newItem, false);
			}
		}

		void FillByClsid(eSelectorDataType selectorDataType, IMetadata* metaData, const CLASS_ID& clsid,
			wxCheckTree* tc, wxVariantAttributeData* oldList) {

			wxImageList* imageList = tc->GetImageList();
			wxASSERT(imageList);
			IObjectValueAbstract* so = CValue::GetAvailableObject(clsid);

			if (selectorDataType == eSelectorDataType::eSelectorDataType_reference) {

				int groupIcon = imageList->Add(so->GetClassIcon());
				const wxTreeItemId& parentID = tc->AppendItem(tc->GetRootItem(), so->GetClassName() + wxT("Ref"),
					groupIcon, groupIcon);

				for (auto so : metaData->GetAvailableSingleObjects(clsid, eMetaObjectType::enReference)) {
					IMetaObjectRecordDataRef* registerData = dynamic_cast<IMetaObjectRecordDataRef*>(so->GetMetaObject());
					{
						int icon = imageList->Add(registerData->GetIcon());
						optionTreeItemData* itemData = new optionTreeItemData(so);
						wxTreeItemId newItem = tc->AppendItem(parentID, registerData->GetSynonym(),
							icon, icon,
							itemData);

						if (oldList != NULL) {
							tc->SetItemState(newItem, oldList->Contains(so->GetClassType()) ? wxCheckTree::CHECKED : wxCheckTree::UNCHECKED);
							tc->Check(newItem, oldList->Contains(so->GetClassType()));
						}
						else {
							tc->SetItemState(newItem, wxCheckTree::UNCHECKED);
							tc->Check(newItem, false);
						}
					}
				}
			}
			else if (selectorDataType == eSelectorDataType::eSelectorDataType_table) {

				int groupIcon = imageList->Add(so->GetClassIcon());
				const wxTreeItemId& parentID = tc->AppendItem(tc->GetRootItem(), so->GetClassName() + wxT("List"),
					groupIcon, groupIcon);

				for (auto so : metaData->GetAvailableSingleObjects(clsid, eMetaObjectType::enList)) {
					IMetaObjectWrapperData* registerData = dynamic_cast<IMetaObjectWrapperData*>(so->GetMetaObject());
					{
						int icon = imageList->Add(registerData->GetIcon());
						optionTreeItemData* itemData = new optionTreeItemData(so);
						wxTreeItemId newItem = tc->AppendItem(parentID, registerData->GetSynonym(),
							icon, icon,
							itemData);

						if (oldList != NULL) {
							tc->SetItemState(newItem, oldList->Contains(so->GetClassType()) ? wxCheckTree::CHECKED : wxCheckTree::UNCHECKED);
							tc->Check(newItem, oldList->Contains(so->GetClassType()));
						}
						else {
							tc->SetItemState(newItem, wxCheckTree::UNCHECKED);
							tc->Check(newItem, false);
						}
					}
				}
			}
			else if (selectorDataType == eSelectorDataType::eSelectorDataType_any) {
				{
					int groupIcon = imageList->Add(so->GetClassIcon());
					const wxTreeItemId& parentID = tc->AppendItem(tc->GetRootItem(), so->GetClassName() + wxT("Object"),
						groupIcon, groupIcon);

					for (auto so : metaData->GetAvailableSingleObjects(clsid, eMetaObjectType::enObject)) {
						IMetaObjectRecordData* registerData = dynamic_cast<IMetaObjectRecordData*>(so->GetMetaObject());
						{
							int icon = imageList->Add(registerData->GetIcon());
							optionTreeItemData* itemData = new optionTreeItemData(so);
							wxTreeItemId newItem = tc->AppendItem(parentID, registerData->GetSynonym(),
								icon, icon,
								itemData);

							if (oldList != NULL) {
								tc->SetItemState(newItem, oldList->Contains(so->GetClassType()) ? wxCheckTree::CHECKED : wxCheckTree::UNCHECKED);
								tc->Check(newItem, oldList->Contains(so->GetClassType()));
							}
							else {
								tc->SetItemState(newItem, wxCheckTree::UNCHECKED);
								tc->Check(newItem, false);
							}
						}
					}
				}
				{
					int groupIcon = imageList->Add(so->GetClassIcon());
					const wxTreeItemId& parentID = tc->AppendItem(tc->GetRootItem(), so->GetClassName() + wxT("Ref"),
						groupIcon, groupIcon);

					for (auto so : metaData->GetAvailableSingleObjects(clsid, eMetaObjectType::enReference)) {
						IMetaObjectRecordDataRef* registerData = dynamic_cast<IMetaObjectRecordDataRef*>(so->GetMetaObject());
						{
							int icon = imageList->Add(registerData->GetIcon());
							optionTreeItemData* itemData = new optionTreeItemData(so);
							wxTreeItemId newItem = tc->AppendItem(parentID, registerData->GetSynonym(),
								icon, icon,
								itemData);

							if (oldList != NULL) {
								tc->SetItemState(newItem, oldList->Contains(so->GetClassType()) ? wxCheckTree::CHECKED : wxCheckTree::UNCHECKED);
								tc->Check(newItem, oldList->Contains(so->GetClassType()));
							}
							else {
								tc->SetItemState(newItem, wxCheckTree::UNCHECKED);
								tc->Check(newItem, false);
							}
						}
					}
				}
			}
		}

	public:

		wxPGEventAdapter() : wxPGEditorDialogAdapter() {}
		virtual ~wxPGEventAdapter() {}

		virtual bool DoShowDialog(wxPropertyGrid* pg, wxPGProperty* prop) wxOVERRIDE
		{
			wxPGTypeSelectorProperty* dlgProp = wxDynamicCast(prop, wxPGTypeSelectorProperty);
			wxCHECK_MSG(dlgProp, false, "Function called for incompatible property");

			IObjectBase* curObject = dlgProp->GetObject();

			// launch editor dialog
			wxDialog* dlg = new wxDialog(pg, wxID_ANY, _("Choice type"), wxDefaultPosition, wxDefaultSize,
				wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCLIP_CHILDREN);

			dlg->SetFont(pg->GetFont()); // To allow entering chars of the same set as the propGrid

			// Multi-line text editor dialog.
			const int spacing = wxPropertyGrid::IsSmallScreen() ? 4 : 8;
			wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
			wxBoxSizer* rowsizer = new wxBoxSizer(wxHORIZONTAL);

			eSelectorDataType selectorDataType = dlgProp->GetSelectorDataType();

			wxVariant value = dlgProp->GetValue();
			wxVariantAttributeData* oldList =
				dynamic_cast<wxVariantAttributeData*>(value.GetData());

			wxCheckBox* compositeDataType = new wxCheckBox(dlg, wxID_ANY,
				_("Composite data type"), wxDefaultPosition, wxDefaultSize);

			compositeDataType->Enable(!dlgProp->HasFlag(wxPG_PROP_READONLY));

			int style = wxCR_SINGLE_CHECK;

			if (selectorDataType == eSelectorDataType::eSelectorDataType_any ||
				selectorDataType == eSelectorDataType::eSelectorDataType_reference) {
				style = wxCR_MULTIPLE_CHECK;
			}

			if (oldList != NULL) {
				style = oldList->GetTypeCount() > 1 ?
					wxCR_MULTIPLE_CHECK : wxCR_SINGLE_CHECK;
				compositeDataType->SetValue(
					oldList->GetTypeCount() > 1);
			}
			else {
				compositeDataType->SetValue(style != wxCR_SINGLE_CHECK);
			}

			compositeDataType->Show(selectorDataType != eSelectorDataType::eSelectorDataType_table);

			wxCheckTree* tc = new wxCheckTree(dlg, wxID_ANY,
				wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_NO_LINES | wxTR_HIDE_ROOT | style | wxSUNKEN_BORDER);

			topsizer->Add(compositeDataType, wxSizerFlags(0).Border(wxALL, spacing));
			rowsizer->Add(tc, wxSizerFlags(1).Expand().Border(wxALL, spacing));
			topsizer->Add(rowsizer, wxSizerFlags(1).Expand());

			wxBoxSizer* stringSizer = new wxBoxSizer(wxHORIZONTAL);
			wxStaticText* stSLength = new wxStaticText(dlg, wxID_ANY, _("Length:"), wxDefaultPosition, wxDefaultSize);
			stSLength->Wrap(-1);
			stringSizer->Add(stSLength, 0, wxALL, 5);

			wxSpinCtrl* tcSLength = new wxSpinCtrl(dlg, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, MAX_LENGTH_STRING);
			stringSizer->Add(tcSLength, 0, wxBOTTOM | wxRIGHT, 0);
			tcSLength->SetValue(oldList->GetLength());

			tcSLength->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED,
				[oldList](wxSpinEvent& event) {
					oldList->SetString(event.GetValue());
					event.Skip();
				}
			);

			topsizer->Add(stringSizer, 0, 0, 5);

			wxBoxSizer* dateSizer = new wxBoxSizer(wxHORIZONTAL);
			wxStaticText* stDDateFormat = new wxStaticText(dlg, wxID_ANY, _("Date format:"), wxDefaultPosition, wxDefaultSize);
			stDDateFormat->Wrap(-1);
			dateSizer->Add(stDDateFormat, 0, wxALL, 5);

			wxArrayString cDDateFormatChoices; auto ch = dlgProp->GetDateTime();
			for (unsigned int idx = 0; idx < ch.GetCount(); idx++) {
				cDDateFormatChoices.Add(ch.GetLabel(idx));
			}
			wxChoice* cDDateFormat = new wxChoice(dlg, wxID_ANY, wxDefaultPosition, wxDefaultSize, cDDateFormatChoices);
			cDDateFormat->SetSelection(oldList->GetDateTime());
			dateSizer->Add(cDDateFormat, 0, wxALL, 0);

			cDDateFormat->Bind(wxEVT_COMMAND_CHOICE_SELECTED,
				[oldList](wxCommandEvent& event) {
					oldList->SetDate((eDateFractions)event.GetSelection());
					event.Skip();
				}
			);

			topsizer->Add(dateSizer, 0, 0, 5);

			wxBoxSizer* numberSizer = new wxBoxSizer(wxHORIZONTAL);
			wxStaticText* stNLength = new wxStaticText(dlg, wxID_ANY, _("Length:"), wxDefaultPosition, wxDefaultSize);
			stNLength->Wrap(-1);
			numberSizer->Add(stNLength, 0, wxALL, 5);

			wxSpinCtrl* tcNLength = new wxSpinCtrl(dlg, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 0, MAX_PRECISION_NUMBER);
			numberSizer->Add(tcNLength, 0, wxBOTTOM | wxRIGHT, 0);
			tcNLength->SetValue(oldList->GetPrecision());

			tcNLength->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED,
				[oldList](wxSpinEvent& event) {
					oldList->SetNumber(event.GetValue(), oldList->GetScale());
					event.Skip();
				}
			);

			wxStaticText* stNScale = new wxStaticText(dlg, wxID_ANY, _("Scale:"), wxDefaultPosition, wxDefaultSize);
			stNScale->Wrap(-1);
			numberSizer->Add(stNScale, 0, wxTOP | wxBOTTOM | wxLEFT, 5);

			wxSpinCtrl* tcNScale = new wxSpinCtrl(dlg, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS);
			numberSizer->Add(tcNScale, 0, wxRIGHT | wxLEFT, 5);
			tcNScale->SetValue(oldList->GetScale());

			tcNScale->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED,
				[oldList](wxSpinEvent& event) {
					unsigned short scale = oldList->GetPrecision() > event.GetValue() ?
						event.GetValue() :
						oldList->GetPrecision();

					oldList->SetNumber(oldList->GetPrecision(), scale);
					event.Skip();
				}
			);

			topsizer->Add(numberSizer, 0, 0, 5);

			tc->SetDoubleBuffered(true);
			tc->Enable(!dlgProp->HasFlag(wxPG_PROP_READONLY));

			wxStdDialogButtonSizer* buttonSizer = dlg->CreateStdDialogButtonSizer(wxOK | wxCANCEL);
			topsizer->Add(buttonSizer, wxSizerFlags(0).Right().Border(wxBOTTOM | wxRIGHT, spacing));

			compositeDataType->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED,
				[tc](wxCommandEvent& event) {
					tc->SetWindowStyle(
						event.IsChecked() ?
						wxCR_MULTIPLE_CHECK :
						wxCR_SINGLE_CHECK
					);
					event.Skip();
				}
			);


			for (unsigned int i = 0; i < numberSizer->GetItemCount(); i++) {
				numberSizer->Hide(i);
			}

			for (unsigned int i = 0; i < dateSizer->GetItemCount(); i++) {
				dateSizer->Hide(i);
			}

			for (unsigned int i = 0; i < stringSizer->GetItemCount(); i++) {
				stringSizer->Hide(i);
			}

			tc->Bind(wxEVT_COMMAND_TREE_SEL_CHANGED,
				[tc, numberSizer, dateSizer, stringSizer, topsizer](wxTreeEvent& event) {
					optionTreeItemData* dataItem = dynamic_cast<optionTreeItemData*>(tc->GetItemData(event.GetItem()));
					if (dataItem != NULL) {
						IObjectValueAbstract* so = dataItem->GetOption();
						wxASSERT(so);
						CLASS_ID clsid = so->GetClassType();

						for (unsigned int i = 0; i < numberSizer->GetItemCount(); i++) {
							numberSizer->Show(i, CValue::GetVTByID(clsid) == eValueTypes::TYPE_NUMBER);
						}

						for (unsigned int i = 0; i < dateSizer->GetItemCount(); i++) {
							dateSizer->Show(i, CValue::GetVTByID(clsid) == eValueTypes::TYPE_DATE);
						}

						for (unsigned int i = 0; i < stringSizer->GetItemCount(); i++) {
							stringSizer->Show(i, CValue::GetVTByID(clsid) == eValueTypes::TYPE_STRING);
						}

						topsizer->Layout();
						tc->Layout();
					}
					event.Skip();
				}
			);

			wxTreeItemId rootItem = tc->AddRoot(wxEmptyString);

			dlg->SetSizer(topsizer);
			topsizer->SetSizeHints(dlg);

			if (!wxPropertyGrid::IsSmallScreen()) {
				dlg->SetSize(400, 300);
				dlg->Move(pg->GetGoodEditorDialogPosition(prop, dlg->GetSize()));
			}

			tc->SetFocus();

			// Make an state image list containing small icons
			tc->SetImageList(
				new wxImageList(ICON_SIZE, ICON_SIZE)
			);

			if (selectorDataType == eSelectorDataType::eSelectorDataType_any) {
				FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_EMPTY), tc, oldList);
			}

			if (selectorDataType == eSelectorDataType::eSelectorDataType_any
				|| selectorDataType == eSelectorDataType::eSelectorDataType_reference) {
				FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_BOOLEAN), tc, oldList);
				FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_NUMBER), tc, oldList);
				FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_DATE), tc, oldList);
				FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_STRING), tc, oldList);
			}
			else if (selectorDataType == eSelectorDataType::eSelectorDataType_resource) {
				FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_NUMBER), tc, oldList);
			}

			if (selectorDataType == eSelectorDataType::eSelectorDataType_any) {
				FillByClsid(CValue::GetIDByVT(eValueTypes::TYPE_NULL), tc, oldList);
			}

			/////////////////////////////////////////////////
			if (selectorDataType == eSelectorDataType::eSelectorDataType_table) {
				FillByClsid(g_valueTableCLSID, tc, oldList);
			}
			/////////////////////////////////////////////////

			IMetadata* metaData = curObject->GetMetaData();
			wxASSERT(metaData);
			if (metaData != NULL) {
				FillByClsid(selectorDataType, metaData, g_metaCatalogCLSID, tc, oldList);
				FillByClsid(selectorDataType, metaData, g_metaDocumentCLSID, tc, oldList);
				FillByClsid(selectorDataType, metaData, g_metaEnumerationCLSID, tc, oldList);
				if (selectorDataType == eSelectorDataType::eSelectorDataType_any) {
					FillByClsid(selectorDataType, metaData, g_metaDataProcessorCLSID, tc, oldList);
					FillByClsid(selectorDataType, metaData, g_metaReportCLSID, tc, oldList);
				}
				if (selectorDataType == eSelectorDataType::eSelectorDataType_table) {
					FillByClsid(selectorDataType, metaData, g_metaInformationRegisterCLSID, tc, oldList);
					FillByClsid(selectorDataType, metaData, g_metaAccumulationRegisterCLSID, tc, oldList);
				}
			}

			wxArrayTreeItemIds ids;
			unsigned int itemCount = tc->GetItems(ids);

			for (auto clsid : oldList->GetClsids()) {
				bool allowType = true;
				for (const wxTreeItemId& selItem : ids) {
					if (selItem.IsOk()) {
						wxTreeItemData* dataItem = tc->GetItemData(selItem);
						IObjectValueAbstract* item = dataItem ?
							dynamic_cast<optionTreeItemData*>(dataItem)->GetOption() : NULL;
						if (dataItem != NULL) {
							if (clsid == item->GetClassType()) {
								allowType = false; break;
							}
						}
					}
				}

				if (allowType) {
					FillByClsid(metaData, clsid, tc, oldList);
				}
			}

			tc->ExpandAll(); int res = dlg->ShowModal();
			wxVariantAttributeData* list = new wxVariantAttributeData(metaData);
			wxString result, description;
			unsigned int selCount = tc->GetSelections(ids);
			for (const wxTreeItemId& selItem : ids) {
				if (selItem.IsOk()) {
					wxTreeItemData* dataItem = tc->GetItemData(selItem);
					if (dataItem
						&& res == wxID_OK) {
						IObjectValueAbstract* item = dynamic_cast<
							optionTreeItemData*>(dataItem)->GetOption();
						list->AppendRecord(item->GetClassType());
					}
				}
			}

			if (oldList != NULL) {
				list->LoadDescription(oldList->GetDescription());
			}

			SetValue(list);

			dlg->Destroy();
			return res == wxID_OK;
		}
	};

	return new wxPGEventAdapter();
}

// -----------------------------------------------------------------------
// wxPGRecordSelectorProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxPGRecordSelectorProperty, wxStringProperty, ComboBoxAndButton)

#include "metadata/metaObjects/objects/document.h"

void wxPGRecordSelectorProperty::FillByClsid(const CLASS_ID& clsid)
{
	IMetadata* metaData = m_curObject->GetMetaData();
	wxASSERT(metaData);
	for (auto metaObject : metaData->GetMetaObjects(clsid)) {
		IMetaObjectRegisterData* registerData = dynamic_cast<IMetaObjectRegisterData*>(metaObject);
		if (registerData && registerData->HasRecorder()) {
			m_choices.Add(registerData->GetSynonym(), registerData->GetIcon(), registerData->GetMetaID());
		}
	}
}

wxPGRecordSelectorProperty::wxPGRecordSelectorProperty(const wxString& label, const wxString& name, const wxVariant& value, IObjectBase* curObject)
	: wxStringProperty(label, name), m_curObject(curObject)
{
	FillByClsid(g_metaInformationRegisterCLSID);
	FillByClsid(g_metaAccumulationRegisterCLSID);

	//m_flags |= wxPG_PROP_READONLY;
	m_flags |= wxPG_PROP_ACTIVE_BTN; // Property button always enabled.

	SetValue(value);
}

wxString wxPGRecordSelectorProperty::ValueToString(wxVariant& value, int argFlags) const
{
	return value.GetString();
}

bool wxPGRecordSelectorProperty::StringToValue(wxVariant& variant,
	const wxString& text,
	int argFlags) const
{
	return false;
}

bool wxPGRecordSelectorProperty::IntToValue(wxVariant& value, int number, int argFlags) const
{
	IMetadata* metaData = m_curObject->GetMetaData();
	wxASSERT(metaData);
	wxVariantRecordData* list =
		new wxVariantRecordData(metaData);
	list->AppendRecord(
		m_choices.GetValue(number)
	);
	value = list;
	return true;
}

wxPGEditorDialogAdapter* wxPGRecordSelectorProperty::GetEditorDialog() const
{
	class wxPGEventAdapter : public wxPGEditorDialogAdapter {
		class optionTreeItemData : public wxTreeItemData {
			IMetaObject* m_currOpt;
		public:
			optionTreeItemData(IMetaObject* opt)
				: wxTreeItemData(), m_currOpt(opt)
			{
			};
			IMetaObject* GetOption() const {
				return m_currOpt;
			}
		};

		void FillByClsid(IMetadata* metaData, const CLASS_ID& clsid,
			wxCheckTree* tc, wxVariantRecordData* oldList) {

			wxImageList* imageList = tc->GetImageList();
			wxASSERT(imageList);
			IObjectValueAbstract* so = CValue::GetAvailableObject(clsid);
			int groupIcon = imageList->Add(so->GetClassIcon());
			const wxTreeItemId& parentID = tc->AppendItem(tc->GetRootItem(), so->GetClassName(),
				groupIcon, groupIcon);
			for (auto metaObject : metaData->GetMetaObjects(clsid)) {
				IMetaObjectRegisterData* registerData = dynamic_cast<IMetaObjectRegisterData*>(metaObject);
				if (registerData && registerData->HasRecorder()) {
					{
						int icon = imageList->Add(registerData->GetIcon());
						optionTreeItemData* itemData = new optionTreeItemData(metaObject);
						wxTreeItemId newItem = tc->AppendItem(parentID, registerData->GetSynonym(),
							icon, icon,
							itemData);

						if (oldList != NULL) {
							tc->SetItemState(newItem, oldList->Contains(registerData->GetMetaID()) ? wxCheckTree::CHECKED : wxCheckTree::UNCHECKED);
							tc->Check(newItem, oldList->Contains(registerData->GetMetaID()));
						}
						else {
							tc->SetItemState(newItem, wxCheckTree::UNCHECKED);
							tc->Check(newItem, false);
						}
					}
				}
			}
		}

	public:
		wxPGEventAdapter() : wxPGEditorDialogAdapter()
		{
		}

		virtual ~wxPGEventAdapter()
		{
		}

		virtual bool DoShowDialog(wxPropertyGrid* pg, wxPGProperty* prop) wxOVERRIDE
		{
			wxPGRecordSelectorProperty* dlgProp = wxDynamicCast(prop, wxPGRecordSelectorProperty);
			wxCHECK_MSG(dlgProp, false, "Function called for incompatible property");

			IObjectBase* curObject = dlgProp->GetObject();

			// launch editor dialog
			wxDialog* dlg = new wxDialog(pg, wxID_ANY, _("Choice recorders"), wxDefaultPosition, wxDefaultSize,
				wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCLIP_CHILDREN);

			dlg->SetFont(pg->GetFont()); // To allow entering chars of the same set as the propGrid

			// Multi-line text editor dialog.
			const int spacing = wxPropertyGrid::IsSmallScreen() ? 4 : 8;
			wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
			wxBoxSizer* rowsizer = new wxBoxSizer(wxHORIZONTAL);

			wxCheckTree* tc = new wxCheckTree(dlg, wxID_ANY,
				wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_NO_LINES | wxTR_HIDE_ROOT | wxCR_MULTIPLE_CHECK | wxCR_EMPTY_CHECK | wxSUNKEN_BORDER);

			wxTreeItemId rootItem = tc->AddRoot(wxEmptyString);

			rowsizer->Add(tc, wxSizerFlags(1).Expand().Border(wxALL, spacing));
			topsizer->Add(rowsizer, wxSizerFlags(1).Expand());

			tc->SetDoubleBuffered(true);
			tc->Enable(!dlgProp->HasFlag(wxPG_PROP_READONLY));

			wxStdDialogButtonSizer* buttonSizer = dlg->CreateStdDialogButtonSizer(wxOK | wxCANCEL);
			topsizer->Add(buttonSizer, wxSizerFlags(0).Right().Border(wxBOTTOM | wxRIGHT, spacing));

			dlg->SetSizer(topsizer);
			topsizer->SetSizeHints(dlg);

			if (!wxPropertyGrid::IsSmallScreen()) {
				dlg->SetSize(400, 300);
				dlg->Move(pg->GetGoodEditorDialogPosition(prop, dlg->GetSize()));
			}

			tc->SetFocus();

			wxVariant value = dlgProp->GetValue();
			wxVariantRecordData* oldList =
				dynamic_cast<wxVariantRecordData*>(value.GetData());

			// Make an state image list containing small icons
			tc->SetImageList(
				new wxImageList(ICON_SIZE, ICON_SIZE)
			);

			IMetadata* metaData = curObject->GetMetaData();
			wxASSERT(metaData);
			if (metaData) {
				FillByClsid(metaData, g_metaInformationRegisterCLSID, tc, oldList);
				FillByClsid(metaData, g_metaAccumulationRegisterCLSID, tc, oldList);
			}

			tc->ExpandAll(); int res = dlg->ShowModal();
			wxVariantRecordData* list = new wxVariantRecordData(metaData);

			wxArrayTreeItemIds ids; wxString result, description;
			unsigned int selCount = tc->GetSelections(ids);
			for (const wxTreeItemId& selItem : ids) {
				if (selItem.IsOk()) {
					wxTreeItemData* dataItem = tc->GetItemData(selItem);
					if (dataItem
						&& res == wxID_OK) {
						IMetaObject* item = dynamic_cast<
							optionTreeItemData*>(dataItem)->GetOption();
						list->AppendRecord(item->GetMetaID());
					}
				}
			}

			SetValue(list);

			dlg->Destroy();
			return res == wxID_OK;
		}
	};

	return new wxPGEventAdapter();
}

// -----------------------------------------------------------------------
// wxPGOwnerSelectorProperty
// -----------------------------------------------------------------------

wxPG_IMPLEMENT_PROPERTY_CLASS(wxPGOwnerSelectorProperty, wxStringProperty, ComboBoxAndButton)

#include "metadata/metaObjects/objects/catalog.h"

void wxPGOwnerSelectorProperty::FillByClsid(const CLASS_ID& clsid)
{
	IMetadata* metaData = m_curObject->GetMetaData();
	wxASSERT(metaData);
	for (auto metaObject : metaData->GetMetaObjects(clsid)) {
		IMetaObjectRecordDataRef* registerData =
			dynamic_cast<IMetaObjectRecordDataRef*>(metaObject);
		m_choices.Add(registerData->GetSynonym(), registerData->GetIcon(), registerData->GetMetaID());
	}
}

wxPGOwnerSelectorProperty::wxPGOwnerSelectorProperty(const wxString& label, const wxString& name, const wxVariant& value, IObjectBase* curObject)
	: wxStringProperty(label, name), m_curObject(curObject)
{
	FillByClsid(g_metaCatalogCLSID);

	//m_flags |= wxPG_PROP_READONLY;
	m_flags |= wxPG_PROP_ACTIVE_BTN; // Property button always enabled.

	SetValue(value);
}

wxString wxPGOwnerSelectorProperty::ValueToString(wxVariant& value, int argFlags) const
{
	return value.GetString();
}

bool wxPGOwnerSelectorProperty::StringToValue(wxVariant& variant,
	const wxString& text,
	int argFlags) const
{
	return false;
}

bool wxPGOwnerSelectorProperty::IntToValue(wxVariant& value, int number, int argFlags) const
{
	IMetadata* metaData = m_curObject->GetMetaData();
	wxASSERT(metaData);
	wxVariantOwnerData* list =
		new wxVariantOwnerData(metaData);
	list->AppendRecord(
		m_choices.GetValue(number)
	);
	value = list;
	return true;
}

wxPGEditorDialogAdapter* wxPGOwnerSelectorProperty::GetEditorDialog() const
{
	class wxPGEventAdapter : public wxPGEditorDialogAdapter {
		class optionTreeItemData : public wxTreeItemData {
			IMetaObject* m_currOpt;
		public:
			optionTreeItemData(IMetaObject* opt)
				: wxTreeItemData(), m_currOpt(opt)
			{
			};
			IMetaObject* GetOption() const {
				return m_currOpt;
			}
		};

		void FillByClsid(IMetadata* metaData, const CLASS_ID& clsid,
			wxCheckTree* tc, wxVariantOwnerData* oldList) {

			wxImageList* imageList = tc->GetImageList();
			wxASSERT(imageList);
			IObjectValueAbstract* so = CValue::GetAvailableObject(clsid);
			int groupIcon = imageList->Add(so->GetClassIcon());
			const wxTreeItemId& parentID = tc->AppendItem(tc->GetRootItem(), so->GetClassName(),
				groupIcon, groupIcon);
			for (auto metaObject : metaData->GetMetaObjects(clsid)) {
				IMetaObjectRecordDataRef* registerData = dynamic_cast<IMetaObjectRecordDataRef*>(metaObject);
				if (registerData != NULL) {
					{
						int icon = imageList->Add(registerData->GetIcon());
						optionTreeItemData* itemData = new optionTreeItemData(metaObject);
						wxTreeItemId newItem = tc->AppendItem(parentID, registerData->GetSynonym(),
							icon, icon,
							itemData);

						if (oldList != NULL) {
							tc->SetItemState(newItem, oldList->Contains(registerData->GetMetaID()) ? wxCheckTree::CHECKED : wxCheckTree::UNCHECKED);
							tc->Check(newItem, oldList->Contains(registerData->GetMetaID()));
						}
						else {
							tc->SetItemState(newItem, wxCheckTree::UNCHECKED);
							tc->Check(newItem, false);
						}
					}
				}
			}
		}

	public:
		wxPGEventAdapter() : wxPGEditorDialogAdapter()
		{
		}

		virtual ~wxPGEventAdapter()
		{
		}

		virtual bool DoShowDialog(wxPropertyGrid* pg, wxPGProperty* prop) wxOVERRIDE
		{
			wxPGOwnerSelectorProperty* dlgProp = wxDynamicCast(prop, wxPGOwnerSelectorProperty);
			wxCHECK_MSG(dlgProp, false, "Function called for incompatible property");

			IObjectBase* curObject = dlgProp->GetObject();

			// launch editor dialog
			wxDialog* dlg = new wxDialog(pg, wxID_ANY, _("Choice owners"), wxDefaultPosition, wxDefaultSize,
				wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCLIP_CHILDREN);

			dlg->SetFont(pg->GetFont()); // To allow entering chars of the same set as the propGrid

			// Multi-line text editor dialog.
			const int spacing = wxPropertyGrid::IsSmallScreen() ? 4 : 8;
			wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
			wxBoxSizer* rowsizer = new wxBoxSizer(wxHORIZONTAL);

			wxCheckTree* tc = new wxCheckTree(dlg, wxID_ANY,
				wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_NO_LINES | wxTR_HIDE_ROOT | wxCR_MULTIPLE_CHECK | wxCR_EMPTY_CHECK | wxSUNKEN_BORDER);

			wxTreeItemId rootItem = tc->AddRoot(wxEmptyString);

			rowsizer->Add(tc, wxSizerFlags(1).Expand().Border(wxALL, spacing));
			topsizer->Add(rowsizer, wxSizerFlags(1).Expand());

			tc->SetDoubleBuffered(true);
			tc->Enable(!dlgProp->HasFlag(wxPG_PROP_READONLY));

			wxStdDialogButtonSizer* buttonSizer = dlg->CreateStdDialogButtonSizer(wxOK | wxCANCEL);
			topsizer->Add(buttonSizer, wxSizerFlags(0).Right().Border(wxBOTTOM | wxRIGHT, spacing));

			dlg->SetSizer(topsizer);
			topsizer->SetSizeHints(dlg);

			if (!wxPropertyGrid::IsSmallScreen()) {
				dlg->SetSize(400, 300);
				dlg->Move(pg->GetGoodEditorDialogPosition(prop, dlg->GetSize()));
			}

			tc->SetFocus();

			wxVariant value = dlgProp->GetValue();
			wxVariantOwnerData* oldList =
				dynamic_cast<wxVariantOwnerData*>(value.GetData());

			// Make an state image list containing small icons
			tc->SetImageList(
				new wxImageList(ICON_SIZE, ICON_SIZE)
			);

			IMetadata* metaData = curObject->GetMetaData();
			wxASSERT(metaData);
			if (metaData != NULL) {
				FillByClsid(metaData, g_metaCatalogCLSID, tc, oldList);
			}

			tc->ExpandAll(); int res = dlg->ShowModal();
			wxVariantOwnerData* list = new wxVariantOwnerData(metaData);

			wxArrayTreeItemIds ids; wxString result, description;
			unsigned int selCount = tc->GetSelections(ids);
			for (const wxTreeItemId& selItem : ids) {
				if (selItem.IsOk()) {
					wxTreeItemData* dataItem = tc->GetItemData(selItem);
					if (dataItem
						&& res == wxID_OK) {
						IMetaObject* item = dynamic_cast<
							optionTreeItemData*>(dataItem)->GetOption();
						list->AppendRecord(item->GetMetaID());
					}
				}
			}

			SetValue(list);

			dlg->Destroy();
			return res == wxID_OK;
		}
	};

	return new wxPGEventAdapter();
}

// -----------------------------------------------------------------------
// wxPGSourceDataProperty
// -----------------------------------------------------------------------

#include "frontend/visualView/controls/attributeControl.h"

wxPG_IMPLEMENT_PROPERTY_CLASS(wxPGSourceDataProperty, wxPGProperty, TextCtrlAndButton)

wxPGSourceDataProperty::wxPGSourceDataProperty(const wxString& label, const wxString& name,
	const wxVariant& value, IObjectBase* curObject) : wxPGProperty(label, name), m_curObject(curObject),
	m_srcDataType(
		curObject->GetComponentType() == COMPONENT_TYPE_WINDOW_TABLE ?
		eSourceDataType::eSourceDataVariant_table :
		curObject->GetComponentType() == COMPONENT_TYPE_TABLE_COLUMN ?
		eSourceDataType::eSourceDataVariant_tableColumn :
		eSourceDataVariant_attribute
	)
{
	wxVariantSourceData* currSrcData =
		dynamic_cast<wxVariantSourceData*>(value.GetData());

	if (currSrcData != NULL) {
		m_typeSelector = new wxPGTypeSelectorProperty(wxT("type"), wxPG_LABEL,
			new wxVariantSourceAttributeData(*currSrcData->GetAttributeData()),
			m_srcDataType == eSourceDataType::eSourceDataVariant_table ? eSelectorDataType::eSelectorDataType_table : eSelectorDataType::eSelectorDataType_reference, curObject
		);
	}
	else {

		IValueFrame* curControl = dynamic_cast<IValueFrame*>(m_curObject);
		wxASSERT(curControl);
		CValueForm* ownerForm = curControl ? curControl->GetOwnerForm() : NULL;
		wxASSERT(ownerForm);

		m_typeSelector = new wxPGTypeSelectorProperty(wxT("type"), wxPG_LABEL,
			new wxVariantSourceAttributeData(curObject->GetMetaData(), ownerForm, wxNOT_FOUND),
			m_srcDataType == eSourceDataType::eSourceDataVariant_table ? eSelectorDataType::eSelectorDataType_table : eSelectorDataType::eSelectorDataType_reference, curObject
		);
	}

	AddPrivateChild(m_typeSelector);

	//m_flags |= wxPG_PROP_READONLY;
	m_flags |= wxPG_PROP_ACTIVE_BTN; // Property button always enabled.

	SetValue(value);
}

wxPGSourceDataProperty::~wxPGSourceDataProperty() {}

wxString wxPGSourceDataProperty::ValueToString(wxVariant& variant,
	int WXUNUSED(argFlags)) const
{
	wxVariantSourceData* currSrcData =
		dynamic_cast<wxVariantSourceData*>(variant.GetData());

	if (currSrcData != NULL) {
		currSrcData->RefreshData();
	}

	return variant.GetString();
}

bool wxPGSourceDataProperty::StringToValue(wxVariant& variant, const wxString& text, int argFlags) const
{
	if (text.IsEmpty()) {

		wxVariantSourceData* currSrcData =
			dynamic_cast<wxVariantSourceData*>(variant.GetData());

		if (currSrcData != NULL) {

			IValueFrame* curControl = dynamic_cast<IValueFrame*>(m_curObject);
			wxASSERT(curControl);
			CValueForm* ownerForm = curControl ? curControl->GetOwnerForm() : NULL;
			wxASSERT(ownerForm);
			wxVariantSourceData* srcData = new wxVariantSourceData(
				curControl->GetMetaData(), ownerForm, wxNOT_FOUND
			);

			wxVariant varData = new wxVariantSourceAttributeData(*currSrcData->GetAttributeData());

			wxVariantSourceAttributeData* attrData = srcData->GetAttributeData();
			wxASSERT(attrData);
			attrData->LoadFromVariant(varData);

			variant = srcData;
			return true;
		}

		return false;

	}

	return text.IsEmpty();
}

wxVariant wxPGSourceDataProperty::ChildChanged(wxVariant& thisValue, int childIndex, wxVariant& childValue) const
{
	wxVariantSourceData* currSrcData =
		dynamic_cast<wxVariantSourceData*>(thisValue.GetData());

	if (currSrcData != NULL) {
		wxVariantSourceData* newSrcData =
			new wxVariantSourceData(*currSrcData);

		if (childIndex == 0) {
			if (!m_typeSelector->HasFlag(wxPG_PROP_READONLY)) {
				newSrcData->SetSourceId(wxNOT_FOUND);
			}
			wxVariantSourceAttributeData* attrData =
				newSrcData->GetAttributeData();
			wxASSERT(attrData);
			attrData->LoadFromVariant(childValue);
		}

		return newSrcData;
	}

	return wxVariant();
}

void wxPGSourceDataProperty::RefreshChildren()
{
	wxVariantSourceData* currSrcData =
		dynamic_cast<wxVariantSourceData*>(m_value.GetData());

	if (currSrcData != NULL) {
		if (currSrcData->GetSourceId() != wxNOT_FOUND) {
			m_typeSelector->SetValue(
				new wxVariantSourceAttributeData(
					currSrcData->GetMetadata(),
					currSrcData->GetOwnerForm(),
					currSrcData->GetSourceId()
				)
			);
		}
		else {
			wxVariantSourceAttributeData* attrData =
				currSrcData->GetAttributeData();
			wxASSERT(attrData);
			m_typeSelector->SetValue(
				new wxVariantSourceAttributeData(*attrData)
			);
		}
	}

	if (currSrcData != NULL) {
		IMetadata* metaData = currSrcData->GetMetadata(); bool enabledProp = true;
		if (metaData != NULL &&
			wxNOT_FOUND != currSrcData->GetSourceId()) {
			enabledProp = false;
			IMetaObject* metaObject = metaData->GetMetaObject(currSrcData->GetSourceId());
			wxASSERT(metaObject);
			if (!metaObject->IsAllowed()) {
				enabledProp = true;
			}
		}
		m_typeSelector->SetFlagRecursively(wxPG_PROP_READONLY, !enabledProp);
	}
	else {
		m_typeSelector->SetFlagRecursively(wxPG_PROP_READONLY, false);
	}
}

extern wxImageList* GetImageList();

#define icon_attribute 219 
#define icon_table 217

#include "frontend/visualView/controls/tableBox.h"

wxPGEditorDialogAdapter* wxPGSourceDataProperty::GetEditorDialog() const
{
	class wxPGEventAdapter : public wxPGEditorDialogAdapter {

		class metaTreeItemData : public wxTreeItemData {
			const wxString m_nameProp;
			const meta_identifier_t m_id;
			const bool m_tableSection;
		public:
			metaTreeItemData(const wxString& nameProp, const meta_identifier_t& id,
				bool tableSection)
				: wxTreeItemData(), m_nameProp(nameProp), m_id(id), m_tableSection(tableSection)
			{
			};

			const wxString& GetPropName() const { return m_nameProp; }
			const meta_identifier_t& GetID() const { return m_id; }
			const bool IsTableSection() const { return m_tableSection; }
		};

		bool ProcessAttribute(wxPropertyGrid* pg,
			wxPGSourceDataProperty* dlgProp, wxVariantSourceData* srcData) {

			IValueFrame* currentControl = wxDynamicCast(
				dlgProp->GetObject(), IValueFrame);

			if (currentControl == NULL)
				return false;

			meta_identifier_t dataSource = wxNOT_FOUND;

			if (srcData != NULL) {
				dataSource = srcData->GetSourceId();
			}

			// launch editor dialog
			wxDialog* dlg = new wxDialog(pg, wxID_ANY, _("Choice source"), wxDefaultPosition, wxDefaultSize,
				wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCLIP_CHILDREN);

			dlg->SetFont(pg->GetFont()); // To allow entering chars of the same set as the propGrid

			// Multi-line text editor dialog.
			const int spacing = wxPropertyGrid::IsSmallScreen() ? 4 : 8;

			wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
			wxBoxSizer* rowsizer = new wxBoxSizer(wxHORIZONTAL);

			wxTreeCtrl* tc = new wxTreeCtrl(dlg, wxID_ANY,
				wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_NO_LINES | wxSUNKEN_BORDER);

			tc->SetImageList(::GetImageList());

			rowsizer->Add(tc, wxSizerFlags(1).Expand().Border(wxALL, spacing));
			topsizer->Add(rowsizer, wxSizerFlags(1).Expand());

			tc->SetDoubleBuffered(true);
			tc->Enable(!dlgProp->HasFlag(wxPG_PROP_READONLY));

			wxStdDialogButtonSizer* buttonSizer = dlg->CreateStdDialogButtonSizer(wxOK | wxCANCEL);
			topsizer->Add(buttonSizer, wxSizerFlags(0).Right().Border(wxBOTTOM | wxRIGHT, spacing));

			dlg->SetSizer(topsizer);
			topsizer->SetSizeHints(dlg);

			if (!wxPropertyGrid::IsSmallScreen()) {
				dlg->SetSize(400, 300);
				dlg->Move(pg->GetGoodEditorDialogPosition(dlgProp, dlg->GetSize()));
			}

			tc->SetFocus();

			CValueForm* ownerForm = currentControl->GetOwnerForm();
			wxASSERT(ownerForm);
			ISourceDataObject* srcObject = ownerForm->GetSourceObject();
			if (srcObject != NULL) {
				CSourceExplorer srcExplorer = srcObject->GetSourceExplorer();
				metaTreeItemData* srcItemData = NULL;
				if (currentControl->FilterSource(srcExplorer, srcExplorer.GetMetaIDSource())) {
					if (srcExplorer.IsSelect()) {
						srcItemData = new metaTreeItemData(
							srcExplorer.GetSourceName(),
							srcExplorer.GetMetaIDSource(),
							srcExplorer.IsTableSection()
						);
					}
				}
				wxTreeItemId rootItem = NULL;
				if (srcExplorer.IsTableSection()) {
					rootItem = tc->AddRoot(srcExplorer.GetSourceName(), icon_table, icon_table, srcItemData);
				}
				else {
					rootItem = tc->AddRoot(srcExplorer.GetSourceName(), icon_attribute, icon_attribute, srcItemData);
				}
				if (srcExplorer.GetMetaIDSource() == dataSource) {
					tc->SelectItem(rootItem);
				}
				for (unsigned int idx = 0; idx < srcExplorer.GetHelperCount(); idx++) {
					CSourceExplorer srcNextExplorer = srcExplorer.GetHelper(idx);
					metaTreeItemData* itemData = NULL;
					if (currentControl->FilterSource(srcNextExplorer, srcExplorer.GetMetaIDSource())) {
						if (srcNextExplorer.IsSelect()) {
							itemData = new metaTreeItemData(
								srcNextExplorer.GetSourceName(),
								srcNextExplorer.GetMetaIDSource(),
								srcNextExplorer.IsTableSection()
							);
						}
					}
					wxTreeItemId newItem = NULL;
					if (srcNextExplorer.IsTableSection()) {
						newItem = tc->AppendItem(rootItem, srcNextExplorer.GetSourceName(), icon_table, icon_table, itemData);
					}
					else {
						newItem = tc->AppendItem(rootItem, srcNextExplorer.GetSourceName(), icon_attribute, icon_attribute, itemData);
					}
					if (srcNextExplorer.GetMetaIDSource() == dataSource) {
						tc->SelectItem(newItem);
					}
					bool needDelete = itemData == NULL;
					if (srcNextExplorer.IsTableSection()) {
						tc->SetItemBold(newItem);
						for (unsigned int a = 0; a < srcNextExplorer.GetHelperCount(); a++) {
							CSourceExplorer srcColExplorer = srcNextExplorer.GetHelper(a);
							metaTreeItemData* itemTableData = NULL;
							if (currentControl->FilterSource(srcColExplorer, srcNextExplorer.GetMetaIDSource())) {
								itemTableData = new metaTreeItemData(
									srcColExplorer.GetSourceName(),
									srcColExplorer.GetMetaIDSource(),
									srcColExplorer.IsTableSection()
								);
								wxTreeItemId nextItem = tc->AppendItem(newItem, srcColExplorer.GetSourceName(), icon_attribute, icon_attribute, itemTableData);
								if (srcColExplorer.GetMetaIDSource() == dataSource) {
									tc->SelectItem(nextItem);
								}
								needDelete = !currentControl->FilterSource(srcNextExplorer, srcNextExplorer.GetMetaIDSource());
							}
						}
					}

					if (needDelete) {
						tc->Delete(newItem);
					}
				}

				if (srcExplorer.IsTableSection()) {
					tc->SetItemBold(rootItem);
				}

				tc->Expand(rootItem);
			}

			tc->ExpandAll(); int res = dlg->ShowModal();

			bool tableBox =
				currentControl->GetComponentType() == COMPONENT_TYPE_WINDOW_TABLE;

			wxTreeItemId selItem = tc->GetSelection();
			if (selItem.IsOk()) {
				wxTreeItemData* dataItem = tc->GetItemData(selItem);
				if (dataItem
					&& res == wxID_OK) {
					metaTreeItemData* item = dynamic_cast<
						metaTreeItemData*>(dataItem);
					wxASSERT(item);
					if (tableBox == item->IsTableSection()) {
						SetValue(
							new wxVariantSourceData(currentControl->GetMetaData(), ownerForm, item->GetID())
						);
					}
					else {
						dlg->Destroy();
						return false;
					}
				}
				else if (!dataItem) {
					dlg->Destroy();
					return false;
				}
			}

			dlg->Destroy();
			return res == wxID_OK
				&& selItem.IsOk();
		}

		bool ProcessTableColumn(wxPropertyGrid* pg,
			wxPGSourceDataProperty* dlgProp, wxVariantSourceData* srcData) {

			IValueFrame* currentControl = wxDynamicCast(
				dlgProp->GetObject(), IValueFrame);

			if (currentControl == NULL)
				return false;

			meta_identifier_t dataSource = wxNOT_FOUND;
			if (srcData != NULL) {
				dataSource = srcData->GetSourceId();
			}

			IMetadata* metaData = currentControl->GetMetaData();
			if (metaData == NULL) {
				return false;
			}

			CValueTableBox* tableBox = wxDynamicCast(
				currentControl->GetParent(), CValueTableBox
			);

			CLASS_ID clsid = 0;
			if (tableBox != NULL) {
				auto clsids = tableBox->GetClsids();
				if (clsids.size() > 0) {
					auto itStart = clsids.begin();
					std::advance(itStart, 0);
					clsid = *itStart;
				}
			}
			if (clsid == 0)
				return false;

			IMetaTypeObjectValueSingle* singleObject =
				metaData->GetTypeObject(clsid);

			if (singleObject == NULL)
				return false;

			ITableAttribute* metaObject =
				dynamic_cast<ITableAttribute*>(singleObject->GetMetaObject());

			if (metaObject == NULL)
				return false;

			// launch editor dialog
			wxDialog* dlg = new wxDialog(pg, wxID_ANY, _("Choice source"), wxDefaultPosition, wxDefaultSize,
				wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCLIP_CHILDREN);

			dlg->SetFont(pg->GetFont()); // To allow entering chars of the same set as the propGrid

			// Multi-line text editor dialog.
			const int spacing = wxPropertyGrid::IsSmallScreen() ? 4 : 8;

			wxBoxSizer* topsizer = new wxBoxSizer(wxVERTICAL);
			wxBoxSizer* rowsizer = new wxBoxSizer(wxHORIZONTAL);

			wxTreeCtrl* tc = new wxTreeCtrl(dlg, wxID_ANY,
				wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_LINES_AT_ROOT | wxTR_NO_LINES | wxSUNKEN_BORDER);

			tc->SetImageList(::GetImageList());

			rowsizer->Add(tc, wxSizerFlags(1).Expand().Border(wxALL, spacing));
			topsizer->Add(rowsizer, wxSizerFlags(1).Expand());

			tc->SetDoubleBuffered(true);
			tc->Enable(!dlgProp->HasFlag(wxPG_PROP_READONLY));

			wxStdDialogButtonSizer* buttonSizer = dlg->CreateStdDialogButtonSizer(wxOK | wxCANCEL);
			topsizer->Add(buttonSizer, wxSizerFlags(0).Right().Border(wxBOTTOM | wxRIGHT, spacing));

			dlg->SetSizer(topsizer);
			topsizer->SetSizeHints(dlg);

			if (!wxPropertyGrid::IsSmallScreen()) {
				dlg->SetSize(400, 300);
				dlg->Move(pg->GetGoodEditorDialogPosition(dlgProp, dlg->GetSize()));
			}

			tc->SetFocus();

			metaTreeItemData* srcItemData = new metaTreeItemData(
				tableBox->GetControlName(),
				wxNOT_FOUND,
				true
			);

			wxTreeItemId rootItem =
				tc->AddRoot(tableBox->GetControlName(), icon_table, icon_table, srcItemData);

			for (auto attribute : metaObject->GetGenericAttributes()) {
				if (!attribute->IsAllowed())
					continue;
				metaTreeItemData* itemData = itemData = new metaTreeItemData(
					attribute->GetName(),
					attribute->GetMetaID(),
					false
				);
				wxTreeItemId newItem = tc->AppendItem(rootItem, attribute->GetName(), icon_attribute, icon_attribute, itemData);
				if (dataSource == attribute->GetMetaID()) {
					tc->SelectItem(newItem);
				}
			}

			tc->ExpandAll();

			int res = dlg->ShowModal();

			wxTreeItemId selItem = tc->GetSelection();
			if (selItem.IsOk()) {
				wxTreeItemData* dataItem = tc->GetItemData(selItem);
				if (dataItem
					&& res == wxID_OK) {
					metaTreeItemData* item = dynamic_cast<
						metaTreeItemData*>(dataItem);
					wxASSERT(item);
					SetValue(
						new wxVariantSourceData(
							currentControl->GetMetaData(),
							currentControl->GetOwnerForm(),
							item->GetID()
						)
					);
				}
				else if (!dataItem) {
					dlg->Destroy();
					return false;
				}
			}

			dlg->Destroy();
			return res == wxID_OK
				&& selItem.IsOk();
		}

	public:
		wxPGEventAdapter() : wxPGEditorDialogAdapter()
		{
		}

		virtual ~wxPGEventAdapter()
		{
		}

		virtual bool DoShowDialog(wxPropertyGrid* pg, wxPGProperty* prop) wxOVERRIDE
		{
			wxPGSourceDataProperty* dlgProp = wxDynamicCast(prop, wxPGSourceDataProperty);
			wxCHECK_MSG(dlgProp, false, "Function called for incompatible property");

			wxVariant value = dlgProp->GetValue();
			wxVariantSourceData* srcData =
				dynamic_cast<wxVariantSourceData*>(value.GetData());

			if (dlgProp->GetSourceDataType() == eSourceDataType::eSourceDataVariant_attribute
				|| dlgProp->GetSourceDataType() == eSourceDataType::eSourceDataVariant_table)
				return ProcessAttribute(pg, dlgProp, srcData);
			else if (dlgProp->GetSourceDataType() == eSourceDataType::eSourceDataVariant_tableColumn)
				return ProcessTableColumn(pg, dlgProp, srcData);

			return false;
		}
	};

	return new wxPGEventAdapter();
}