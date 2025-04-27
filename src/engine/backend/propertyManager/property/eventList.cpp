#include "eventList.h"

//base property for "list"
bool CEventList::SetDataValue(const CValue& varPropVal)
{
	if (!m_functor->Invoke(this))
		return false;
	for (unsigned int idx = 0; idx < m_listPropValue.GetItemCount(); idx++) {
		if (m_listPropValue.GetItemValue(idx) == varPropVal) {
			SetValue(stringUtils::IntToStr(m_listPropValue.GetItemId(idx)));
			return true;
		}
	}
	return false;
};

bool CEventList::GetDataValue(CValue& pvarPropVal) const
{
	if (!m_functor->Invoke(const_cast<CEventList*>(this)))
		return false;
	for (unsigned int idx = 0; idx < m_listPropValue.GetItemCount(); idx++) {
		if (m_listPropValue.GetItemId(idx) == GetValueAsInteger()) {
			pvarPropVal = m_listPropValue.GetItemValue(idx);
			return true;
		}
	}
	return false;
};

bool CEventList::LoadData(CMemoryReader& reader)
{
	SetValue(stringUtils::IntToStr(reader.r_s32()));
	return true;
};

bool CEventList::SaveData(CMemoryWriter& writer)
{
	writer.w_s32(GetValueAsInteger());
	return true;
};