#ifndef __DATABASE_RESULT_SET_H__
#define __DATABASE_RESULT_SET_H__

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/hashmap.h>
#include <wx/hashset.h>
#include <wx/datetime.h>

#include "databaseLayerDef.h"
#include "databaseErrorReporter.h"
#include "databaseStringConverter.h"
#include "resultSetMetaData.h"

WX_DECLARE_STRING_HASH_MAP(int, StringToIntMap);
WX_DECLARE_HASH_SET(IResultSetMetaData*, wxPointerHash, wxPointerEqual, MetaDataHashSet);

class BACKEND_API IDatabaseResultSet : public CDatabaseErrorReporter, public CDatabaseStringConverter
{
public:
	/// Constructor
	IDatabaseResultSet();

	/// Destructor
	virtual ~IDatabaseResultSet();

	/// Move to the next record in the result set
	virtual bool Next() = 0;
	/// Close the result set (call IDatabaseLayer::CloseResultSet() instead on the result set)
	virtual void Close() = 0;

	virtual int LookupField(const wxString& strField) = 0;

	// get field
	/// Retrieve an integer from the result set by the 1-based field index
	virtual int GetResultInt(int nField) = 0;
	/// Retrieve a wxString from the result set by the 1-based field index
	virtual wxString GetResultString(int nField) = 0;
	/// Retrieve a long from the result set by the 1-based field index
	virtual long long GetResultLong(int nField) = 0;
	/// Retrieve a boolean from the result set by the 1-based field index
	virtual bool GetResultBool(int nField) = 0;
	/// Retrieve a wxDateTime from the result set by the 1-based field index
	virtual wxDateTime GetResultDate(int nField) = 0;
	/// Retrieve a BLOB from the result set by the 1-based field index
	virtual void* GetResultBlob(int nField, wxMemoryBuffer& buffer) = 0;
	/// Retrieve a double from the result set by the 1-based field index
	virtual double GetResultDouble(int nField) = 0;
	/// Retrieve a number from the result set by the 1-based field index
	virtual number_t GetResultNumber(int nField) = 0;
	/// Check if a field in the current result set record is nullptr
	virtual bool IsFieldNull(int nField) = 0;

	/// Retrieve an integer from the result set by the result set column name
	virtual int GetResultInt(const wxString& strField);
	/// Retrieve a wxString from the result set by the result set column name
	virtual wxString GetResultString(const wxString& strField);
	/// Retrieve a long from the result set by the result set column name
	virtual long long GetResultLong(const wxString& strField);
	/// Retrieve a boolean from the result set by the result set column name
	virtual bool GetResultBool(const wxString& strField);
	/// Retrieve a wxDateTime from the result set by the result set column name
	virtual wxDateTime GetResultDate(const wxString& strField);
	/// Retrieve a BLOB from the result set by the result set column name
	virtual void* GetResultBlob(const wxString& strField, wxMemoryBuffer& buffer);
	/// Retrieve a double from the result set by the result set column name
	virtual double GetResultDouble(const wxString& strField);
	/// Retrieve a number from the result set by the result set column name
	virtual number_t GetResultNumber(const wxString& strField);
	/// Check if a field in the current result set record is nullptr
	virtual bool IsFieldNull(const wxString& strField);

	// get MetaData
	/// Retrieve the MetaData associated with this result set
	virtual IResultSetMetaData* GetMetaData() = 0;
	/// Close MetaData previously returned by the result set
	virtual bool CloseMetaData(IResultSetMetaData* pMetaData);

protected:
	/// Close all meta data objects that have been generated but not yet closed
	void CloseMetaData();
	/// Add meta data object pointer to the list for "garbage collection"
	void LogMetaDataForCleanup(IResultSetMetaData* pMetaData) { m_MetaData.insert(pMetaData); }

private:
	MetaDataHashSet m_MetaData;
};

#endif // __DATABASE_RESULT_SET_H__

