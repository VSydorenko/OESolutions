////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : enumeration metadata
////////////////////////////////////////////////////////////////////////////

#include "enumeration.h"
#include "metadata/metadata.h"
#include "list/objectList.h"

#define managerModule wxT("managerModule")

wxIMPLEMENT_DYNAMIC_CLASS(CMetaObjectEnumeration, IMetaObjectRecordDataRef)

//********************************************************************************************
//*                                      metadata                                            *
//********************************************************************************************

CMetaObjectEnumeration::CMetaObjectEnumeration() : IMetaObjectRecordDataRef(),
m_defaultFormList(wxNOT_FOUND), m_defaultFormSelect(wxNOT_FOUND)
{
	PropertyContainer* categoryForm = IObjectBase::CreatePropertyContainer("DefaultForms");
	categoryForm->AddProperty("default_list", PropertyType::PT_OPTION, &CMetaObjectEnumeration::GetFormList);
	categoryForm->AddProperty("default_select", PropertyType::PT_OPTION, &CMetaObjectEnumeration::GetFormSelect);

	m_category->AddCategory(categoryForm);

	//create default attributes
	m_attributeOrder = CMetaDefaultAttributeObject::CreateNumber(wxT("order"), _("Order"), wxEmptyString, 6, true);
	m_attributeOrder->SetClsid(g_metaDefaultAttributeCLSID);

	//set child/parent
	m_attributeOrder->SetParent(this);
	AddChild(m_attributeOrder);

	m_moduleManager = new CMetaManagerModuleObject(managerModule);
	m_moduleManager->SetClsid(g_metaManagerCLSID);

	//set child/parent
	m_moduleManager->SetParent(this);
	AddChild(m_moduleManager);
}

CMetaObjectEnumeration::~CMetaObjectEnumeration()
{
	wxDELETE(m_attributeOrder);
	wxDELETE(m_moduleManager);
}

CMetaFormObject* CMetaObjectEnumeration::GetDefaultFormByID(const form_identifier_t &id)
{
	if (id == eFormList
		&& m_defaultFormList != wxNOT_FOUND) {
		for (auto obj : GetObjectForms()) {
			if (m_defaultFormList == obj->GetMetaID()) {
				return obj;
			}
		}
	}
	else if (id == eFormSelect
		&& m_defaultFormSelect != wxNOT_FOUND) {
		for (auto obj : GetObjectForms()) {
			if (m_defaultFormSelect == obj->GetMetaID()) {
				return obj;
			}
		}
	}

	return NULL;
}

ISourceDataObject* CMetaObjectEnumeration::CreateObjectData(IMetaFormObject* metaObject)
{
	switch (metaObject->GetTypeForm())
	{
	case eFormList: return new CListDataObjectRef(this, metaObject->GetTypeForm()); break;
	case eFormSelect: return new CListDataObjectRef(this, metaObject->GetTypeForm(), true); break;
	}

	return NULL;
}

#include "frontend/visualView/controls/form.h"
#include "utils/stringUtils.h"

CValueForm* CMetaObjectEnumeration::GetObjectForm(const wxString& formName, IValueFrame* ownerControl, const CUniqueKey& formGuid)
{
	return NULL;
}

CValueForm* CMetaObjectEnumeration::GetListForm(const wxString& formName, IValueFrame* ownerControl, const CUniqueKey& formGuid)
{
	CMetaFormObject* defList = NULL;

	if (!formName.IsEmpty()) {
		for (auto metaForm : GetObjectForms()) {
			if (CMetaObjectEnumeration::eFormList == metaForm->GetTypeForm()
				&& StringUtils::CompareString(formName, metaForm->GetName())) {
				defList = metaForm; break;
			}
		}
		wxASSERT(defList);
	}
	else {
		defList = GetDefaultFormByID(CMetaObjectEnumeration::eFormList);
	}

	if (defList == NULL) {
		CValueForm* valueForm = new CValueForm();
		valueForm->InitializeForm(ownerControl, NULL,
			new CListDataObjectRef(this, CMetaObjectEnumeration::eFormList), formGuid
		);
		valueForm->ReadProperty();
		valueForm->BuildForm(CMetaObjectEnumeration::eFormList);
		return valueForm;
	}

	return defList->GenerateFormAndRun(ownerControl,
		new CListDataObjectRef(this, defList->GetTypeForm()), formGuid
	);
}

