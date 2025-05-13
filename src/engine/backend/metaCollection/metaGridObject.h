#ifndef _METAGRIDOBJECT_H__
#define _METAGRIDOBJECT_H__

#include "metaObject.h"

class BACKEND_API IMetaObjectGrid : public IMetaObject {
	wxDECLARE_ABSTRACT_CLASS(IMetaObjectGrid);
protected:
	enum
	{
		ID_METATREE_OPEN_TEMPLATE = 19000,
	};

public:

	//support icons
	virtual wxIcon GetIcon() const;
	static wxIcon GetIconGroup();

	//module manager is started or exit 
	virtual bool OnBeforeRunMetaObject(int flags);
	virtual bool OnAfterCloseMetaObject();

	//prepare menu for item
	virtual bool PrepareContextMenu(wxMenu* defaultMenu);
	virtual void ProcessCommand(unsigned int id);

protected:

	virtual bool LoadData(CMemoryReader& reader);
	virtual bool SaveData(CMemoryWriter& writer = CMemoryWriter());
};

class BACKEND_API CMetaObjectGrid : public IMetaObjectGrid {
	wxDECLARE_DYNAMIC_CLASS(CMetaObjectCommonGrid);
protected:
	CPropertyCategory* m_categoryTemplate = IPropertyObject::CreatePropertyCategory(wxT("template"), _("template"));
	CPropertyTempalate* m_propertyTemplate = IPropertyObject::CreateProperty<CPropertyTempalate>(m_categoryTemplate, wxT("templateData"), _("template data"));
public:
};

class BACKEND_API CMetaObjectCommonGrid : public IMetaObjectGrid {
	wxDECLARE_DYNAMIC_CLASS(CMetaObjectCommonGrid);
protected:
	CPropertyCategory* m_categoryTemplate = IPropertyObject::CreatePropertyCategory(wxT("commonTemplate"), _("common template"));
	CPropertyTempalate* m_propertyTemplate = IPropertyObject::CreateProperty<CPropertyTempalate>(m_categoryTemplate, wxT("templateData"), _("template data"));
public:
};

#endif 