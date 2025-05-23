/////////////////////////////////////////////////////////////////////////////
// Name:        wxmd5.h
// Purpose:     This implements the md5 hash algorithm
// Author:      Brice Andr�
// Created:     2010/12/05
// RCS-ID:      $Id: mycomp.h 505 2007-03-31 10:31:46Z frm $
// Copyright:   (c) 2010 Brice Andr�
// Licence:     wxWidgets licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MD5_H_
#define _WX_MD5_H_

#include "backend/backend.h"

class BACKEND_API wxMD5 {
public:
	static wxString ComputeMd5(const wxString& content);
	static wxString ComputeKeyedMd5(const wxString& content, const wxString& key);
};

#endif /* _WX_MD5_H_ */
