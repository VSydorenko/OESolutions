#ifndef _OBJECT_LIST_H__
#define _OBJECT_LIST_H__

#include "metadata/metaObjects/objects/object.h"

//base list class 
class IListDataObject : public IValueTable,
	public ISourceDataObject {
	wxDECLARE_ABSTRACT_CLASS(IListDataObject);
private:

	// implementation of base class virtuals to define model
	virtual unsigned int GetColumnCount() const override {
		return 0;
	}

	virtual wxString GetColumnType(unsigned int col) const override {
		return wxT("string");
	}

	virtual IValueTableColumnCollection* GetColumns() const override {
		return m_dataColumnCollection;
	}

	virtual IValueTableReturnLine* GetRowAt(unsigned int line) {
		if (line > GetRowCount())
			return NULL;
		return new CDataObjectListReturnLine(this, line);
	}

	virtual std::map<meta_identifier_t, CValue>& GetRowData(unsigned int line) = 0;
	virtual unsigned int GetRowCount() const = 0;

public:

	class CDataObjectListColumnCollection : public IValueTable::IValueTableColumnCollection {
		wxDECLARE_DYNAMIC_CLASS(CDataObjectListColumnCollection);
	public:
		class CDataObjectListColumnInfo : public IValueTable::IValueTableColumnCollection::IValueTableColumnInfo {
			wxDECLARE_DYNAMIC_CLASS(CDataObjectListColumnInfo);
			IMetaAttributeObject* m_metaAttribute;
		public:

			virtual unsigned int GetColumnID() const {
				return m_metaAttribute->GetMetaID();
			}
			
			virtual wxString GetColumnName() const { 
				return m_metaAttribute->GetName(); 
			}
			
			virtual wxString GetColumnCaption() const {
				return m_metaAttribute->GetSynonym(); 
			}
			
			virtual CValueTypeDescription* GetColumnTypes() const {
				return m_metaAttribute->GetValueTypeDescription();
			}
			
			virtual int GetColumnWidth() const { 
				return 0; 
			}

			CDataObjectListColumnInfo();
			CDataObjectListColumnInfo(IMetaAttributeObject* metaAttribute);
			virtual ~CDataObjectListColumnInfo();

			virtual wxString GetTypeString() const { 
				return wxT("listColumnInfo"); 
			}
			
			virtual wxString GetString() const { 
				return wxT("listSectionColumnInfo");
			}

			friend CDataObjectListColumnCollection;
		};

	public:

		CDataObjectListColumnCollection();
		CDataObjectListColumnCollection(IListDataObject* ownerTable, IMetaObjectWrapperData *metaObject);
		virtual ~CDataObjectListColumnCollection();

		virtual CValueTypeDescription* GetColumnTypes(unsigned int col) const
		{
			CDataObjectListColumnInfo* columnInfo = m_aColumnInfo.at(col);
			wxASSERT(columnInfo);
			return columnInfo->GetColumnTypes();
		}

		virtual IValueTableColumnInfo* GetColumnInfo(unsigned int idx) const
		{
			if (m_aColumnInfo.size() < idx)
				return NULL;

			auto foundedIt = m_aColumnInfo.begin();
			std::advance(foundedIt, idx);
			return foundedIt->second;
		}

		virtual unsigned int GetColumnCount() const {
			IMetaObjectWrapperData* metaTable = m_ownerTable->GetMetaObject();
			wxASSERT(metaTable);
			auto attributes =
				metaTable->GetGenericAttributes();
			return attributes.size();
		}

		virtual wxString GetTypeString() const { 
			return wxT("listColumn"); 
		}
		
		virtual wxString GetString() const { 
			return wxT("listColumn");
		}

		//array support 
		virtual CValue GetAt(const CValue& cKey);
		virtual void SetAt(const CValue& cKey, CValue& cVal);

		//������ � ����������� 
		virtual bool HasIterator() const { return true; }
		virtual CValue GetItAt(unsigned int idx)
		{
			auto itFounded = m_aColumnInfo.begin();
			std::advance(itFounded, idx);
			return itFounded->second;
		}

		virtual unsigned int GetItSize() const { 
			return GetColumnCount(); 
		}

		friend class IListDataObject;

	protected:

		IListDataObject* m_ownerTable;
		CMethods* m_methods;

		std::map<meta_identifier_t, CDataObjectListColumnInfo*> m_aColumnInfo;

	} *m_dataColumnCollection;

	class CDataObjectListReturnLine : public IValueTableReturnLine {
		wxDECLARE_ABSTRACT_CLASS(CDataObjectListReturnLine);
	public:
		
		virtual unsigned int GetLineTable() const {
			return m_lineTable;
		}
		
		virtual IValueTable* GetOwnerTable() const {
			return m_ownerTable;
		}

		CDataObjectListReturnLine(IListDataObject* ownerTable = NULL, int line = wxNOT_FOUND);
		virtual ~CDataObjectListReturnLine();

		virtual CMethods* GetPMethods() const { PrepareNames(); return m_methods; }; //�������� ������ �� ����� �������� ������� ���� ��������� � �������
		virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������

		virtual wxString GetTypeString() const { 
			return wxT("listValueRow"); 
		}
		
		virtual wxString GetString() const {
			return wxT("listValueRow"); 
		}

		virtual void SetAttribute(attributeArg_t& aParams, CValue& cVal); //��������� ��������
		virtual CValue GetAttribute(attributeArg_t& aParams); //�������� ��������

	protected:
		CMethods* m_methods;
		IListDataObject* m_ownerTable; int m_lineTable;
	};

public:

	virtual bool AutoCreateColumns() const {
		return false;
	}

	virtual bool EditableLine(const wxDataViewItem& item, unsigned int col) const {
		return false;
	}

	//ctor
	IListDataObject(IMetaObjectWrapperData* metaObject = NULL, const form_identifier_t &formType = wxNOT_FOUND);
	virtual ~IListDataObject();

	//****************************************************************************
	//*                               Support model                              *
	//****************************************************************************

	virtual void RefreshModel() = 0;

	//get metadata from object 
	virtual IMetaObjectWrapperData* GetMetaObject() const = 0;

	//get unique identifier 
	virtual CUniqueKey GetGuid() const {
		return m_objGuid;
	};

	//counter
	virtual void IncrRef() { CValue::IncrRef(); }
	virtual void DecrRef() { CValue::DecrRef(); }

	virtual inline bool IsEmpty() const { return false; }

	//Get ref class 
	virtual CLASS_ID GetClassType() const = 0;

	virtual wxString GetTypeString() const = 0;
	virtual wxString GetString() const = 0;

	//operator 
	virtual operator CValue() const {
		return this;
	};

protected:

	Guid m_objGuid;
	CMethods* m_methods;
};

