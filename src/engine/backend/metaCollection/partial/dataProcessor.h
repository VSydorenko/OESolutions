#ifndef __DATA_PROCESSOR_H__
#define __DATA_PROCESSOR_H__

#include "commonObject.h"

class CMetaObjectDataProcessor : public IMetaObjectRecordDataExt {
    wxDECLARE_DYNAMIC_CLASS(CMetaObjectDataProcessor);
protected:

    enum
    {
        ID_METATREE_OPEN_MODULE = 19000,
        ID_METATREE_OPEN_MANAGER = 19001,
    };

    CMetaObjectModule* m_moduleObject = IMetaObjectSourceData::CreateMetaObjectAndSetParent<CMetaObjectModule>(wxT("objectModule"));
    CMetaObjectCommonModule* m_moduleManager = IMetaObjectSourceData::CreateMetaObjectAndSetParent<CMetaObjectManagerModule>(wxT("managerModule"));

public:

    enum
    {
        eFormDataProcessor = 1,
    };

    virtual CFormTypeList GetFormType() const override {
        CFormTypeList formList;
        formList.AppendItem(wxT("formDataProcessor"), _("Form data processor"), eFormDataProcessor);
        return formList;
    }

protected:

    CPropertyCategory* m_categoryForm = IPropertyObject::CreatePropertyCategory(wxT("defaultForms"), _("default forms"));
    CPropertyList* m_propertyDefFormObject = IPropertyObject::CreateProperty<CPropertyList>(m_categoryForm, wxT("defaultFormObject"), _("default object"), &CMetaObjectDataProcessor::GetFormObject, wxNOT_FOUND);

private:
    bool GetFormObject(CPropertyList* prop);
public:

    form_identifier_t GetDefFormObject() const {
        return m_propertyDefFormObject->GetValueAsInteger();
    }

    void SetDefFormObject(const form_identifier_t& id) const {
        m_propertyDefFormObject->SetValue(id);
    }

    CMetaObjectDataProcessor(int objMode = METAOBJECT_NORMAL);
    virtual ~CMetaObjectDataProcessor();

    //support icons
    virtual wxIcon GetIcon() const;
    static wxIcon GetIconGroup();

    //events: 
    virtual bool OnCreateMetaObject(IMetaData* metaData);
    virtual bool OnLoadMetaObject(IMetaData* metaData);
    virtual bool OnSaveMetaObject();
    virtual bool OnDeleteMetaObject();

    //for designer 
    virtual bool OnReloadMetaObject();

    //module manager is started or exit 
    virtual bool OnBeforeRunMetaObject(int flags);
    virtual bool OnAfterRunMetaObject(int flags);

    virtual bool OnBeforeCloseMetaObject();
    virtual bool OnAfterCloseMetaObject();

    //form events 
    virtual void OnCreateFormObject(IMetaObjectForm* metaForm);
    virtual void OnRemoveMetaForm(IMetaObjectForm* metaForm);

    //create associate value 
    virtual CMetaObjectForm* GetDefaultFormByID(const form_identifier_t& id);

    //create object data with metaForm
    virtual ISourceDataObject* CreateObjectData(IMetaObjectForm* metaObject);

    //create form with data 
    virtual IBackendValueForm* CreateObjectForm(IMetaObjectForm* metaForm) override {
        return metaForm->GenerateFormAndRun(
            nullptr, CreateObjectData(metaForm)
        );
    }

    //suppot form
    virtual IBackendValueForm* GetObjectForm(const wxString& formName = wxEmptyString, IBackendControlFrame* ownerControl = nullptr, const CUniqueKey& formGuid = wxNullGuid);

    //get module object in compose object 
    virtual CMetaObjectModule* GetModuleObject() const {
        return m_moduleObject;
    }

    virtual CMetaObjectCommonModule* GetModuleManager() const {
        return m_moduleManager;
    }

    //prepare menu for item
    virtual bool PrepareContextMenu(wxMenu* defaultMenu);
    virtual void ProcessCommand(unsigned int id);

protected:

    //create empty object
    virtual IRecordDataObjectExt* CreateObjectExtValue();  //create object 

    //load & save metaData from DB 
    virtual bool LoadData(CMemoryReader& reader);
    virtual bool SaveData(CMemoryWriter& writer = CMemoryWriter());

protected:
    friend class IMetaData;
    friend class CRecordDataObjectDataProcessor;
};

#define default_meta_id 10 //for dataProcessors

class CMetaObjectDataProcessorExternal : public CMetaObjectDataProcessor {
    wxDECLARE_DYNAMIC_CLASS(CMetaObjectDataProcessorExternal);
public:
    CMetaObjectDataProcessorExternal() : CMetaObjectDataProcessor(METAOBJECT_EXTERNAL) {
        m_metaId = default_meta_id;
    }
};

//********************************************************************************************
//*                                      Object                                              *
//********************************************************************************************

class CRecordDataObjectDataProcessor : public IRecordDataObjectExt {
    CRecordDataObjectDataProcessor(CMetaObjectDataProcessor* metaObject);
    CRecordDataObjectDataProcessor(const CRecordDataObjectDataProcessor& source);
public:

    //support show 
    virtual void ShowFormValue(const wxString& formName = wxEmptyString, IBackendControlFrame* ownerControl = nullptr);
    virtual IBackendValueForm* GetFormValue(const wxString& formName = wxEmptyString, IBackendControlFrame* ownerControl = nullptr);

    //support actionData
    virtual CActionCollection GetActionCollection(const form_identifier_t& formType);
    virtual void ExecuteAction(const action_identifier_t& lNumAction, IBackendValueForm* srcForm);

protected:
    friend class IMetaData;
    friend class CMetaObjectDataProcessor;
    friend class CModuleManagerExternalDataProcessor;
};

#endif