////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : table model information
////////////////////////////////////////////////////////////////////////////

#include "tableInfo.h"

static int my_sort(int* v1, int* v2)
{
	return *v2 - *v1;
}

wxIMPLEMENT_ABSTRACT_CLASS(IValueTable, CValue);
wxIMPLEMENT_ABSTRACT_CLASS(IValueTable::IValueTableColumnCollection, CValue);
wxIMPLEMENT_ABSTRACT_CLASS(IValueTable::IValueTableColumnCollection::IValueTableColumnInfo, CValue);
wxIMPLEMENT_ABSTRACT_CLASS(IValueTable::IValueTableReturnLine, CValue);

IValueTable::IValueTable(unsigned int initial_size)
	: CValue(eValueTypes::TYPE_VALUE), wxDataViewModel(), m_srcNotifier(NULL)
{
	// IDs are ordered until an item gets deleted or inserted
	m_ordered = true;

	// build initial index
	unsigned int i;

	for (i = 1; i < initial_size + 1; i++)
		m_hash.Add(wxDataViewItem(wxUIntToPtr(i)));

	m_nextFreeID = initial_size + 1;
}

void IValueTable::Reset(unsigned int new_size)
{
	/* wxDataViewModel:: */ BeforeReset();

	m_hash.Clear();

	// IDs are ordered until an item gets deleted or inserted
	m_ordered = true;

	// build initial index
	unsigned int i;
	for (i = 1; i < new_size + 1; i++)
		m_hash.Add(wxDataViewItem(wxUIntToPtr(i)));

	m_nextFreeID = new_size + 1;

	/* wxDataViewModel:: */ AfterReset();
}

wxDataViewItem IValueTable::GetSelection() const
{
	if (m_srcNotifier == NULL) {
		return wxDataViewItem(0);
	}
	return m_srcNotifier->GetSelection();
}

void IValueTable::RowValueStartEdit(const wxDataViewItem& item, unsigned int col)
{
	if (m_srcNotifier == NULL) {
		return;
	}
	m_srcNotifier->StartEditing(item, col);
}

enum
{
	eAddValue = 1,
	eCopyValue,
	eEditValue,
	eDeleteValue
};

IValueTable::actionData_t IValueTable::GetActions(const form_identifier_t &formType)
{
	actionData_t action(this);
	action.AddAction("add", _("Add"), eAddValue);
	action.AddAction("copy", _("Copy"), eCopyValue);
	action.AddAction("edit", _("Edit"), eEditValue);
	action.AddAction("delete", _("Delete"), eDeleteValue);
	return action;
}

void IValueTable::ExecuteAction(const action_identifier_t &action, CValueForm* srcForm)
{
	switch (action)
	{
	case eAddValue: AddValue(); break;
	case eCopyValue: CopyValue(); break;
	case eEditValue: EditValue(); break;
	case eDeleteValue: DeleteValue(); break;
	}
}

void IValueTable::RowPrepended()
{
	m_ordered = false;

	unsigned int id = m_nextFreeID;
	m_nextFreeID++;

	wxDataViewItem item(wxUIntToPtr(id));
	m_hash.Insert(item, 0);
	ItemAdded(wxDataViewItem(0), item);

	/*if (m_srcNotifier != NULL) {
		m_srcNotifier->Select(item);
	}*/
}

void IValueTable::RowInserted(unsigned int before)
{
	m_ordered = false;

	unsigned int id = m_nextFreeID;
	m_nextFreeID++;

	wxDataViewItem item(wxUIntToPtr(id));
	m_hash.Insert(item, before);
	ItemAdded(wxDataViewItem(0), item);

	/*if (m_srcNotifier != NULL) {
		m_srcNotifier->Select(item);
	}*/
}

void IValueTable::RowAppended()
{
	unsigned int id = m_nextFreeID;
	m_nextFreeID++;

	wxDataViewItem item(wxUIntToPtr(id));
	m_hash.Add(item);
	ItemAdded(wxDataViewItem(0), item);

	/*if (m_srcNotifier != NULL) {
		m_srcNotifier->Select(item);
	}*/
}

void IValueTable::RowDeleted(unsigned int row)
{
	m_ordered = false;

	wxDataViewItem item(m_hash[row]);
	m_hash.RemoveAt(row);
	/* wxDataViewModel:: */ ItemDeleted(wxDataViewItem(0), item);
}

void IValueTable::RowsDeleted(const wxArrayInt& rows)
{
	m_ordered = false;

	wxDataViewItemArray array;
	unsigned int i;
	for (i = 0; i < rows.GetCount(); i++)
	{
		wxDataViewItem item(m_hash[rows[i]]);
		array.Add(item);
	}

	wxArrayInt sorted = rows;
	sorted.Sort(my_sort);
	for (i = 0; i < sorted.GetCount(); i++)
		m_hash.RemoveAt(sorted[i]);

	/* wxDataViewModel:: */ ItemsDeleted(wxDataViewItem(0), array);
}

void IValueTable::RowChanged(unsigned int row)
{
	/* wxDataViewModel:: */ ItemChanged(GetItem(row));
}

void IValueTable::RowValueChanged(unsigned int row, unsigned int col)
{
	/* wxDataViewModel:: */ ValueChanged(GetItem(row), col);
}

unsigned int IValueTable::GetRow(const wxDataViewItem& item) const
{
	if (m_ordered)
		return wxPtrToUInt(item.GetID()) - 1;

	// assert for not found
	return (unsigned int)m_hash.Index(item);
}