// list without parent  
class CListDataObjectRef : public IListDataObject {
	wxDECLARE_DYNAMIC_CLASS(CListDataObjectRef);
public:

	virtual wxDataViewItem GetLineByGuid(const Guid& guid) const;

	virtual std::map<meta_identifier_t, CValue>& GetRowData(unsigned int line);

	virtual unsigned int GetRowCount() const {
		return m_aObjectValues.size();
	}

	//Constructor
	CListDataObjectRef(IMetaObjectRecordDataRef* metaObject = NULL, const form_identifier_t &formType = wxNOT_FOUND, bool choiceMode = false);

	virtual void GetValueByRow(wxVariant& variant,
		unsigned int row, unsigned int col) const;

	virtual bool GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr& attr) const {
		return true;
	}

	virtual bool SetValueByRow(const wxVariant& variant,
		unsigned int row, unsigned int col) override;

	//****************************************************************************
	//*                               Support model                              *
	//****************************************************************************

	virtual void RefreshModel();

	//support source data 
	virtual CSourceExplorer GetSourceExplorer() const;
	virtual bool GetModel(IValueModel*& tableValue, const meta_identifier_t &id);

	//****************************************************************************
	//*                              Support methods                             *
	//****************************************************************************
	virtual CMethods* GetPMethods() const;                          // �������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                             // ���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t& aParams);       // ����� ������

	//****************************************************************************
	//*                              Override attribute                          *
	//****************************************************************************
	virtual void SetAttribute(attributeArg_t& aParams, CValue& cVal);        //��������� ��������
	virtual CValue GetAttribute(attributeArg_t& aParams);                   //�������� ��������

	//on activate item
	virtual void ActivateItem(CValueForm* srcForm,
		const wxDataViewItem& item, unsigned int col) {
		if (m_choiceMode) {
			ChooseValue(srcForm);
		}
		else {
			EditValue();
		}
	}

	//get metadata from object 
	virtual IMetaObjectWrapperData* GetMetaObject() const {
		return m_metaObject;
	};

	//Get ref class 
	virtual CLASS_ID GetClassType() const;

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

	//support actions
	virtual actionData_t GetActions(const form_identifier_t &formType);
	virtual void ExecuteAction(const action_identifier_t &action, CValueForm* srcForm);

	//events:
	virtual void AddValue(unsigned int before = 0) override;
	virtual void CopyValue() override;
	virtual void EditValue() override;
	virtual void DeleteValue() override;

	virtual void ChooseValue(CValueForm* srcForm);

