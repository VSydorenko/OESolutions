#ifndef __DATABASE_ERROR_REPORTER_H__
#define __DATABASE_ERROR_REPORTER_H__

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include "databaseLayerDef.h"

class BACKEND_API CDatabaseErrorReporter
{
public:
	// ctor
	CDatabaseErrorReporter();

	// dtor
	virtual ~CDatabaseErrorReporter();

	const wxString& GetErrorMessage();
	int GetErrorCode();

	void ResetErrorCodes();

protected:

	void SetErrorMessage(const wxString& strErrorMessage);
	void SetErrorCode(int nErrorCode);

	void ThrowDatabaseException();

private:

	wxString m_strErrorMessage;
	int m_nErrorCode;
};

#endif // __DATABASE_ERROR_REPORTER_H__

