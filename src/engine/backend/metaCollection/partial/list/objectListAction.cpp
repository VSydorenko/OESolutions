////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : list actionData 
////////////////////////////////////////////////////////////////////////////

#include "objectList.h"

enum
{
	eChooseValue = 25,
	eMarkAsDelete = 26,
	eAddFolder = 27,
};

CListDataObjectEnumRef::CActionCollection CListDataObjectEnumRef::GetActionCollection(const form_identifier_t& formType)
{
	CActionCollection actionData(this);

	if (m_choiceMode)
		actionData.AddAction("select", _("Select"), eChooseValue);

	const CActionCollection& data =
		IValueTable::GetActionCollection(formType);

	for (unsigned int idx = 0; idx < data.GetCount(); idx++) {
		const action_identifier_t& id = data.GetID(idx);
		if (id > 0) {
			actionData.AddAction(
				data.GetNameByID(id),
				data.GetCaptionByID(id),
				id
			);
		}
		else {
			actionData.AddSeparator();
		}
	}

	return actionData;
}

void CListDataObjectEnumRef::ExecuteAction(const action_identifier_t& lNumAction, IBackendValueForm* srcForm)
{
	switch (lNumAction)
	{
		case eChooseValue:
			ChooseValue(srcForm);
			break;
		default:
			IValueTable::ExecuteAction(lNumAction, srcForm);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CListDataObjectRef::CActionCollection CListDataObjectRef::GetActionCollection(const form_identifier_t& formType)
{
	CActionCollection actionData(this);

	if (m_choiceMode)
		actionData.AddAction("select", _("Select"), eChooseValue);

	if (m_choiceMode)
		actionData.AddSeparator();

	const CActionCollection& data =
		IValueTable::GetActionCollection(formType);

	for (unsigned int idx = 0; idx < data.GetCount(); idx++) {
		const action_identifier_t& id = data.GetID(idx);
		if (id > 0) {
			actionData.AddAction(
				data.GetNameByID(id),
				data.GetCaptionByID(id),
				id
			);
		}
		else {
			actionData.AddSeparator();
		}
	}

	actionData.InsertAction(3, "markAsDelete", _("Mark as delete"), eMarkAsDelete);

	return actionData;
}

void CListDataObjectRef::ExecuteAction(const action_identifier_t& lNumAction, IBackendValueForm* srcForm)
{
	switch (lNumAction)
	{
		case eMarkAsDelete:
			MarkAsDeleteValue();
			break;
		case eChooseValue:
			ChooseValue(srcForm);
			break;
		default:
			IValueTable::ExecuteAction(lNumAction, srcForm);
			break;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CListDataObjectRef::CActionCollection CTreeDataObjectFolderRef::GetActionCollection(const form_identifier_t& formType)
{
	CActionCollection actionData(this);

	if (m_choiceMode) {
		actionData.AddAction("select", _("Select"), eChooseValue);
		actionData.AddSeparator();
	}

	if (m_listMode == LIST_FOLDER || m_listMode == LIST_ITEM || m_listMode == LIST_ITEM_FOLDER) {
		actionData.AddAction("addFolder", _("Add folder"), eAddFolder);
	}

	CActionCollection data =
		IValueTree::GetActionCollection(formType);

	if (m_listMode == LIST_FOLDER)
		data.RemoveAction(eAddValue); //add

	for (unsigned int idx = 0; idx < data.GetCount(); idx++) {
		const action_identifier_t& id = data.GetID(idx);
		if (id > 0) {
			actionData.AddAction(
				data.GetNameByID(id),
				data.GetCaptionByID(id),
				id
			);
		}
		else {
			actionData.AddSeparator();
		}
	}

	if (m_choiceMode) {
		actionData.InsertAction(5, "markAsDelete", _("Mark as delete"), eMarkAsDelete);
	}
	else {
		actionData.InsertAction(4, "markAsDelete", _("Mark as delete"), eMarkAsDelete);
	}

	return actionData;
}

void CTreeDataObjectFolderRef::ExecuteAction(const action_identifier_t& lNumAction, IBackendValueForm* srcForm)
{
	switch (lNumAction)
	{
		case eAddFolder:
			AddFolderValue();
			break;
		case eMarkAsDelete:
			MarkAsDeleteValue();
			break;
		case eChooseValue:
			ChooseValue(srcForm);
			break;
		default:
			IValueTree::ExecuteAction(lNumAction, srcForm);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////

CListRegisterObject::CActionCollection CListRegisterObject::GetActionCollection(const form_identifier_t& formType)
{
	return IValueTable::GetActionCollection(formType);
}

void CListRegisterObject::ExecuteAction(const action_identifier_t& lNumAction, IBackendValueForm* srcForm)
{
	IValueTable::ExecuteAction(lNumAction, srcForm);
}