private:

	bool m_choiceMode;

	IMetaObjectRecordDataRef* m_metaObject;
	std::map<Guid, std::map<meta_identifier_t, CValue>> m_aObjectValues;
};

// list register
class CListRegisterObject : public IListDataObject {
	wxDECLARE_DYNAMIC_CLASS(CListRegisterObject);
public:

	virtual std::map<meta_identifier_t, CValue>& GetRowData(unsigned int line);

	virtual unsigned int GetRowCount() const {
		return m_aObjectValues.size();
	}

	//Constructor
	CListRegisterObject(IMetaObjectRegisterData* metaObject = NULL, const form_identifier_t &formType = wxNOT_FOUND);

	virtual void GetValueByRow(wxVariant& variant,
		unsigned int row, unsigned int col) const;

	virtual bool SetValueByRow(const wxVariant& variant,
		unsigned int row, unsigned int col) override;

	//****************************************************************************
	//*                               Support model                              *
	//****************************************************************************

	virtual void RefreshModel();

	//support source data 
	virtual CSourceExplorer GetSourceExplorer() const;
	virtual bool GetModel(IValueModel*& tableValue, const meta_identifier_t &id);

	//****************************************************************************
	//*                              Support methods                             *
	//****************************************************************************
	virtual CMethods* GetPMethods() const;                          // �������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                             // ���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t& aParams);       // ����� ������

	//****************************************************************************
	//*                              Override attribute                          *
	//****************************************************************************
	virtual void SetAttribute(attributeArg_t& aParams, CValue& cVal);        //��������� ��������
	virtual CValue GetAttribute(attributeArg_t& aParams);                   //�������� ��������

	//on activate item
	virtual void ActivateItem(CValueForm* srcForm,
		const wxDataViewItem& item, unsigned int col) {
		EditValue();
	}

	//get metadata from object 
	virtual IMetaObjectWrapperData* GetMetaObject() const {
		return m_metaObject;
	};

	//Get ref class 
	virtual CLASS_ID GetClassType() const;

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

	//support actions
	virtual actionData_t GetActions(const form_identifier_t &formType);
	virtual void ExecuteAction(const action_identifier_t &action, CValueForm* srcForm);

	//events:
	virtual void AddValue(unsigned int before = 0) override;
	virtual void CopyValue() override;
	virtual void EditValue() override;
	virtual void DeleteValue() override;

private:

	IMetaObjectRegisterData* m_metaObject;
	std::vector<
		std::pair<
		std::map<meta_identifier_t, CValue>,
		std::map<meta_identifier_t, CValue>
		>
	> m_aObjectValues;
};

