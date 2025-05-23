#ifndef __MYSQL_PREPARED_STATEMENT_H__
#define __MYSQL_PREPARED_STATEMENT_H__

// For compilers that support precompilation, includes "wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include <wx/wx.h>
#endif

#include <wx/dynarray.h>

#include "backend/databaseLayer/preparedStatement.h"
#include "mysqlPreparedStatementParameter.h"
#include "mysqlPreparedStatementWrapper.h"
#include "mysqlInterface.h"

#include "engine/mysql.h"

class IDatabaseResultSet;

WX_DEFINE_ARRAY_PTR(CMysqlPreparedStatementWrapper*, MysqlStatementWrapperArray);

class CMysqlPreparedStatement : public IPreparedStatement
{
public:
	// ctor
	CMysqlPreparedStatement(CMysqlInterface* pInterface);
	CMysqlPreparedStatement(CMysqlInterface* pInterface, MYSQL_STMT* pStatement);

	// dtor
	virtual ~CMysqlPreparedStatement();

	virtual void Close();

	void AddPreparedStatement(MYSQL_STMT* pStatement);

	// get field
	virtual void SetParamInt(int nPosition, int nValue);
	virtual void SetParamDouble(int nPosition, double dblValue);
	virtual void SetParamNumber(int nPosition, const number_t &dblValue);
	virtual void SetParamString(int nPosition, const wxString& strValue);
	virtual void SetParamNull(int nPosition);
	virtual void SetParamBlob(int nPosition, const void* pData, long nDataLength);
	virtual void SetParamDate(int nPosition, const wxDateTime& dateValue);
	virtual void SetParamBool(int nPosition, bool bValue);
	virtual int GetParameterCount();

	virtual int RunQuery();
	virtual IDatabaseResultSet* RunQueryWithResults();

private:
	int FindStatementAndAdjustPositionIndex(int* pPosition);

	CMysqlInterface* m_pInterface;
	MysqlStatementWrapperArray m_Statements;
};

#endif // __MYSQL_PREPARED_STATEMENT_H__

