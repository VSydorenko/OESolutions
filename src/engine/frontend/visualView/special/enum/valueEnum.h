#ifndef __VALUE_ENUM_H__
#define __VALUE_ENUM_H__

enum enTitleLocation {
	eLeft = 1,
	eRight
};

#pragma region enumeration
#include "backend/compiler/enumUnit.h"
class CValueEnumOrient : public IEnumeration<wxOrientation> {
	wxDECLARE_DYNAMIC_CLASS(CValueEnumOrient);
public:
	CValueEnumOrient() : IEnumeration() {}
	//CValueEnumOrient(wxOrientation orient) : IEnumeration(orient) {}

	virtual void CreateEnumeration() {
		AddEnumeration(wxHORIZONTAL, wxT("horizontal"));
		AddEnumeration(wxVERTICAL, wxT("vertical"));
	}
};

class CValueEnumStretch : public IEnumeration<wxStretch> {
	wxDECLARE_DYNAMIC_CLASS(CValueEnumStretch);
public:
	CValueEnumStretch() : IEnumeration() {}
	//CValueEnumStretch(const wxStretch &v) : IEnumeration(v) {}

	virtual void CreateEnumeration() {
		AddEnumeration(wxStretch::wxSHRINK, wxT("shrink"));
		AddEnumeration(wxStretch::wxEXPAND, wxT("expand"));
	}
};

#include <wx/aui/auibook.h>

class CValueEnumOrientNotebookPage : public IEnumeration<wxAuiNotebookOption> {
	wxDECLARE_DYNAMIC_CLASS(CValueEnumOrientNotebookPage);
public:
	CValueEnumOrientNotebookPage() : IEnumeration() {}
	//CValueEnumOrientNotebookPage(const wxAuiNotebookOption& v) : IEnumeration(v) {}

	virtual void CreateEnumeration() {
		AddEnumeration(wxAuiNotebookOption::wxAUI_NB_TOP, wxT("top"));
		AddEnumeration(wxAuiNotebookOption::wxAUI_NB_BOTTOM, wxT("bottom"));
	}
};

class CValueEnumHorizontalAlignment : public IEnumeration<wxAlignment> {
	wxDECLARE_DYNAMIC_CLASS(CValueEnumHorizontalAlignment);
public:
	CValueEnumHorizontalAlignment() : IEnumeration() {}
	//CValueEnumHorizontalAlignment(const wxAlignment& v) : IEnumeration(v) {}

	virtual void CreateEnumeration() {
		AddEnumeration(wxAlignment::wxALIGN_LEFT, wxT("left"));
		AddEnumeration(wxAlignment::wxALIGN_CENTER, wxT("center"));
		AddEnumeration(wxAlignment::wxALIGN_RIGHT, wxT("right"));
	}
};

class CValueEnumVerticalAlignment : public IEnumeration<wxAlignment> {
	wxDECLARE_DYNAMIC_CLASS(CValueEnumVerticalAlignment);
public:
	CValueEnumVerticalAlignment() : IEnumeration() {}
	//CValueEnumVerticalAlignment(const wxAlignment& v) : IEnumeration(v) {}

	virtual void CreateEnumeration() {
		AddEnumeration(wxAlignment::wxALIGN_TOP, wxT("top"));
		AddEnumeration(wxAlignment::wxALIGN_CENTER, wxT("center"));
		AddEnumeration(wxAlignment::wxALIGN_BOTTOM, wxT("bottom"));
	}
};

class CValueEnumBorder : public IEnumeration<wxPenStyle> {
	wxDECLARE_DYNAMIC_CLASS(CValueEnumBorder);
public:
	CValueEnumBorder() : IEnumeration() {}
	//CValueEnumBorder(const wxPenStyle& v) : IEnumeration(v) {}

	virtual void CreateEnumeration() {
		AddEnumeration(wxPenStyle::wxPENSTYLE_TRANSPARENT, wxT("none"), _("none"));
		AddEnumeration(wxPenStyle::wxPENSTYLE_SOLID, wxT("solid"), _("solid"));
		AddEnumeration(wxPenStyle::wxPENSTYLE_DOT, wxT("dotted"), _("dotted"));
		AddEnumeration(wxPenStyle::wxPENSTYLE_SHORT_DASH, wxT("thin_dashed"), _("thin dashed"));
		AddEnumeration(wxPenStyle::wxPENSTYLE_DOT_DASH, wxT("thick_dashed"), _("thick dashed"));
		AddEnumeration(wxPenStyle::wxPENSTYLE_LONG_DASH, wxT("large_dashed"), _("large dashed"));
	}
};

class CValueEnumTitleLocation : public IEnumeration<enTitleLocation> {
	wxDECLARE_DYNAMIC_CLASS(CValueEnumTitleLocation);
public:
	CValueEnumTitleLocation() : IEnumeration() {}
	//CValueEnumTitleLocation(enTitleLocation v) : IEnumeration(v) {}

	virtual void CreateEnumeration() {
		AddEnumeration(enTitleLocation::eLeft, wxT("left"));
		AddEnumeration(enTitleLocation::eRight, wxT("right"));
	}
};
#pragma endregion 
#endif