CValueForm* CMetaObjectEnumeration::GetSelectForm(const wxString& formName, IValueFrame* ownerControl, const CUniqueKey& formGuid)
{
	CMetaFormObject* defList = NULL;

	if (!formName.IsEmpty()) {
		for (auto metaForm : GetObjectForms()) {
			if (CMetaObjectEnumeration::eFormSelect == metaForm->GetTypeForm()
				&& StringUtils::CompareString(formName, metaForm->GetName())) {
				defList = metaForm; break;
			}
		}
		wxASSERT(defList);
	}
	else {
		defList = GetDefaultFormByID(CMetaObjectEnumeration::eFormSelect);
	}

	if (defList == NULL) {
		CValueForm* valueForm = new CValueForm();
		valueForm->InitializeForm(ownerControl, NULL,
			new CListDataObjectRef(this, CMetaObjectEnumeration::eFormSelect, true), formGuid
		);
		valueForm->ReadProperty();
		valueForm->BuildForm(CMetaObjectEnumeration::eFormSelect);
		return valueForm;
	}

	return defList->GenerateFormAndRun(ownerControl,
		new CListDataObjectRef(this, defList->GetTypeForm(), true), formGuid
	);
}

OptionList* CMetaObjectEnumeration::GetFormList(Property*)
{
	OptionList* optlist = new OptionList();
	optlist->AddOption(_("<not selected>"), wxNOT_FOUND);

	for (auto formObject : GetObjectForms()) {
		if (eFormList == formObject->GetTypeForm()) {
			optlist->AddOption(formObject->GetName(), formObject->GetMetaID());
		}
	}

	return optlist;
}

OptionList* CMetaObjectEnumeration::GetFormSelect(Property*)
{
	OptionList* optlist = new OptionList();
	optlist->AddOption(_("<not selected>"), wxNOT_FOUND);

	for (auto formObject : GetObjectForms()) {
		if (eFormSelect == formObject->GetTypeForm()) {
			optlist->AddOption(formObject->GetName(), formObject->GetMetaID());
		}
	}

	return optlist;
}

wxString CMetaObjectEnumeration::GetDescription(const IObjectValueInfo* objValue) const
{
	for (auto eEnum : GetObjectEnums()) {
		if (objValue->GetGuid() == eEnum->GetGuid()) {
			return eEnum->GetSynonym();
		}
	}

	return wxEmptyString;
}

//***************************************************************************
//*                       Save & load metadata                              *
//***************************************************************************

bool CMetaObjectEnumeration::LoadData(CMemoryReader& dataReader)
{
	//load default attributes:
	m_attributeOrder->LoadMeta(dataReader);

	//Load object module
	m_moduleManager->LoadMeta(dataReader);

	//save default form 
	m_defaultFormList = dataReader.r_u32();
	m_defaultFormSelect = dataReader.r_u32();

	return IMetaObjectRecordDataRef::LoadData(dataReader);
}

bool CMetaObjectEnumeration::SaveData(CMemoryWriter& dataWritter)
{
	//save default attributes:
	m_attributeOrder->SaveMeta(dataWritter);

	//Save object module
	m_moduleManager->SaveMeta(dataWritter);

	//save default form 
	dataWritter.w_u32(m_defaultFormList);
	dataWritter.w_u32(m_defaultFormSelect);

	//create or update table:
	return IMetaObjectRecordDataRef::SaveData(dataWritter);
}

//***********************************************************************
//*                           read & save events                        *
//***********************************************************************

bool CMetaObjectEnumeration::OnCreateMetaObject(IMetadata* metaData)
{
	if (!IMetaObjectRecordDataRef::OnCreateMetaObject(metaData))
		return false;

	return m_attributeOrder->OnCreateMetaObject(metaData) &&
		m_moduleManager->OnCreateMetaObject(metaData);
}

