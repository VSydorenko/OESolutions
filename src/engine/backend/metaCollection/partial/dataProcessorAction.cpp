////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : dataProcessor
////////////////////////////////////////////////////////////////////////////

#include "dataProcessor.h"

CRecordDataObjectDataProcessor::CActionCollection CRecordDataObjectDataProcessor::GetActionCollection(const form_identifier_t &formType)
{
	return CActionCollection(this);
}

void CRecordDataObjectDataProcessor::ExecuteAction(const action_identifier_t &action, IBackendValueForm *srcForm)
{
}