//base tree class 
class ITreeDataObject : public IValueTree,
	public ISourceDataObject {
	wxDECLARE_ABSTRACT_CLASS(ITreeDataObject);
private:

	// implementation of base class virtuals to define model
	virtual unsigned int GetColumnCount() const override {
		return 0;
	}

	virtual wxString GetColumnType(unsigned int col) const override {
		return wxT("string");
	}

	virtual IValueTableColumnCollection* GetColumns() const override {
		return m_dataColumnCollection;
	}

	virtual IValueTableReturnLine* GetRowAt(unsigned int line) {
		if (line > GetRowCount())
			return NULL;
		return new CDataObjectTreeReturnLine(this, line);
	}

	virtual std::map<meta_identifier_t, CValue>& GetRowData(unsigned int line) = 0;
	virtual unsigned int GetRowCount() const = 0;

public:

	class CDataObjectTreeColumnCollection : public IValueTable::IValueTableColumnCollection {
		wxDECLARE_DYNAMIC_CLASS(CDataObjectListColumnCollection);
	public:
		class CDataObjectTreeColumnInfo : public IValueTable::IValueTableColumnCollection::IValueTableColumnInfo {
			wxDECLARE_DYNAMIC_CLASS(CDataObjectListColumnInfo);
			IMetaAttributeObject* m_metaAttribute;
		public:

			virtual unsigned int GetColumnID() const {
				return m_metaAttribute->GetMetaID();
			}

			virtual wxString GetColumnName() const {
				return m_metaAttribute->GetName();
			}

			virtual wxString GetColumnCaption() const {
				return m_metaAttribute->GetSynonym();
			}

			virtual CValueTypeDescription* GetColumnTypes() const {
				return m_metaAttribute->GetValueTypeDescription();
			}

			virtual int GetColumnWidth() const {
				return 0;
			}

			CDataObjectTreeColumnInfo();
			CDataObjectTreeColumnInfo(IMetaAttributeObject* metaAttribute);
			virtual ~CDataObjectTreeColumnInfo();

			virtual wxString GetTypeString() const {
				return wxT("treeColumnInfo");
			}

			virtual wxString GetString() const {
				return wxT("treeSectionColumnInfo");
			}

			friend CDataObjectTreeColumnCollection;
		};

	public:

		CDataObjectTreeColumnCollection();
		CDataObjectTreeColumnCollection(IListDataObject* ownerTable, IMetaObjectWrapperData* metaObject);
		virtual ~CDataObjectTreeColumnCollection();

		virtual CValueTypeDescription* GetColumnTypes(unsigned int col) const
		{
			CDataObjectTreeColumnInfo* columnInfo = m_aColumnInfo.at(col);
			wxASSERT(columnInfo);
			return columnInfo->GetColumnTypes();
		}

		virtual IValueTableColumnInfo* GetColumnInfo(unsigned int idx) const
		{
			if (m_aColumnInfo.size() < idx)
				return NULL;

			auto foundedIt = m_aColumnInfo.begin();
			std::advance(foundedIt, idx);
			return foundedIt->second;
		}

		virtual unsigned int GetColumnCount() const {
			IMetaObjectWrapperData* metaTable = m_ownerTable->GetMetaObject();
			wxASSERT(metaTable);
			auto attributes =
				metaTable->GetGenericAttributes();
			return attributes.size();
		}

		virtual wxString GetTypeString() const {
			return wxT("treeColumn");
		}

		virtual wxString GetString() const {
			return wxT("treeColumn");
		}

		//array support 
		virtual CValue GetAt(const CValue& cKey);
		virtual void SetAt(const CValue& cKey, CValue& cVal);

		//������ � ����������� 
		virtual bool HasIterator() const { return true; }
		virtual CValue GetItAt(unsigned int idx)
		{
			auto itFounded = m_aColumnInfo.begin();
			std::advance(itFounded, idx);
			return itFounded->second;
		}

		virtual unsigned int GetItSize() const {
			return GetColumnCount();
		}

		friend class IListDataObject;

	protected:

		IListDataObject* m_ownerTable;
		CMethods* m_methods;

		std::map<meta_identifier_t, CDataObjectTreeColumnInfo*> m_aColumnInfo;

	} *m_dataColumnCollection;

	class CDataObjectTreeReturnLine : public IValueTableReturnLine {
		wxDECLARE_ABSTRACT_CLASS(CDataObjectTreeReturnLine);
	public:

		virtual unsigned int GetLineTable() const {
			return m_lineTable;
		}

		virtual IValueTable* GetOwnerTable() const {
			return m_ownerTable;
		}

		CDataObjectTreeReturnLine(ITreeDataObject* ownerTable = NULL, int line = wxNOT_FOUND);
		virtual ~CDataObjectTreeReturnLine();

		virtual CMethods* GetPMethods() const { PrepareNames(); return m_methods; }; //�������� ������ �� ����� �������� ������� ���� ��������� � �������
		virtual void PrepareNames() const;                         //���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������

		virtual wxString GetTypeString() const {
			return wxT("treeValueRow");
		}

		virtual wxString GetString() const {
			return wxT("treeValueRow");
		}

		virtual void SetAttribute(attributeArg_t& aParams, CValue& cVal); //��������� ��������
		virtual CValue GetAttribute(attributeArg_t& aParams); //�������� ��������

	protected:
		CMethods* m_methods;
		IListDataObject* m_ownerTable; int m_lineTable;
	};

public:

	virtual bool AutoCreateColumns() const {
		return false;
	}

	virtual bool EditableLine(const wxDataViewItem& item, unsigned int col) const {
		return false;
	}

	//ctor
	ITreeDataObject(IMetaObjectWrapperData* metaObject = NULL, const form_identifier_t& formType = wxNOT_FOUND);
	virtual ~ITreeDataObject();

	//****************************************************************************
	//*                               Support model                              *
	//****************************************************************************

	virtual void RefreshModel() = 0;

	//get metadata from object 
	virtual IMetaObjectWrapperData* GetMetaObject() const = 0;

	//get unique identifier 
	virtual CUniqueKey GetGuid() const {
		return m_objGuid;
	};

	//counter
	virtual void IncrRef() { CValue::IncrRef(); }
	virtual void DecrRef() { CValue::DecrRef(); }

	virtual inline bool IsEmpty() const { return false; }

	//Get ref class 
	virtual CLASS_ID GetClassType() const = 0;

	virtual wxString GetTypeString() const = 0;
	virtual wxString GetString() const = 0;

	//operator 
	virtual operator CValue() const {
		return this;
	};

protected:

	Guid m_objGuid;
	CMethods* m_methods;
};