wxDataViewItem IValueTable::GetItem(unsigned int row) const
{
	wxASSERT(row < m_hash.GetCount());
	return wxDataViewItem(m_hash[row]);
}

unsigned int IValueTable::GetChildren(const wxDataViewItem& item, wxDataViewItemArray& children) const
{
	if (item.IsOk())
		return 0;

	children = m_hash;

	return m_hash.GetCount();
}

///////////////////////////////////////////////////////////////////////////////////////s

bool IValueTable::IValueTableColumnCollection::HasColumnID(unsigned int col_id)
{
	return GetColumnByID(col_id) != NULL;
}

///////////////////////////////////////////////////////////////////////////////////////

#include "compiler/methods.h"

IValueTable::IValueTableColumnCollection::IValueTableColumnInfo::IValueTableColumnInfo() : CValue(eValueTypes::TYPE_VALUE, true)
{
	m_methods = new CMethods();
}

IValueTable::IValueTableColumnCollection::IValueTableColumnInfo::~IValueTableColumnInfo()
{
	wxDELETE(m_methods);
}

enum
{
	enColumnName,
	enColumnTypes,
	enColumnCaption,
	enColumnWidth
};

void IValueTable::IValueTableColumnCollection::IValueTableColumnInfo::PrepareNames() const
{
	std::vector<SEng> aAttributes;

	{
		SEng aAttribute;
		aAttribute.sName = wxT("name");
		aAttribute.sSynonym = wxT("default");
		aAttributes.push_back(aAttribute);
	}
	{
		SEng aAttribute;
		aAttribute.sName = wxT("types");
		aAttribute.sSynonym = wxT("default");
		aAttributes.push_back(aAttribute);
	}
	{
		SEng aAttribute;
		aAttribute.sName = wxT("caption");
		aAttribute.sSynonym = wxT("default");
		aAttributes.push_back(aAttribute);
	}
	{
		SEng aAttribute;
		aAttribute.sName = wxT("width");
		aAttribute.sSynonym = wxT("default");
		aAttributes.push_back(aAttribute);
	}

	m_methods->PrepareAttributes(aAttributes.data(), aAttributes.size());
}

CValue IValueTable::IValueTableColumnCollection::IValueTableColumnInfo::GetAttribute(attributeArg_t& aParams)
{
	switch (aParams.GetIndex())
	{
	case enColumnName: return GetColumnName();
	case enColumnTypes: return GetColumnTypes();
	case enColumnCaption: return GetColumnCaption();
	case enColumnWidth: return GetColumnWidth();
	}

	return CValue();
}


IValueTable::IValueTableColumnCollection::IValueTableColumnInfo* IValueTable::IValueTableColumnCollection::GetColumnByID(unsigned int col_id)
{
	for (unsigned int idx = 0; idx < GetColumnCount(); idx++)
	{
		IValueTableColumnInfo* columnInfo = GetColumnInfo(idx);
		wxASSERT(columnInfo);

		if (col_id == columnInfo->GetColumnID())
			return columnInfo;
	}

	return NULL;
}

#include "utils/stringUtils.h"

IValueTable::IValueTableColumnCollection::IValueTableColumnInfo* IValueTable::IValueTableColumnCollection::GetColumnByName(const wxString& colName)
{
	for (unsigned int idx = 0; idx < GetColumnCount(); idx++)
	{
		IValueTableColumnInfo* columnInfo = GetColumnInfo(idx);
		wxASSERT(columnInfo);
		if (StringUtils::CompareString(colName, columnInfo->GetColumnName()))
			return columnInfo;
	}

	return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

IValueTable::IValueTableReturnLine::IValueTableReturnLine() : CValue(eValueTypes::TYPE_VALUE, true) {}

IValueTable::IValueTableReturnLine::~IValueTableReturnLine() { }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool CDataViewCtrl::AssociateModel(IValueTable* model)
{
	if (model) {
		m_genNotitfier = new CTableModelNotifier(this);
		model->AppendNotifier(m_genNotitfier);
	}
	else {
		// Our previous notifier has either been already deleted when the
		// previous model was DecRef()'d in the base class AssociateModel() or
		// is not associated with us any more because if the model is still
		// alive, it's not used by this control.
		m_genNotitfier = NULL;
	}

	return wxDataViewCtrl::AssociateModel(model);
}

void CDataViewCtrl::Select(const wxDataViewItem& item)
{
	wxDataViewCtrl::Select(item);
}

void CTableModelNotifier::Select(const wxDataViewItem& sel) const
{
	m_mainWindow->Select(sel);
}

wxDataViewItem CTableModelNotifier::GetSelection() const
{
	return m_mainWindow->GetSelection();
}

int CTableModelNotifier::GetSelections(wxDataViewItemArray& sel) const
{
	return m_mainWindow->GetSelections(sel);
}

void CTableModelNotifier::StartEditing(const wxDataViewItem& item, unsigned int col) const
{
	int viewColumn = m_mainWindow->GetModelColumnIndex(col);
	if (viewColumn != wxNOT_FOUND) {
		m_mainWindow->EditItem(item,
			m_mainWindow->GetColumn(viewColumn)
		);
	}
	else if (col == 0 && m_mainWindow->GetColumnCount() > 0) {
		m_mainWindow->EditItem(item,
			m_mainWindow->GetColumnAt(0)
		);
	}
}