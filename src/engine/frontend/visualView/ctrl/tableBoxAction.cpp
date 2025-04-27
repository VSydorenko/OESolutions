#include "tableBox.h"
#include "backend/metaCollection/partial/commonObject.h"
#include "form.h"

//****************************************************************************
//*                              actionData                                     *
//****************************************************************************

CValueTableBox::CActionCollection CValueTableBox::GetActionCollection(const form_identifier_t& formType)
{
	if (m_tableModel == nullptr) {
		//if (m_dataSource.isValid()) {
		//	if (srcObject != nullptr) {
		//		IValueModel* tableModel = nullptr;
		//		if (srcObject->GetModel(tableModel, GetIdByGuid(m_dataSource))) {
		//			return tableModel->GetActionCollection(formType);
		//		}
		//	}
		//}

		if (!m_propertySource->IsEmptyProperty()) {
			ISourceDataObject* srcObject = m_formOwner->GetSourceObject();
			if (srcObject != nullptr) {
				IValueModel* tableModel = nullptr;
				if (srcObject->GetModel(tableModel, m_propertySource->GetValueAsSource())) {
					return tableModel->GetActionCollection(formType);
				}
			}
		}

		return CActionCollection();
	}

	return m_tableModel->GetActionCollection(formType);
}

#include "backend/appData.h"

void CValueTableBox::ExecuteAction(const action_identifier_t& lNumAction, IBackendValueForm* srcForm)
{
	if (m_tableModel == nullptr) return;
	if (!appData->DesignerMode()) {
		m_tableModel->ExecuteAction(lNumAction, srcForm);
	}
}