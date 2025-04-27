#ifndef _SYSTEMOBJECTS_ENUMS_H__
#define _SYSTEMOBJECTS_ENUMS_H__

#include "systemEnum.h"
#include "backend/compiler/enumUnit.h"

class CValueStatusMessage : public IEnumeration<eStatusMessage> {
	wxDECLARE_DYNAMIC_CLASS(CValueStatusMessage);
public:
	CValueStatusMessage() : IEnumeration() {}
	//CValueStatusMessage(eStatusMessage status) : IEnumeration(status) {}

	virtual void CreateEnumeration() {
		AddEnumeration(eStatusMessage::eStatusMessage_Information, wxT("information"));
		AddEnumeration(eStatusMessage::eStatusMessage_Warning, wxT("warning"));
		AddEnumeration(eStatusMessage::eStatusMessage_Error, wxT("error"));
	}
};

class CValueQuestionMode : public IEnumeration<eQuestionMode> {
	wxDECLARE_DYNAMIC_CLASS(CValueQuestionMode);
public:
	CValueQuestionMode() : IEnumeration() {}
	//CValueQuestionMode(eQuestionMode mode) : IEnumeration(mode) {}

	virtual void CreateEnumeration() {
		AddEnumeration(eQuestionMode::eQuestionMode_YesNo, wxT("yesNo"));
		AddEnumeration(eQuestionMode::eQuestionMode_YesNoCancel, wxT("yesNoCancel"));
		AddEnumeration(eQuestionMode::eQuestionMode_OK, wxT("ok"));
		AddEnumeration(eQuestionMode::eQuestionMode_OKCancel, wxT("okCancel"));
	}
};

class CValueQuestionReturnCode : public IEnumeration<eQuestionReturnCode> {
	wxDECLARE_DYNAMIC_CLASS(CValueQuestionReturnCode);
public:
	CValueQuestionReturnCode() : IEnumeration() {}
	//CValueQuestionReturnCode(eQuestionReturnCode code) : IEnumeration(code) {}

	virtual void CreateEnumeration() {
		AddEnumeration(eQuestionReturnCode::eQuestionReturnCode_Yes, wxT("yes"));
		AddEnumeration(eQuestionReturnCode::eQuestionReturnCode_No, wxT("no"));
		AddEnumeration(eQuestionReturnCode::eQuestionReturnCode_OK, wxT("ok"));
		AddEnumeration(eQuestionReturnCode::eQuestionReturnCode_Cancel, wxT("cancel"));
	}
};

class CValueRoundMode : public IEnumeration<eRoundMode> {
	wxDECLARE_DYNAMIC_CLASS(CValueQuestionReturnCode);
public:
	CValueRoundMode() : IEnumeration() {}
	//CValueRoundMode(eRoundMode mode) : IEnumeration(mode) {}

	virtual void CreateEnumeration() {
		AddEnumeration(eRoundMode::eRoundMode_Round15as10, wxT("round15as10"));
		AddEnumeration(eRoundMode::eRoundMode_Round15as20, wxT("round15as20"));
	}
};

class CValueChars : public IEnumeration<enChars> {
	wxDECLARE_DYNAMIC_CLASS(CValueChars);
public:
	CValueChars() : IEnumeration() {}
	//CValueChars(enChars c) : IEnumeration(c) {}

	virtual void CreateEnumeration() {
		AddEnumeration(enChars::eCR, wxT("CR"));
		AddEnumeration(enChars::eFF, wxT("FF"));
		AddEnumeration(enChars::eLF, wxT("LF"));
		AddEnumeration(enChars::eNBSp, wxT("NBSp"));
		AddEnumeration(enChars::eTab, wxT("Tab"));
		AddEnumeration(enChars::eVTab, wxT("VTab"));
	}
	
	virtual wxString GetDescription(enChars val) const {
		return (char)val;
	}
};

#endif