////////////////////////////////////////////////////////////////////////////
//	Author		: Maxim Kornienko
//	Description : base db reader/saver
////////////////////////////////////////////////////////////////////////////

#include "baseObject.h"
#include "appData.h"
#include "databaseLayer/databaseLayer.h"
#include "databaseLayer/databaseErrorCodes.h"
#include "metadata/metaObjects/objects/tabularSection/tabularSection.h"
#include "utils/stringUtils.h"

//**********************************************************************************************************
//*                                          Common functions                                              *
//**********************************************************************************************************

wxString IMetaObjectRecordDataRef::GetTableNameDB() const
{
	wxString className = GetClassName();
	wxASSERT(m_metaId != 0);
	return wxString::Format("%s%i",
		className, GetMetaID());
}

int IMetaObjectRecordDataRef::ProcessAttribute(const wxString& tableName, IMetaAttributeObject* srcAttr, IMetaAttributeObject* dstAttr)
{
	return IMetaAttributeObject::ProcessAttribute(tableName, srcAttr, dstAttr);
}

int IMetaObjectRecordDataRef::ProcessEnumeration(const wxString& tableName, const meta_identifier_t& id, CMetaEnumerationObject* srcEnum, CMetaEnumerationObject* dstEnum)
{
	int retCode = 1;
	//is null - create
	if (dstEnum == NULL) {
		PreparedStatement* prepareStatement =
			databaseLayer->PrepareStatement("UPDATE OR INSERT INTO %s (UUID, UUIDREF) VALUES (?, ?) MATCHING(UUID);", tableName);
		if (!prepareStatement)
			return 0;
		reference_t* reference_impl = new reference_t{ id, srcEnum->GetGuid() };
		prepareStatement->SetParamString(1, srcEnum->GetGuid().str());
		prepareStatement->SetParamBlob(2, reference_impl, sizeof(reference_t));

		retCode = prepareStatement->RunQuery();
		delete reference_impl;
	}
	// update 
	else if (srcEnum != NULL) {
		PreparedStatement* prepareStatement =
			databaseLayer->PrepareStatement("UPDATE OR INSERT INTO %s (UUID, UUIDREF) VALUES (?, ?) MATCHING(UUID);", tableName);
		if (!prepareStatement)
			return 0;
		reference_t* reference_impl = new reference_t{ id, srcEnum->GetGuid() };
		prepareStatement->SetParamString(1, srcEnum->GetGuid().str());
		prepareStatement->SetParamBlob(2, reference_impl, sizeof(reference_t));
		retCode = prepareStatement->RunQuery();
		delete reference_impl;
	}
	//delete 
	else if (srcEnum == NULL) {
		retCode = databaseLayer->RunQuery("DELETE FROM %s WHERE UUID = '%s' ;", tableName, dstEnum->GetGuid().str());
	}

	return retCode;
}

int IMetaObjectRecordDataRef::ProcessTable(const wxString& tabularName, CMetaTableObject* srcTable, CMetaTableObject* dstTable)
{
	int retCode = 1;
	//is null - create
	if (dstTable == NULL) {
		retCode = databaseLayer->RunQuery("CREATE TABLE %s (UUID VARCHAR(36) NOT NULL, UUIDREF BLOB);", tabularName);
		if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR) return retCode;
		CMetaDefaultAttributeObject* attrNumberLine = srcTable->GetNumberLine();
		wxASSERT(attrNumberLine);
		//default attributes
		for (auto attribute : srcTable->GetGenericAttributes()) {
			retCode = ProcessAttribute(tabularName,
				attribute, NULL);
		}
	}
	// update 
	else if (srcTable != NULL) {
		for (auto attribute : srcTable->GetGenericAttributes()) {
			retCode = ProcessAttribute(tabularName,
				attribute, dstTable->FindAttributeByName(attribute->GetDocPath())
			);
			if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
				return retCode;
		}
	}
	//delete 
	else if (srcTable == NULL) {
		retCode = databaseLayer->RunQuery("DROP TABLE %s", tabularName);
	}

	return retCode;
}