bool CMetaObjectEnumeration::OnLoadMetaObject(IMetadata* metaData)
{
	if (!m_attributeOrder->OnLoadMetaObject(metaData))
		return false;

	if (!m_moduleManager->OnLoadMetaObject(metaData))
		return false;

	return IMetaObjectRecordDataRef::OnLoadMetaObject(metaData);
}

bool CMetaObjectEnumeration::OnSaveMetaObject()
{
	if (!m_attributeOrder->OnSaveMetaObject())
		return false;

	if (!m_moduleManager->OnSaveMetaObject())
		return false;

	return IMetaObjectRecordDataRef::OnSaveMetaObject();
}

bool CMetaObjectEnumeration::OnDeleteMetaObject()
{
	if (!m_attributeOrder->OnDeleteMetaObject())
		return false;

	if (!m_moduleManager->OnDeleteMetaObject())
		return false;

	return IMetaObjectRecordDataRef::OnDeleteMetaObject();
}

bool CMetaObjectEnumeration::OnReloadMetaObject()
{
	return true;
}

bool CMetaObjectEnumeration::OnBeforeRunMetaObject(int flags)
{
	if (!m_attributeOrder->OnBeforeRunMetaObject(flags))
		return false;

	if (!m_moduleManager->OnBeforeRunMetaObject(flags))
		return false;

	return IMetaObjectRecordDataRef::OnBeforeRunMetaObject(flags);
}

bool CMetaObjectEnumeration::OnAfterCloseMetaObject()
{
	if (!m_attributeOrder->OnAfterCloseMetaObject())
		return false;

	if (!m_moduleManager->OnAfterCloseMetaObject())
		return false;

	return IMetaObjectRecordDataRef::OnAfterCloseMetaObject();
}

//***********************************************************************
//*                             form events                             *
//***********************************************************************

void CMetaObjectEnumeration::OnCreateMetaForm(IMetaFormObject* metaForm)
{
	if (metaForm->GetTypeForm() == CMetaObjectEnumeration::eFormList
		&& m_defaultFormList == wxNOT_FOUND)
	{
		m_defaultFormList = metaForm->GetMetaID();
	}
	else if (metaForm->GetTypeForm() == CMetaObjectEnumeration::eFormSelect
		&& m_defaultFormSelect == wxNOT_FOUND)
	{
		m_defaultFormSelect = metaForm->GetMetaID();
	}
}

void CMetaObjectEnumeration::OnRemoveMetaForm(IMetaFormObject* metaForm)
{
	if (metaForm->GetTypeForm() == CMetaObjectEnumeration::eFormList
		&& m_defaultFormList == metaForm->GetMetaID())
	{
		m_defaultFormList = wxNOT_FOUND;
	}
	else if (metaForm->GetTypeForm() == CMetaObjectEnumeration::eFormSelect
		&& m_defaultFormSelect == metaForm->GetMetaID())
	{
		m_defaultFormSelect = wxNOT_FOUND;
	}
}

std::vector<IMetaAttributeObject*> CMetaObjectEnumeration::GetDefaultAttributes() const
{
	std::vector<IMetaAttributeObject*> attributes;
	attributes.push_back(m_attributeReference);
	attributes.push_back(m_attributeOrder);
	return attributes;
}

std::vector<IMetaAttributeObject*> CMetaObjectEnumeration::GetSearchedAttributes() const
{
	return std::vector<IMetaAttributeObject*>();
}

//***********************************************************************
//*                           read & save property                      *
//***********************************************************************

void CMetaObjectEnumeration::ReadProperty()
{
	IMetaObjectRecordData::ReadProperty();

	IObjectBase::SetPropertyValue("default_list", m_defaultFormList);
	IObjectBase::SetPropertyValue("default_select", m_defaultFormSelect);
}

void CMetaObjectEnumeration::SaveProperty()
{
	IMetaObjectRecordData::SaveProperty();

	IObjectBase::GetPropertyValue("default_list", m_defaultFormList);
	IObjectBase::GetPropertyValue("default_select", m_defaultFormSelect);
}

//***********************************************************************
//*                       Register in runtime                           *
//***********************************************************************

METADATA_REGISTER(CMetaObjectEnumeration, "enumeration", g_metaEnumerationCLSID);