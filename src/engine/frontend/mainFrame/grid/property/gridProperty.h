#ifndef _gridProperty_H__
#define _gridProperty_H__

#include <wx/grid.h>
#include "backend/propertyManager/propertyManager.h"

class CGrid;

#include "frontend/visualView/special/enum/valueEnum.h"

class CPropertyObjectGrid : public IPropertyObject {
	std::vector<wxGridBlockCoords> m_currentBlocks;
	CGrid* m_ownerGrid;
private:

	CPropertyCategory* m_categoryGeneral = IPropertyObject::CreatePropertyCategory(wxT("general"), _("general"));
	CPropertyName* m_propertyName = IPropertyObject::CreateProperty<CPropertyName>(m_categoryGeneral, wxT("name"), _("name"), wxEmptyString);
	CPropertyString* m_propertyText = IPropertyObject::CreateProperty<CPropertyString>(m_categoryGeneral, wxT("text"), _("text"), wxEmptyString);

	CPropertyCategory* m_categoryAlignment = IPropertyObject::CreatePropertyCategory(wxT("alignment"), _("alignment"));
	CPropertyEnum<CValueEnumHorizontalAlignment>* m_propertyAlignHorz = IPropertyObject::CreateProperty<CPropertyEnum<CValueEnumHorizontalAlignment>>(m_categoryAlignment, wxT("align_horz"), _("horizontal align"), wxAlignment::wxALIGN_LEFT);
	CPropertyEnum<CValueEnumVerticalAlignment>* m_propertyAlignVert = IPropertyObject::CreateProperty<CPropertyEnum<CValueEnumVerticalAlignment>>(m_categoryAlignment, wxT("align_vert"), _("vertical align"), wxAlignment::wxALIGN_CENTER);
	CPropertyEnum<CValueEnumOrient>* m_propertyOrient = IPropertyObject::CreateProperty<CPropertyEnum<CValueEnumOrient>>(m_categoryAlignment, wxT("orient_text"), _("orientation text"), wxOrientation::wxVERTICAL);

	CPropertyCategory* m_categoryAppearance = IPropertyObject::CreatePropertyCategory(wxT("appearance"), _("appearance"));
	CPropertyFont* m_propertyFont = IPropertyObject::CreateProperty<CPropertyFont>(m_categoryAppearance, wxT("font"), _("font"));
	CPropertyColour* m_propertyBackgroundColour = IPropertyObject::CreateProperty<CPropertyColour>(m_categoryAppearance, wxT("background_colour"), _("background colour"), wxNullColour);
	CPropertyColour* m_propertyTextColour = IPropertyObject::CreateProperty<CPropertyColour>(m_categoryAppearance, wxT("text_colour"), _("text colour"), wxNullColour);

	CPropertyCategory* m_categoryBorder = IPropertyObject::CreatePropertyCategory(wxT("border"), _("border"));
	CPropertyEnum<CValueEnumBorder>* m_propertyLeftBorder = IPropertyObject::CreateProperty<CPropertyEnum<CValueEnumBorder>>(m_categoryBorder, wxT("left_border"), _("left"), wxPENSTYLE_TRANSPARENT);
	CPropertyEnum<CValueEnumBorder>* m_propertyRightBorder = IPropertyObject::CreateProperty<CPropertyEnum<CValueEnumBorder>>(m_categoryBorder, wxT("right_border"), _("right"), wxPENSTYLE_TRANSPARENT);
	CPropertyEnum<CValueEnumBorder>* m_propertyTopBorder = IPropertyObject::CreateProperty<CPropertyEnum<CValueEnumBorder>>(m_categoryBorder, wxT("top_border"), _("top"), wxPENSTYLE_TRANSPARENT);
	CPropertyEnum<CValueEnumBorder>* m_propertyBottomBorder = IPropertyObject::CreateProperty<CPropertyEnum<CValueEnumBorder>>(m_categoryBorder, wxT("bottom_border"), _("bottom"), wxPENSTYLE_TRANSPARENT);
	CPropertyColour* m_propertyColourBorder = IPropertyObject::CreateProperty<CPropertyColour>(m_categoryBorder, wxT("border_colour"), _("colour"), *wxBLACK);

private:

	void ClearSelectedCell() { m_currentBlocks.clear(); }

	void AddSelectedCell(const wxGridBlockCoords& coords, bool afterErase = false);
	void ShowProperty();

	void OnPropertyCreated(IProperty* property, const wxGridBlockCoords& coords);
	void OnPropertyChanged(IProperty* property, const wxGridBlockCoords& coords);

	friend class CGrid;

public:

	CPropertyObjectGrid(CGrid* ownerGrid);
	virtual ~CPropertyObjectGrid();

	//system override 
	virtual int GetComponentType() const { return COMPONENT_TYPE_ABSTRACT; }

	virtual wxString GetObjectTypeName() const override { return wxT("cells"); }
	virtual wxString GetClassName() const { return wxT("cells"); }

	/**
	* Property events
	*/
	virtual void OnPropertyCreated(IProperty* property);
	virtual void OnPropertyChanged(IProperty* property, const wxVariant& oldValue, const wxVariant& newValue);

};

#endif 