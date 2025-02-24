////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : frame action
////////////////////////////////////////////////////////////////////////////

#include "form.h"
#include "backend/appData.h"
#include "backend/metaCollection/partial/object.h"

enum
{
	enClose = 10000,
	enUpdate,
	enHelp,
};

//****************************************************************************
//*                              actionData                                     *
//****************************************************************************

CValueForm::CActionCollection CValueForm::GetActionCollection(const form_identifier_t& formType)
{
	CActionCollection actionData(this);

	IActionSource* srcAction =
		dynamic_cast<IActionSource*>(CValueForm::GetSourceObject());

	if (srcAction != nullptr) {
		srcAction->AppendActionCollection(actionData, formType);
	}

	actionData.AddAction("close", _("Close"), enClose);
	actionData.AddAction("update", _("Update"), enUpdate);
	actionData.AddAction("help", _("Help"), enHelp);

	return actionData;
}

void CValueForm::ExecuteAction(const action_identifier_t& lNumAction, IBackendValueForm* srcForm)
{
	if (appData->DesignerMode()) {
		return;
	}

	switch (lNumAction)
	{
	case enClose:
		CloseForm();
		break;
	case enUpdate:
		UpdateForm();
		break;
	case enHelp:
		HelpForm();
		break;
	default:
	{
		IActionSource* srcAction = 
			dynamic_cast<IActionSource*>(CValueForm::GetSourceObject());
		if (srcAction != nullptr)
			srcAction->ExecuteAction(lNumAction, srcForm);	
		break;
	}
	}
}