enum {
	LIST_GROUP,
	LIST_ITEM,
};

// tree with parent or only parent 
class CListDataObjectGroupRef : public IListDataObject {
	wxDECLARE_DYNAMIC_CLASS(CListDataObjectRef);
public:

	virtual wxDataViewItem GetLineByGuid(const Guid& guid) const;
	virtual std::map<meta_identifier_t, CValue>& GetRowData(unsigned int line);

	virtual unsigned int GetRowCount() const {
		return m_aObjectValues.size();
	}

	//Constructor
	CListDataObjectGroupRef(IMetaObjectRecordDataGroupMutableRef* metaObject = NULL,
		const form_identifier_t& formType = wxNOT_FOUND, int listMode = LIST_ITEM, bool choiceMode = false);

	virtual void GetValueByRow(wxVariant& variant,
		unsigned int row, unsigned int col) const;

	virtual bool SetValueByRow(const wxVariant& variant,
		unsigned int row, unsigned int col) override;

	//****************************************************************************
	//*                               Support model                              *
	//****************************************************************************

	virtual void RefreshModel();

	//support source data 
	virtual CSourceExplorer GetSourceExplorer() const;
	virtual bool GetModel(IValueModel*& tableValue, const meta_identifier_t& id);

	//****************************************************************************
	//*                              Support methods                             *
	//****************************************************************************
	virtual CMethods* GetPMethods() const;                          // �������� ������ �� ����� �������� ������� ���� ��������� � �������
	virtual void PrepareNames() const;                             // ���� ����� ������������� ���������� ��� ������������� ���� ��������� � �������
	virtual CValue Method(methodArg_t& aParams);       // ����� ������

	//****************************************************************************
	//*                              Override attribute                          *
	//****************************************************************************
	virtual void SetAttribute(attributeArg_t& aParams, CValue& cVal);        //��������� ��������
	virtual CValue GetAttribute(attributeArg_t& aParams);                   //�������� ��������

	//on activate item
	virtual void ActivateItem(CValueForm* srcForm,
		const wxDataViewItem& item, unsigned int col) {
		if (m_choiceMode) {
			ChooseValue(srcForm);
		}
		else {
			EditValue();
		}
	}

	//get metadata from object 
	virtual IMetaObjectRecordDataGroupMutableRef* GetMetaObject() const {
		return m_metaObject;
	};

	//Get ref class 
	virtual CLASS_ID GetClassType() const;

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

	//support actions
	virtual actionData_t GetActions(const form_identifier_t& formType);
	virtual void ExecuteAction(const action_identifier_t& action, CValueForm* srcForm);

	//events:
	virtual void AddValue(unsigned int before = 0) override;
	virtual void CopyValue() override;
	virtual void EditValue() override;
	virtual void DeleteValue() override;

	virtual void ChooseValue(CValueForm* srcForm);

private:

	bool m_choiceMode;

	IMetaObjectRecordDataGroupMutableRef* m_metaObject;
	std::map<Guid, std::map<meta_identifier_t, CValue>> m_aObjectValues;
};

#endif 