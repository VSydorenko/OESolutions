////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : report
////////////////////////////////////////////////////////////////////////////

#include "dataReport.h"

CRecordDataObjectReport::CActionCollection CRecordDataObjectReport::GetActionCollection(const form_identifier_t &formType)
{
	return CActionCollection(this);
}

void CRecordDataObjectReport::ExecuteAction(const action_identifier_t &action, IBackendValueForm *srcForm)
{
}