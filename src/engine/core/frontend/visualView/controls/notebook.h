#ifndef _NOTEBOOKS_H__
#define _NOTEBOOKS_H__

#include "window.h"
#include <wx/aui/auibook.h>

class CValueNotebookPage;

#include "frontend/visualView/special/enums/valueOrient.h"

class CValueNotebook : public IValueWindow {
	wxDECLARE_DYNAMIC_CLASS(CValueNotebook);
protected:

	friend class CValueNotebookPage;

	OptionList* GetOrientPage(PropertyOption* property)
	{
		OptionList* optList = new OptionList();
		optList->AddOption(_("top"), wxAUI_NB_TOP);
		//optList->AddOption(_("Left", wxAUI_NB_LEFT);
		//optList->AddOption(_("Right", wxAUI_NB_RIGHT);
		optList->AddOption(_("bottom"), wxAUI_NB_BOTTOM);
		return optList;
	}

	PropertyCategory* m_categoryNotebook = IPropertyObject::CreatePropertyCategory("Notebook");
	Property* m_propertyOrient = IPropertyObject::CreateProperty(m_categoryNotebook, "orient_page", &CValueNotebook::GetOrientPage, wxAUI_NB_TOP);
	PropertyCategory* m_categoryEvent = IPropertyObject::CreatePropertyCategory("Event");
	Event* m_eventOnPageChanged = IPropertyObject::CreateEvent(m_categoryEvent, "onPageChanged", { "page" });

private:
	CValueNotebookPage* m_activePage;
	std::vector< CValueNotebookPage*> m_aPages;
	bool m_bInitialized;
public:

	CValueNotebook();

	virtual wxObject* Create(wxObject* parent, IVisualHost* visualHost) override;
	virtual void OnCreated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost, bool first�reated) override;
	virtual void OnUpdated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost) override;
	virtual void OnSelected(wxObject* wxobject) override;
	virtual void Update(wxObject* wxobject, IVisualHost* visualHost) override;
	virtual void Cleanup(wxObject* obj, IVisualHost* visualHost) override;

	virtual wxString GetClassName() const override {
		return wxT("notebook");
	}

	virtual wxString GetObjectTypeName() const override {
		return wxT("notebook");
	}

	//methods 
	virtual void PrepareNames() const override;                          //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t& aParams) override;       //����� ������

	/**
	* Support default menu
	*/
	virtual void PrepareDefaultMenu(wxMenu* m_menu);
	virtual void ExecuteMenu(IVisualHost* visualHost, int id);

	void AddNotebookPage();

	//load & save object in control 
	virtual bool LoadData(CMemoryReader& reader);
	virtual bool SaveData(CMemoryWriter& writer = CMemoryWriter());

protected:

	//Events
	void OnEnablePage(wxCommandEvent& event);
	void OnChangedPage(wxAuiNotebookEvent& event);
};

class CValueNotebookPage : public IValueControl {
	wxDECLARE_DYNAMIC_CLASS(CValueNotebookPage);
protected:
	PropertyCategory* m_categoryPage = IPropertyObject::CreatePropertyCategory("Page");
	Property* m_propertyCaption = IPropertyObject::CreateProperty(m_categoryPage, "caption", PropertyType::PT_WXSTRING, _("New page"));
	Property* m_propertyVisible = IPropertyObject::CreateProperty(m_categoryPage, "visible", PropertyType::PT_BOOL, true);
	Property* m_propertyIcon = IPropertyObject::CreateProperty(m_categoryPage, "icon", PropertyType::PT_BITMAP);

	PropertyCategory* m_categorySizer = IPropertyObject::CreatePropertyCategory("Sizer");
	Property* m_propertyOrient = IPropertyObject::CreateProperty(m_categorySizer, "orient", &CValueNotebookPage::GetOrient, wxVERTICAL);
public:

	CValueNotebookPage();

	virtual wxObject* Create(wxObject* parent, IVisualHost* visualHost) override;
	virtual void OnCreated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost, bool first�reated) override;
	virtual void OnUpdated(wxObject* wxobject, wxWindow* wxparent, IVisualHost* visualHost) override;
	virtual void OnSelected(wxObject* wxobject) override;
	virtual void Cleanup(wxObject* obj, IVisualHost* visualHost) override;

	virtual wxString GetClassName() const override {
		return wxT("page");
	}

	virtual wxString GetObjectTypeName() const override {
		return wxT("page");
	}

	virtual bool CanDeleteControl() const;

	virtual int GetComponentType() const override {
		return COMPONENT_TYPE_WINDOW;
	}

	//load & save object in control 
	virtual bool LoadData(CMemoryReader& reader);
	virtual bool SaveData(CMemoryWriter& writer = CMemoryWriter());

	friend class CValueNotebook;
};

#endif 