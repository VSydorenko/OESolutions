#include "constant.h"

enum
{
	eDefActionAndClose = 1,
	eSave,
};


CRecordDataObjectConstant::CActionCollection CRecordDataObjectConstant::GetActionCollection(const form_identifier_t& formType)
{
	CActionCollection catalogActions(this);
	catalogActions.AddAction("saveAndClose", _("Save and close"), eDefActionAndClose);
	catalogActions.AddAction("save", _("Save"), eSave);
	return catalogActions;
}

void CRecordDataObjectConstant::ExecuteAction(const action_identifier_t& lNumAction, IBackendValueForm* srcForm)
{
	switch (lNumAction)
	{
	case eDefActionAndClose:
		if (SetConstValue(m_constVal))
			srcForm->CloseForm();
		break;
	case eSave: 
		SetConstValue(m_constVal); 
		break;
	}
}