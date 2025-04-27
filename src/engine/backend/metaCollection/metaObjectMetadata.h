#ifndef __METAOBJECT_METADATA_H__
#define __METAOBJECT_METADATA_H__

#include "metaObject.h"
#include "metaObjectMetadataEnum.h"

//*****************************************************************************************
//*                                  metaData object                                      *
//*****************************************************************************************

#define configurationDefaultName _("�onfiguration")

///////////////////////////////////////////////////////////////////////////
class BACKEND_API CMetaObjectModule;
///////////////////////////////////////////////////////////////////////////

class BACKEND_API CMetaObjectConfiguration : public IMetaObject {
	wxDECLARE_DYNAMIC_CLASS(CMetaObjectConfiguration);
private:
	Role* m_roleAdministration = IMetaObject::CreateRole("administration", _("administration"));
	Role* m_roleDataAdministration = IMetaObject::CreateRole("dataAdministration", _("data administration"));
	Role* m_roleUpdateDatabaseConfiguration = IMetaObject::CreateRole("updateDatabaseConfiguration", _("update database configuration"));
	Role* m_roleActiveUsers = IMetaObject::CreateRole("activeUsers", _("active users"));
	Role* m_roleExclusiveMode = IMetaObject::CreateRole("exclusiveMode", _("exclusive mode"));
	Role* m_roleModeAllFunction = IMetaObject::CreateRole("modeAllFunctions", _("mode \"All functions\""));
protected:
	
	enum
	{
		ID_METATREE_OPEN_INIT_MODULE = 19000,
	};

	CPropertyCategory* m_compatibilityCategory = IPropertyObject::CreatePropertyCategory(wxT("compatibility"), _("compatibility"));
	CPropertyEnum<CValueEnumVersion>* m_propertyVersion = IPropertyObject::CreateProperty<CPropertyEnum<CValueEnumVersion>>(m_compatibilityCategory, wxT("version"), _("version"), version_oes_last);

public:

	virtual bool FilterChild(const class_identifier_t &clsid) const {

		if (
			clsid == g_metaCommonModuleCLSID ||
			clsid == g_metaCommonFormCLSID ||
			clsid == g_metaCommonTemplateCLSID ||
			clsid == g_metaRoleCLSID ||
			clsid == g_metaInterfaceCLSID ||
			clsid == g_metaConstantCLSID ||
			clsid == g_metaCatalogCLSID ||
			clsid == g_metaDocumentCLSID ||
			clsid == g_metaEnumerationCLSID ||
			clsid == g_metaDataProcessorCLSID ||
			clsid == g_metaReportCLSID ||
			clsid == g_metaInformationRegisterCLSID ||
			clsid == g_metaAccumulationRegisterCLSID
			)
			return true; 

		return false;
	}

	virtual void SetVersion(const version_identifier_t& version) {
		m_propertyVersion->SetValue(static_cast<eProgramVersion>(version));
	}

	version_identifier_t GetVersion() const {
		return m_propertyVersion->GetValueAsInteger();
	}

	CMetaObjectConfiguration();
	virtual ~CMetaObjectConfiguration();

	virtual wxString GetFullName() const {
		return configurationDefaultName;
	}

	virtual wxString GetModuleName() const {
		return configurationDefaultName;
	}

	//support icons
	virtual wxIcon GetIcon() const;
	static wxIcon GetIconGroup();

	//events
	virtual bool OnCreateMetaObject(IMetaData* metaData);
	virtual bool OnLoadMetaObject(IMetaData* metaData);
	virtual bool OnSaveMetaObject();
	virtual bool OnDeleteMetaObject();

	//module manager is started or exit 
	virtual bool OnBeforeRunMetaObject(int flags);
	virtual bool OnAfterCloseMetaObject();

	//prepare menu for item
	virtual bool PrepareContextMenu(wxMenu* defaultMenu);
	virtual void ProcessCommand(unsigned int id);

	//create function 
	static bool ExecuteSystemSQLCommand();

public:

	virtual CMetaObjectModule* GetModuleObject() const {
		return m_commonModule;
	}

protected:

	//load & save metaData from DB 
	virtual bool LoadData(CMemoryReader& reader);
	virtual bool SaveData(CMemoryWriter& writer = CMemoryWriter());

private:

	CMetaObjectModule* m_commonModule = IMetaObject::CreateMetaObjectAndSetParent<CMetaObjectModule>(wxT("configurationModule"));
};

#endif 