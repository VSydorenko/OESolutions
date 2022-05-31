#ifndef _VALUEEVENT_H__
#define _VALUEEVENT_H__

#include "compiler/value.h"

//��������� ��������
class CValueEvent : public CValue
{
	wxDECLARE_DYNAMIC_CLASS(CValueEvent);

public:

	wxString m_eventName;

public:

	CValueEvent();
	CValueEvent(const wxString &eventName);

	virtual bool Init(CValue **aParams);

	virtual wxString GetTypeString() const;
	virtual wxString GetString() const;

	virtual inline bool IsEmpty() const override { return m_eventName.IsEmpty(); }

	virtual ~CValueEvent();
};

#endif