bool IMetaObjectRecordDataRef::CreateAndUpdateTableDB(IConfigMetadata* srcMetaData, IMetaObject* srcMetaObject, int flags)
{
	wxString tableName = GetTableNameDB(); int retCode = DATABASE_LAYER_QUERY_RESULT_ERROR;

	if ((flags & createMetaTable) != 0) {
		retCode = databaseLayer->RunQuery("CREATE TABLE %s (UUID VARCHAR(36) NOT NULL PRIMARY KEY, UUIDREF BLOB);", tableName);

		for (auto attribute : GetDefaultAttributes()) {
			if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
				return false;
			if (IMetaObjectRecordDataRef::IsDataReference(attribute->GetMetaID()))
				continue;
			retCode = ProcessAttribute(tableName,
				attribute, NULL);
		}

		if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
			return false;

		for (auto attribute : GetObjectAttributes()) {
			if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
				return false;
			retCode = ProcessAttribute(tableName,
				attribute, NULL);
		}

		if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
			return false;

		for (auto enumeration : GetObjectEnums()) {
			if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
				return false;
			retCode = ProcessEnumeration(tableName, m_metaId,
				enumeration, NULL);
		}

		if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
			return false;

		for (auto table : GetObjectTables()) {
			if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
				return false;
			retCode = ProcessTable(table->GetTableNameDB(),
				table, NULL);
		}
	}
	else if ((flags & updateMetaTable) != 0) {
		//if src is null then delete
		IMetaObjectRecordDataRef* dstValue = NULL;
		if (srcMetaObject->ConvertToValue(dstValue)) {

			//attributes from dst 
			for (auto attribute : dstValue->GetDefaultAttributes()) {
				IMetaObject* foundedMeta =
					IMetaObjectRecordDataRef::FindDefAttributeByName(attribute->GetDocPath());
				if (dstValue->IsDataReference(attribute->GetMetaID()))
					continue;
				if (foundedMeta == NULL) {
					retCode = ProcessAttribute(tableName, NULL, attribute);
					if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
						return false;
				}
			}
			//attributes current
			for (auto attribute : GetDefaultAttributes()) {
				if (IMetaObjectRecordDataRef::IsDataReference(attribute->GetMetaID()))
					continue;
				retCode = ProcessAttribute(tableName,
					attribute, dstValue->FindDefAttributeByName(attribute->GetDocPath())
				);
				if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
					return false;
			}

			//attributes from dst 
			for (auto attribute : dstValue->GetObjectAttributes()) {
				IMetaObject* foundedMeta =
					IMetaObjectRecordDataRef::FindAttributeByName(attribute->GetDocPath());
				if (foundedMeta == NULL) {
					retCode = ProcessAttribute(tableName, NULL, attribute);
					if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
						return false;
				}
			}
			//attributes current
			for (auto attribute : GetObjectAttributes()) {
				retCode = ProcessAttribute(tableName,
					attribute, dstValue->FindAttributeByName(attribute->GetDocPath())
				);
				if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
					return false;
			}

			//enums from dst 
			for (auto enumeration : dstValue->GetObjectEnums()) {
				IMetaObject* foundedMeta =
					IMetaObjectRecordDataRef::FindEnumByName(enumeration->GetDocPath());
				if (foundedMeta == NULL) {
					retCode = ProcessEnumeration(tableName, m_metaId, NULL, enumeration);
					if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
						return false;
				}
			}
			//enums current
			for (auto enumeration : GetObjectEnums()) {
				retCode = ProcessEnumeration(tableName, m_metaId,
					enumeration, dstValue->FindEnumByName(enumeration->GetDocPath())
				);
				if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
					return false;
			}

			//tables from dst 
			for (auto table : dstValue->GetObjectTables()) {
				IMetaObject* foundedMeta =
					IMetaObjectRecordDataRef::FindTableByName(table->GetDocPath());
				if (foundedMeta == NULL) {
					retCode = ProcessTable(table->GetTableNameDB(), NULL, table);
					if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
						return false;
				}
			}
			//tables current 
			for (auto table : GetObjectTables()) {
				retCode = ProcessTable(table->GetTableNameDB(),
					table, dstValue->FindTableByName(table->GetDocPath())
				);
				if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
					return false;
			}
		}
	}
	else if ((flags & deleteMetaTable) != 0) {
		retCode = databaseLayer->RunQuery("DROP TABLE %s", tableName);
		for (auto table : GetObjectTables()) {
			if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
				return false;
			wxString tabularName = table->GetTableNameDB();
			retCode = databaseLayer->RunQuery("DROP TABLE %s", tabularName);
		}
	}

	if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
		return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

wxString IMetaObjectRegisterData::GetTableNameDB() const
{
	wxString className = GetClassName();
	wxASSERT(m_metaId != 0);
	return wxString::Format("%s%i",
		className, GetMetaID());
}

int IMetaObjectRegisterData::ProcessDimension(const wxString& tableName, IMetaAttributeObject* srcAttr, IMetaAttributeObject* dstAttr)
{
	return IMetaAttributeObject::ProcessAttribute(tableName, srcAttr, dstAttr);
}

int IMetaObjectRegisterData::ProcessResource(const wxString& tableName, IMetaAttributeObject* srcAttr, IMetaAttributeObject* dstAttr)
{
	return IMetaAttributeObject::ProcessAttribute(tableName, srcAttr, dstAttr);
}

int IMetaObjectRegisterData::ProcessAttribute(const wxString& tableName, IMetaAttributeObject* srcAttr, IMetaAttributeObject* dstAttr)
{
	return IMetaAttributeObject::ProcessAttribute(tableName, srcAttr, dstAttr);
}

bool IMetaObjectRegisterData::CreateAndUpdateTableDB(IConfigMetadata* srcMetaData, IMetaObject* srcMetaObject, int flags)
{
	wxString tableName = GetTableNameDB();

	int retCode = DATABASE_LAYER_QUERY_RESULT_ERROR;

	if ((flags & createMetaTable) != 0) {

		retCode = databaseLayer->RunQuery("CREATE TABLE %s (rowData BLOB);", tableName);

		for (auto attribute : GetDefaultAttributes()) {
			if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
				return false;
			retCode = ProcessAttribute(tableName,
				attribute, NULL);
		}

		if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
			return false;

		for (auto dimension : GetObjectDimensions()) {
			if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
				return false;
			retCode = ProcessDimension(tableName,
				dimension, NULL);
		}

		if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
			return false;

		for (auto resource : GetObjectResources()) {
			if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
				return false;
			retCode = ProcessResource(tableName,
				resource, NULL);
		}

		if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
			return false;

		for (auto attribute : GetObjectAttributes()) {
			if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
				return false;
			retCode = ProcessAttribute(tableName,
				attribute, NULL);
		}
	}
	else if ((flags & updateMetaTable) != 0) {

		//if src is null then delete
		IMetaObjectRegisterData* dstValue = NULL;
		if (srcMetaObject->ConvertToValue(dstValue)) {

			//attributes from dst 
			for (auto attribute : dstValue->GetDefaultAttributes()) {
				IMetaObject* foundedMeta =
					IMetaObjectRegisterData::FindDefAttributeByName(attribute->GetDocPath());
				if (foundedMeta == NULL) {
					retCode = ProcessAttribute(tableName, NULL, attribute);
					if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
						return false;
				}
			}

			//attributes current
			for (auto attribute : GetDefaultAttributes()) {
				retCode = ProcessAttribute(tableName,
					attribute, dstValue->FindDefAttributeByName(attribute->GetDocPath())
				);
				if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
					return false;
			}

			//dimensions from dst 
			for (auto dimension : dstValue->GetObjectDimensions()) {
				IMetaObject* foundedMeta =
					IMetaObjectRegisterData::FindDimensionByName(dimension->GetDocPath());
				if (foundedMeta == NULL) {
					retCode = ProcessDimension(tableName, NULL, dimension);
					if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
						return false;
				}
			}

			//dimensions current
			for (auto dimension : GetObjectDimensions()) {
				retCode = ProcessDimension(tableName,
					dimension, dstValue->FindDimensionByName(dimension->GetDocPath())
				);
				if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
					return false;
			}

			//resources from dst 
			for (auto resource : dstValue->GetObjectResources()) {
				IMetaObject* foundedMeta =
					IMetaObjectRegisterData::FindResourceByName(resource->GetDocPath());
				if (foundedMeta == NULL) {
					retCode = ProcessResource(tableName, NULL, resource);
					if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
						return false;
				}
			}

			//resources current
			for (auto resource : GetObjectResources()) {
				retCode = ProcessResource(tableName,
					resource, dstValue->FindResourceByName(resource->GetDocPath())
				);
				if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
					return false;
			}

			//attributes from dst 
			for (auto attribute : dstValue->GetObjectAttributes()) {
				IMetaObject* foundedMeta =
					IMetaObjectRegisterData::FindAttributeByName(attribute->GetDocPath());
				if (foundedMeta == NULL) {
					retCode = ProcessAttribute(tableName, NULL, attribute);
					if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
						return false;
				}
			}

			//attributes current
			for (auto attribute : GetObjectAttributes()) {
				retCode = ProcessAttribute(tableName,
					attribute, dstValue->FindAttributeByName(attribute->GetDocPath())
				);
				if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
					return false;
			}
		}
	}
	else if ((flags & deleteMetaTable) != 0) {
		retCode = databaseLayer->RunQuery("DROP TABLE %s", tableName);
	}

	if (retCode == DATABASE_LAYER_QUERY_RESULT_ERROR)
		return false;

	return true;
}

//**********************************************************************************************************
//*                                          Query functions                                               *
//**********************************************************************************************************

#include "compiler/systemObjects.h"

bool IRecordDataObjectRef::ReadData()
{
	if (m_metaObject == NULL ||
		m_newObject)
		return false;

	wxString tableName = m_metaObject->GetTableNameDB();

	if (databaseLayer->TableExists(tableName)) {
		wxString queryText = "SELECT FIRST 1 * FROM " + tableName + " WHERE UUID = '" + m_objGuid.str() + "';";
		DatabaseResultSet* resultSet = databaseLayer->RunQueryWithResults(queryText);
		if (resultSet == NULL)
			return false;
		for (auto currTable : m_metaObject->GetObjectTables()) {
			CTabularSectionDataObjectRef* tabularSection =
				new CTabularSectionDataObjectRef(this, currTable);
			m_aObjectValues[currTable->GetMetaID()] = tabularSection;
			m_aObjectTables.push_back(tabularSection);
		}
		if (resultSet->Next()) {
			//load other attributes 
			for (auto attribute : m_metaObject->GetGenericAttributes()) {
				m_aObjectValues.insert_or_assign(attribute->GetMetaID(),
					IMetaAttributeObject::GetValueAttribute(attribute, resultSet)
				);
			}
			for (auto tabularSection : m_aObjectTables) {
				tabularSection->LoadData();
			}
		}
		resultSet->Close();
		return true;
	}
	return false;
}

bool IRecordDataObjectRef::SaveData()
{
	wxASSERT(m_metaObject);

	if (m_codeGenerator != NULL) {
		if (m_aObjectValues[m_codeGenerator->GetMetaID()].IsEmpty()) {
			m_aObjectValues[m_codeGenerator->GetMetaID()] = m_codeGenerator->GenerateCode();
		}
	}

	//check fill attributes 
	bool fillCheck = true;

	for (auto attribute : m_metaObject->GetGenericAttributes()) {
		if (attribute->FillCheck()) {
			if (m_aObjectValues[attribute->GetMetaID()].IsEmpty()) {
				wxString fillError =
					wxString::Format(_("""%s"" is a required field"), attribute->GetSynonym());
				CSystemObjects::Message(fillError, eStatusMessage::eStatusMessage_Information);
				fillCheck = false;
			}
		}
	}

	if (!fillCheck)
		return false;

	wxString tableName = m_metaObject->GetTableNameDB();
	wxString queryText = "UPDATE OR INSERT INTO " + tableName + " (";
	queryText += "UUID, UUIDREF";
	for (auto attribute : m_metaObject->GetGenericAttributes()) {
		if (m_metaObject->IsDataReference(attribute->GetMetaID()))
			continue;
		queryText = queryText + ", " + IMetaAttributeObject::GetSQLFieldName(attribute);
	}
	queryText += ") VALUES (?, ?";
	for (auto attribute : m_metaObject->GetGenericAttributes()) {
		if (m_metaObject->IsDataReference(attribute->GetMetaID()))
			continue;
		unsigned int fieldCount = IMetaAttributeObject::GetSQLFieldCount(attribute);
		for (unsigned int i = 0; i < fieldCount; i++) {
			queryText += ", ?";
		}
	}
	queryText += ") MATCHING (UUID);";

	PreparedStatement* statement = databaseLayer->PrepareStatement(queryText);

	if (statement == NULL)
		return false;

	m_objGuid = m_reference_impl->m_guid;

	statement->SetParamString(1, m_objGuid.str());
	statement->SetParamBlob(2, m_reference_impl, sizeof(reference_t));

	int position = 3;

	for (auto attribute : m_metaObject->GetGenericAttributes()) {
		if (m_metaObject->IsDataReference(attribute->GetMetaID()))
			continue;
		IMetaAttributeObject::SetValueAttribute(
			attribute,
			m_aObjectValues.at(attribute->GetMetaID()),
			statement,
			position
		);
	}

	bool hasError = statement->RunQuery() == DATABASE_LAYER_QUERY_RESULT_ERROR;
	databaseLayer->CloseStatement(statement);

	//table parts
	if (!hasError) {
		for (auto table : m_aObjectTables) {
			if (!table->SaveData()) {
				hasError = true; break;
			}
		}
	}

	if (!hasError) {
		m_newObject = false;
	}

	return !hasError;
}

bool IRecordDataObjectRef::DeleteData()
{
	if (m_newObject)
		return false;

	wxString tableName = m_metaObject->GetTableNameDB();
	bool hasError = databaseLayer->RunQuery("DELETE FROM " + tableName + " WHERE UUID = '" + m_objGuid.str() + "';") == DATABASE_LAYER_QUERY_RESULT_ERROR;

	//table parts
	for (auto table : m_aObjectTables) {
		if (!table->DeleteData()) {
			hasError = true; break;
		}
	}

	return !hasError;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool IRecordManagerObject::ExistData()
{
	wxString tableName = m_metaObject->GetTableNameDB(); int position = 1;
	wxString queryText = "SELECT * FROM " + tableName; bool firstWhere = true;

	if (m_recordLine == NULL)
		return false;

	for (auto attribute : m_metaObject->GetGenericDimensions()) {
		if (firstWhere) {
			queryText = queryText + " WHERE ";
		}
		queryText = queryText +
			(firstWhere ? " " : " AND ") + IMetaAttributeObject::GetCompositeSQLFieldName(attribute);
		if (firstWhere) {
			firstWhere = false;
		}
	}

	PreparedStatement* statement = databaseLayer->PrepareStatement(queryText);

	if (statement == NULL)
		return false;

	for (auto attribute : m_metaObject->GetGenericDimensions()) {
		IMetaAttributeObject::SetValueAttribute(
			attribute,
			m_recordLine->GetValueByMetaID(attribute->GetMetaID()),
			statement,
			position
		);
	}

	DatabaseResultSet* resultSet = statement->RunQueryWithResults();

	if (resultSet == NULL)
		return false;

	bool founded = false;

	if (resultSet->Next()) {
		founded = true;
	}

	return founded;
}

bool IRecordManagerObject::ReadData()
{
	if (m_recordSet->ReadData()) {

		if (m_recordLine == NULL) {
			m_recordLine = m_recordSet->GetRowAt(0);
		}

		return true;
	}

	return false;
}

bool IRecordManagerObject::SaveData(bool replace)
{
	if (m_recordSet->Selected()
		&& !DeleteData())
		return false;

	if (ExistData())
		return false;

	m_recordSet->m_aKeyValues.clear();

	wxASSERT(m_recordLine);

	for (auto attribute : m_metaObject->GetGenericDimensions()) {
		m_recordSet->m_aKeyValues.insert_or_assign(
			attribute->GetMetaID(),
			m_recordLine->GetValueByMetaID(attribute->GetMetaID())
		);
	}

	if (m_recordSet->WriteRecordSet(replace, false)) {
		m_objGuid.SetKeyPair(m_recordSet->m_aKeyValues);
		return true;
	}

	return false;
}

bool IRecordManagerObject::DeleteData()
{
	return m_recordSet->DeleteRecordSet();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool IRecordSetObject::ExistData()
{
	wxString tableName = m_metaObject->GetTableNameDB(); int position = 1;
	wxString queryText = "SELECT * FROM " + tableName; bool firstWhere = true;

	for (auto attribute : m_metaObject->GetGenericDimensions()) {
		if (!IRecordSetObject::FindKeyValue(attribute->GetMetaID()))
			continue;
		if (firstWhere) {
			queryText = queryText + " WHERE ";
		}
		queryText = queryText +
			(firstWhere ? " " : " AND ") + IMetaAttributeObject::GetCompositeSQLFieldName(attribute);
		if (firstWhere) {
			firstWhere = false;
		}
	}

	PreparedStatement* statement = databaseLayer->PrepareStatement(queryText);

	if (statement == NULL)
		return false;

	for (auto attribute : m_metaObject->GetGenericDimensions()) {
		if (!IRecordSetObject::FindKeyValue(attribute->GetMetaID()))
			continue;
		IMetaAttributeObject::SetValueAttribute(
			attribute,
			m_aKeyValues.at(attribute->GetMetaID()),
			statement,
			position
		);
	}

	DatabaseResultSet* resultSet = statement->RunQueryWithResults();

	if (resultSet == NULL)
		return false;

	bool founded = false;

	if (resultSet->Next()) {
		founded = true;
	}

	return founded;
}

bool IRecordSetObject::ExistData(number_t& lastNum)
{
	wxString tableName = m_metaObject->GetTableNameDB(); int position = 1;
	wxString queryText = "SELECT * FROM " + tableName; bool firstWhere = true;

	for (auto attribute : m_metaObject->GetGenericDimensions()) {
		if (!IRecordSetObject::FindKeyValue(attribute->GetMetaID()))
			continue;
		if (firstWhere) {
			queryText = queryText + " WHERE ";
		}
		queryText = queryText +
			(firstWhere ? " " : " AND ") + IMetaAttributeObject::GetCompositeSQLFieldName(attribute);
		if (firstWhere) {
			firstWhere = false;
		}
	}

	PreparedStatement* statement = databaseLayer->PrepareStatement(queryText);

	if (statement == NULL)
		return false;

	for (auto attribute : m_metaObject->GetGenericDimensions()) {
		if (!IRecordSetObject::FindKeyValue(attribute->GetMetaID()))
			continue;
		IMetaAttributeObject::SetValueAttribute(
			attribute,
			m_aKeyValues.at(attribute->GetMetaID()),
			statement,
			position
		);
	}

	DatabaseResultSet* resultSet = statement->RunQueryWithResults();

	if (resultSet == NULL)
		return false;

	bool founded = false; lastNum = 1;

	while (resultSet->Next()) {
		CValue numLine = IMetaAttributeObject::GetValueAttribute(
			m_metaObject->GetRegisterLineNumber(), resultSet
		);
		if (numLine > lastNum) {
			lastNum = numLine.GetNumber();
		}
		founded = true;
	}

	return founded;
}

bool IRecordSetObject::ReadData()
{
	m_aObjectValues.clear(); int position = 1;

	wxString tableName = m_metaObject->GetTableNameDB();
	wxString queryText = "SELECT * FROM " + tableName; bool firstWhere = true;

	for (auto attribute : m_metaObject->GetGenericDimensions()) {
		if (!IRecordSetObject::FindKeyValue(attribute->GetMetaID()))
			continue;
		if (firstWhere) {
			queryText = queryText + " WHERE ";
		}
		queryText = queryText +
			(firstWhere ? " " : " AND ") + IMetaAttributeObject::GetCompositeSQLFieldName(attribute);
		if (firstWhere) {
			firstWhere = false;
		}
	}

	PreparedStatement* statement = databaseLayer->PrepareStatement(queryText);

	if (statement == NULL)
		return false;

	for (auto attribute : m_metaObject->GetGenericDimensions()) {
		if (!IRecordSetObject::FindKeyValue(attribute->GetMetaID()))
			continue;
		IMetaAttributeObject::SetValueAttribute(
			attribute,
			m_aKeyValues.at(attribute->GetMetaID()),
			statement,
			position
		);
	}

	DatabaseResultSet* resultSet = statement->RunQueryWithResults();

	if (resultSet == NULL)
		return false;

	while (resultSet->Next()) {
		std::map<meta_identifier_t, CValue> aKeyTable, aRowTable;
		for (auto attribute : m_metaObject->GetGenericDimensions()) {
			aKeyTable.insert_or_assign(
				attribute->GetMetaID(),
				IMetaAttributeObject::GetValueAttribute(attribute, resultSet)
			);
		}
		for (auto attribute : m_metaObject->GetGenericAttributes()) {
			aRowTable.insert_or_assign(
				attribute->GetMetaID(),
				IMetaAttributeObject::GetValueAttribute(attribute, resultSet)
			);
		}
		m_aObjectValues.push_back(aRowTable); m_selected = true;
	}

	resultSet->Close();

	if (!CTranslateError::IsSimpleMode()) {
		IValueTable::Reset(m_aObjectValues.size());
	}

	return m_aObjectValues.size() > 0;
}

bool IRecordSetObject::SaveData(bool replace, bool clearTable)
{
	bool hasError = false;

	//check fill attributes 
	bool fillCheck = true; long currLine = 1;
	for (auto objectValue : m_aObjectValues) {
		for (auto attribute : m_metaObject->GetGenericAttributes()) {
			if (attribute->FillCheck()) {
				if (objectValue[attribute->GetMetaID()].IsEmpty()) {
					wxString fillError =
						wxString::Format(_("The %s is required on line %i of the %s"), attribute->GetSynonym(), currLine, m_metaObject->GetSynonym());
					CSystemObjects::Message(fillError, eStatusMessage::eStatusMessage_Information);
					fillCheck = false;
				}
			}
		}
		currLine++;
	}

	if (!fillCheck)
		return false;

	number_t numberLine = 1, oldNumberLine = 1;

	if (m_metaObject->HasRecorder() &&
		IRecordSetObject::ExistData(oldNumberLine)) {
		if (replace && !IRecordSetObject::DeleteData())
			return false;
		if (!replace) {
			numberLine = oldNumberLine;
		}
	}
	else if (IRecordSetObject::ExistData()) {
		if (replace && !IRecordSetObject::DeleteData())
			return false;
		if (!replace) {
			numberLine = oldNumberLine;
		}
	}

	wxString tableName = m_metaObject->GetTableNameDB(); bool firstUpdate = true;
	wxString queryText = "UPDATE OR INSERT INTO " + tableName + " (";
	for (auto attribute : m_metaObject->GetGenericAttributes()) {
		queryText += (firstUpdate ? "" : ",") + IMetaAttributeObject::GetSQLFieldName(attribute);
		if (firstUpdate) {
			firstUpdate = false;
		}
	}
	queryText += ") VALUES ("; bool firstInsert = true;
	for (auto attribute : m_metaObject->GetGenericAttributes()) {
		unsigned int fieldCount = IMetaAttributeObject::GetSQLFieldCount(attribute);
		for (unsigned int i = 0; i < fieldCount; i++) {
			queryText += (firstInsert ? "?" : ",?");
			if (firstInsert) {
				firstInsert = false;
			}
		}
	}

	queryText += ") MATCHING (";

	if (m_metaObject->HasRecorder()) {
		CMetaDefaultAttributeObject* attributeRecorder = m_metaObject->GetRegisterRecorder();
		wxASSERT(attributeRecorder);
		queryText += IMetaAttributeObject::GetSQLFieldName(attributeRecorder);
		CMetaDefaultAttributeObject* attributeNumberLine = m_metaObject->GetRegisterLineNumber();
		wxASSERT(attributeNumberLine);
		queryText += "," + IMetaAttributeObject::GetSQLFieldName(attributeNumberLine);
	}
	else
	{
		bool firstMatching = true;
		for (auto attribute : m_metaObject->GetGenericDimensions()) {
			queryText += (firstMatching ? "" : ",") + IMetaAttributeObject::GetSQLFieldName(attribute);
			if (firstMatching) {
				firstMatching = false;
			}
		}
	}

	queryText += ");";

	PreparedStatement* statement = databaseLayer->PrepareStatement(queryText);

	if (statement == NULL)
		return false;

	for (auto objectValue : m_aObjectValues) {

		if (hasError)
			break;

		int position = 1;

		for (auto attribute : m_metaObject->GetGenericAttributes()) {
			auto foundedKey = m_aKeyValues.find(attribute->GetMetaID());
			if (foundedKey != m_aKeyValues.end()) {
				IMetaAttributeObject::SetValueAttribute(
					attribute,
					foundedKey->second,
					statement,
					position
				);
			}
			else if (m_metaObject->IsRegisterLineNumber(attribute->GetMetaID())) {
				IMetaAttributeObject::SetValueAttribute(
					attribute,
					numberLine++,
					statement,
					position
				);
			}
			else {
				IMetaAttributeObject::SetValueAttribute(
					attribute,
					objectValue.at(attribute->GetMetaID()),
					statement,
					position
				);
			}
		}

		hasError = statement->RunQuery() == DATABASE_LAYER_QUERY_RESULT_ERROR;
	}

	databaseLayer->CloseStatement(statement);

	if (!hasError && !SaveVirtualTable())
		return false;

	if (!hasError && clearTable)
		m_aObjectValues.clear();
	else if (!clearTable)
		m_selected = true;

	return !hasError;
}

bool IRecordSetObject::DeleteData()
{
	wxString tableName = m_metaObject->GetTableNameDB();
	wxString queryText = "DELETE FROM " + tableName; bool firstWhere = true;
	for (auto attribute : m_metaObject->GetGenericDimensions()) {
		if (!IRecordSetObject::FindKeyValue(attribute->GetMetaID()))
			continue;
		if (firstWhere) {
			queryText = queryText + " WHERE ";
		}
		queryText = queryText +
			(firstWhere ? " " : " AND ") + IMetaAttributeObject::GetCompositeSQLFieldName(attribute);
		if (firstWhere) {
			firstWhere = false;
		}
	}

	PreparedStatement* statement = databaseLayer->PrepareStatement(queryText); int position = 1;

	if (statement == NULL)
		return false;

	for (auto attribute : m_metaObject->GetGenericDimensions()) {
		if (!IRecordSetObject::FindKeyValue(attribute->GetMetaID()))
			continue;
		IMetaAttributeObject::SetValueAttribute(
			attribute,
			m_aKeyValues.at(attribute->GetMetaID()),
			statement,
			position
		);
	}

	statement->RunQuery();
	statement->Close();

	return DeleteVirtualTable();
}

//**********************************************************************************************************
//*                                          Code generator												   *
//**********************************************************************************************************

#include "metadata/metadata.h"

CValue IRecordDataObjectRef::CCodeGenerator::GenerateCode() const
{
	wxASSERT(m_metaAttribute);

	wxString tableName = m_metaObject->GetTableNameDB();
	wxString fieldName = m_metaAttribute->GetFieldNameDB();

	IMetadata* metaData = m_metaObject->GetMetadata();
	wxASSERT(metaData);

	DatabaseResultSet* resultSet = databaseLayer->RunQueryWithResults("SELECT %s FROM %s ORDER BY %s;",
		IMetaAttributeObject::GetSQLFieldName(m_metaAttribute),
		tableName,
		IMetaAttributeObject::GetSQLFieldName(m_metaAttribute)
	);

	if (resultSet == NULL) {
		return m_metaAttribute->CreateValue();
	}

	number_t code = 1;

	if (m_metaAttribute->ContainType(eValueTypes::TYPE_NUMBER)) {
		while (resultSet->Next()) {
			CValue fieldCode =
				IMetaAttributeObject::GetValueAttribute(m_metaAttribute, resultSet);
			if (fieldCode == code) {
				code++;
			}
		}
		resultSet->Close();
		return code;
	}
	else if (m_metaAttribute->ContainType(eValueTypes::TYPE_STRING)) {
		ttmath::Conv conv;
		conv.precision = m_metaAttribute->GetLength();
		conv.leading_zero = true;
		wxString strCode = code.ToString(conv);

		while (resultSet->Next()) {
			CValue fieldCode = IMetaAttributeObject::GetValueAttribute(m_metaAttribute, resultSet);
			if (fieldCode == strCode) {
				code++;
				strCode = code.ToString(conv);
			}
		}
		resultSet->Close();
		return strCode;
	}

	wxASSERT_MSG(false, "m_metaAttribute->GetClsids() != eValueTypes::TYPE_NUMBER"
		"|| m_metaAttribute->GetClsids() != eValueTypes::TYPE_STRING");

	return CValue();
}